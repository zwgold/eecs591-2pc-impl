#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

// C Header Files
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// C++ Header Files
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <semaphore>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// #include "Follower.hpp"
#include "Logger.hpp"

class Coordinator {
private:
    // Socket Stuff
    std::string host;
    static constexpr uint16_t PORT = 8888;
    int socketfd;
    struct sockaddr_in address;

    // Logging
    Logger dtlog;

    // Protocol
    std::vector<int> acks;
    std::vector<int> votes;

    int handle_connection(int connectionfd) {
        std::string voteReqMsg = "VOTE-REQ";
        //[[maybe_unused]] ssize_t rval = send(connectionfd, voteReqMsg.c_str(), sizeof(voteReqMsg.c_str()), MSG_WAITALL);

        size_t message_len = strlen(voteReqMsg.c_str());
        size_t sent = 0;
        do {
            std::cout << voteReqMsg.c_str() + sent << std::endl;
            ssize_t n = send(connectionfd, voteReqMsg.c_str() + sent, message_len - sent, 0);
            if (n == -1) {
                perror("Error sending on stream socket");
                return -1;
            }
            sent += n;
        } while (sent < message_len);


        // (4) Close connection
        close(connectionfd);
        return 0;
    }

public:
    Coordinator(std::string logfile) : dtlog(logfile) {
        socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (socketfd == -1) {
            perror("Error opening stream socket");
            exit(1);
        }

        int yesval = 1;
        if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1) {
            perror("Error setting socket options");
            exit(1);
        }

        (&address)->sin_family = AF_INET;
        (&address)->sin_addr.s_addr = INADDR_ANY;
        (&address)->sin_port = htons(PORT);

        if (bind(socketfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
            perror("Error binding stream socket");
            exit(1);
        }
        
        listen(socketfd, 10);
  }

    ~Coordinator() {
        close(socketfd);
    }

  void send_vote_req() {
    std::string voteReqMsg = "VOTE-REQ";

    while (1) {
        int connectionfd = accept(socketfd, 0, 0);
        if (connectionfd == -1) {
            perror("Error accepting connection");
            exit(1);
        }

        if (handle_connection(connectionfd) == -1) {
            exit(1);
        }
  }


    // TODO: write then log
    dtlog.log(voteReqMsg);

    // (2) Create a sockaddr_in to specify remote host and port

    // (4) Send message to remote server
    // Call send() enough times to send all the data
    /*ssize_t sent = 0;
    do {
        const ssize_t n = send(master_socketfd, str.c_str() + sent, str.size() -
    sent, 0); if (n == -1) { perror("Error sending on stream socket"); return;
        }
        sent += n;
    } while (sent < (ssize_t)str.size());
    */
  }

  void commit() {
    std::string commitMsg = "COMMIT";
    // TODO: send message AFTER log
    dtlog.log(commitMsg);
  }

  void abort() {
    std::string abortMsg = "ABORT";
    // TODO: send message before log
    dtlog.log(abortMsg);
  }
  // void start_vote(Follower& follower);

  void run();
};

#endif