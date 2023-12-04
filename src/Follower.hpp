#ifndef FOLLOWER_HPP_
#define FOLLOWER_HPP_


// C Header Files
#include <arpa/inet.h>
#include <netdb.h>  
#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


// C++ Header Files
#include <chrono>
#include <string>

#include "Logger.hpp"

class Follower {
private:
    // Socket variables
    std::string host;
    uint16_t port;
    int socketfd;
    struct sockaddr_in addr;

    // Voting and Coordinator
    std::string vote;
    bool rewaitForCoord = false;

    // Personal Info (Logging and name)
    Logger dtlog;
    std::string name;

    int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, uint16_t port) {
        // Step (1): Specify Socket Family
        addr->sin_family = AF_INET;

        // Step (2): Specify Socket Address (hostname).
        struct hostent *host = gethostbyname(hostname);
        if (host == NULL) {
            fprintf(stderr, "%s: unknown host\n", hostname);
            return -1;
        }
        memcpy(&addr->sin_addr, host->h_addr, host->h_length);

        // Step (3): Set the port value.
        addr->sin_port = htons(port);

        return 0;
    }

public:
    Follower(std::string &hostname, uint16_t &port_in, std::string& name_in) : host(hostname), port(port_in), dtlog("follower_" + name_in + ".txt"), name(name_in) {
        socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (make_client_sockaddr(&addr, host.c_str(), port) == -1) {
            exit(1);
        }

        // Set timeout value for socket
        /*struct timeval tv;
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);*/
    }

    ~Follower() {
        close(socketfd);
    }

    // Establish connection to coordinator
    void connectToCoordinator() {
        if (connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("Error connecting stream socket");
            exit(1);
        }
    }

    // Wait for the vote request and send vote
    void run() {
        rewaitForCoord = false;
        // Part 1: Wait for the VOTE-REQ from the Coordinator
        char msg[101];
        memset(msg, 0, sizeof(msg));
        size_t recvd = 0;
        ssize_t rval;

        auto timeNow = std::chrono::high_resolution_clock::now();
        long int diff;
        do {
            rval = recv(socketfd, msg + recvd, 100 - recvd, 0);
            if (rval == -1) {
                perror("Error reading stream message");
                exit(1);
            }
            recvd += rval;
            diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeNow).count(); 
        } while (rval > 0 && recvd < 8 && diff < 5000); // timeout of 5000ms = 5 sec

        // Timeout Action: We have not received anything fully from the coordinator
        // so trigger a timeout
        if (diff >= 5000 && recvd < 8) {
            vote = "ABORT";
            // This isn't our vote, this is our timeout
            dtlog.log(vote);
            return;
        }

        // Remove the VOTE_REQ part, we know that we are voting once we get it
        std::string participants(msg);
        participants.erase(0, 8);
        dtlog.log("PARTICIPANTS:" + participants);

        // Part 3: Send the vote, seed it randomly for each follower
        uint t = (uint)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        srand(t);

        // Figure out the vote
        vote = (rand() % 10) ? "COMMIT" : "ABORT"; // 9/10 chance of being a COMMIT
        dtlog.log("VOTE: " + vote);

        // Send the vote with the name of the participant
        std::string toSend = vote + " " + name;
        size_t message_len = strlen(toSend.c_str());
        size_t sent = 0;
        do {
            ssize_t n = send(socketfd, toSend.c_str() + sent, message_len - sent, 0);
            if (n == -1) {
                perror("Error sending on stream socket");
                break;
            }
            sent += n;
        } while (sent < message_len);
    }

    const std::string getVote() const {
        return vote;
    }

    // Termination Protocol
    bool getCoordStatus() const {
        return rewaitForCoord;
    }


    // After vote sent, if we did a commit, we will still listen and want to get the final verdict.
    // Have to account for termination protocol if a timeout occurs.
    void getAction() {
        char msg[101];
        memset(msg, 0, sizeof(msg));
        size_t recvd = 0;
        ssize_t rval;

        auto timeNow = std::chrono::high_resolution_clock::now();
        long int diff;
        do {
            rval = recv(socketfd, msg + recvd, 100 - recvd, 0);
            if (rval == -1) {
                perror("Error reading stream message");
                exit(1);
            }
            recvd += rval;
            diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeNow).count(); 
        } while (rval > 0 && recvd < 7 && diff < 5000); // timeout of 5000ms = 5 sec

        // TERMINATION PROTOCOL
        if (diff >= 5000 && (recvd != 6 && recvd != 5)) {
            rewaitForCoord = true;
        }
        else {
            std::string action(msg);
            dtlog.log("DECISION: " + action);
        }
    }
    
};

#endif