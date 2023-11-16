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
#include <chrono>
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
  static constexpr uint16_t NUM_PARTICIPANTS = 1;
  std::vector<int> acks;
  std::vector<int> votes;

  std::vector<int> connectionFds;
  std::vector<std::thread> connectionThreads;

  int handle_connection(int connectionfd, std::string messageSend) {
    // Send the VOTE-REQ
    size_t message_len = strlen(messageSend.c_str());
    size_t sent = 0;
    do {
      ssize_t n =
          send(connectionfd, messageSend.c_str() + sent, message_len - sent, 0);
      if (n == -1) {
        perror("Error sending on stream socket");
        return -1;
      }
      sent += n;
    } while (sent < message_len);

    // Now, wait for the vote
    char msg[101];
    memset(msg, 0, sizeof(msg));
    // Call recv() enough times to consume all the data the client sends.
    size_t recvd = 0;
    ssize_t rval;
    do {
      // Receive as many additional bytes as we can in one call to recv()
      // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
      rval = recv(connectionfd, msg + recvd, 100 - recvd, MSG_WAITALL);
      if (rval == -1) {
        perror("Error reading stream message");
        exit(1);
      }
      recvd += rval;
    } while (rval > 0); // recv() returns 0 when client closes

    // Get the vote, store as a 1 (commit) or 0 (abort)
    std::string voteType(msg);
    std::cout << voteType << std::endl;
    if (voteType == "COMMIT") {
      votes.push_back(1);
    } else {
      votes.push_back(0);
    }

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
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yesval,
                   sizeof(yesval)) == -1) {
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

  Coordinator &operator=(Coordinator &) = delete;
  Coordinator(Coordinator &) = delete;

  ~Coordinator() { close(socketfd); }

  void send_and_get_vote() {
    std::string voteReqMsg = "VOTE-REQ";

    dtlog.log(voteReqMsg);

    // TODO: Do we need multithreading
    auto timeNow = std::chrono::high_resolution_clock::now();
    long int diff;
    do {
      int connectionfd = accept(socketfd, 0, 0);
      if (connectionfd == -1) {
        perror("Error accepting connection");
        exit(1);
      }

      connectionFds.push_back(connectionfd);

      if (handle_connection(connectionfd, "VOTE_REQ") == -1) {
        exit(1);
      }

      // std::thread connect (&Coordinator::handle_connection, this,
      // connectionfd); connectionThreads.push_back(std::move(connect));
      diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                 std::chrono::high_resolution_clock::now() - timeNow)
                 .count();
    } while (connectionFds.size() != NUM_PARTICIPANTS && diff < 5000);

    // Failed to get all the votes
    if (connectionFds.size() != NUM_PARTICIPANTS && diff >= 5000) {
      abort();
    }

    for (auto connect : connectionFds) {
      close(connect);
    }

    /*for (size_t i = 0; i < connectionThreads.size(); i++) {
        connectionThreads[i].join();
    }*/
    // TODO: write then log

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
};

#endif