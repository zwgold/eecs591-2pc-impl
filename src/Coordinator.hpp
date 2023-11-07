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

//#include "Follower.hpp"


class Coordinator {
private:
    std::string host;
    static constexpr uint16_t PORT = 8888;
    static constexpr uint8_t MAX_CLIENTS = 30;
    std::array<int, MAX_CLIENTS> client_sockets;
    int master_socketfd;
    struct sockaddr_in* address;


public:
    Coordinator() {
        int master_socket;
        //[[unused]] int addrlen, activity, valread;

        int yes = 1;
        // create a master socket
        master_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (master_socket <= 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // set master socket to allow multiple connections ,
        // this is just a good habit, it will work without this
        int success = setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (success < 0) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        // type of socket created
        address->sin_family = AF_INET;
        address->sin_addr.s_addr = INADDR_ANY;
        address->sin_port = htons(PORT);

        // bind the socket to localhost port 8888
        success = ::bind(master_socket, (struct sockaddr *)address, sizeof(*address));
        if (success < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }socket
        printf("---Listening on port %d---\n", PORT);

        // try to specify maximum of 3 pending connections for the master socket
        if (listen(master_socket, 3) < 0) {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }

    ~Coordinator() {}

    void send_vote_req() {
        std::string str = "VOTE-REQ";

        // (2) Create a sockaddr_in to specify remote host and port


        // (4) Send message to remote server
        // Call send() enough times to send all the data
        /*ssize_t sent = 0;
        do {
            const ssize_t n = send(master_socketfd, str.c_str() + sent, str.size() - sent, 0);
            if (n == -1) {
                perror("Error sending on stream socket");
                return;
            }
            sent += n;
        } while (sent < (ssize_t)str.size());
        */
    }

    //void start_vote(Follower& follower);

    void run();

    std::counting_semaphore<2> coor{0};

    //std::unordered_map<
    std::vector<int> acks;
    std::vector<int> votes;
    //std::vector<Follower> followers;
};

#endif