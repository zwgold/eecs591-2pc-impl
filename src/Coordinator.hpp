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
#include <mutex>
#include <numeric>
#include <semaphore>
#include <string>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

// #include "Follower.hpp"
#include "Logger.hpp"

class Coordinator {
private:
  // Socket Variables that are kept around
  std::string host;
  static constexpr uint16_t PORT = 8888;
  int socketfd;
  struct sockaddr_in address;

  // Logging
  Logger dtlog;

  // Protocol
  static constexpr uint16_t NUM_PARTICIPANTS = 1;

  // Connection Info
  std::vector<int> connectionFds;
  std::unordered_map<int, std::string> connectionToHostname;
  std::unordered_map<int, int> connectionToVote;

  // Threads
  std::vector<std::thread> connectionThreads;
  std::mutex m;

  void handle_connection(int connectionfd, std::string messageSend) {
    // 1. Send the VOTE-REQ and a list of participants. This can be done via a delimitter
    // and will need to be locked to prevent this from being modified. This should be the only
    // section that needs to be locked.
    m.lock();
    if (connectionToHostname.size() > 0) {
      messageSend += " "; // add delimitter
    }
    for (auto &[conn, hostname] : connectionToHostname) {
      messageSend += hostname;
      messageSend += " ";
    }
    m.unlock();

    size_t message_len = strlen(messageSend.c_str());
    size_t sent = 0;
    do {
      ssize_t n = send(connectionfd, messageSend.c_str() + sent, message_len - sent, 0);
      if (n == -1) {
        perror("Error sending on stream socket");
        connectionFds.erase(std::remove(connectionFds.begin(), connectionFds.end(), connectionfd), connectionFds.end());
        close(connectionfd);
        return;
      }
      sent += n;
    } while (sent < message_len);


    // 2. Wait for vote and name of participant. From there, if a vote is obtained, we can split it and 
    // add it to our counts and host info for sending participants earlier. (Assume when we first start,
    // participant list is empty)
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
        connectionFds.erase(std::remove(connectionFds.begin(), connectionFds.end(), connectionfd), connectionFds.end());
        close(connectionfd);
        return;
      }
      recvd += rval;
    } while (rval > 0 && recvd < 5); // recv() returns 0 when client closes

    // Store as string, then
    // split into the vote and name separately
    std::string voteAndName(msg);
    std::string delimiter = " ";
    size_t pos = voteAndName.find(delimiter);
    std::string voteType = voteAndName.substr(0, pos);
    voteAndName.erase(0, pos + delimiter.length());
    pos = voteAndName.find(delimiter);
    std::string connName = voteAndName.substr(0, pos);

    // For when we send list of participants
    connectionToHostname[connectionfd] = connName;
    // Store vote
    if (voteType == "COMMIT") {
      connectionToVote[connectionfd] = 1;
    } else {
      connectionToVote[connectionfd] = 0;
    }
  }

public:
  Coordinator(std::string logfile) : dtlog(logfile) {
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socketfd == -1) {
      perror("Error opening stream socket");
      exit(1);
    }

    int yesval = 1;

    // Set timeout value for socket
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    // Set reusable address for socket
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

  void run() {
    // Perform the first log
    dtlog.log("START-2PC");

    // Start accepting and making threads
    std::vector<std::thread> threads;
    auto timeNow = std::chrono::high_resolution_clock::now();
    long int diff;
    
    do {
      int connectionfd = accept(socketfd, 0, 0);
      if (connectionfd == -1) {
        perror("Error accepting connection");
      }
      else {
        // Spawn Thread, store connection info for later
        connectionFds.push_back(connectionfd);
        std::thread connectThread(&Coordinator::handle_connection, this, connectionfd, "VOTE_REQ");
        threads.push_back(std::move(connectThread));
      }

      diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timeNow).count();
    } while (connectionFds.size() != NUM_PARTICIPANTS && diff < 5000);

    // Finish off the initial connections, don't want threads to hang without calling join()
    for (auto& t : threads) {
      t.join();
    }

    // Failed to get all the votes and timeout occured
    if (connectionFds.size() != NUM_PARTICIPANTS && diff >= 5000) {
      abort();
      return;
    }

    int count = 0;
    for (auto &[k, v] : connectionToVote) {
      count += v;
    }

    
    // If we have enough Yes's, then we can commit!
    if (count == NUM_PARTICIPANTS) {
      commit();
    }  
    else {
      abort();
    }
  }

  void commit() {
    std::string commitMsg = "COMMIT";
    // TODO: send message AFTER log
    dtlog.log("DECISION: " + commitMsg);
    // Send this to all
    for (auto connectionfd : connectionFds) {
      // We will commit regardless
      size_t message_len = strlen(commitMsg.c_str());
      size_t sent = 0;
      do {
        ssize_t n = send(connectionfd, commitMsg.c_str() + sent, message_len - sent, 0);
        if (n == -1) {
          perror("Error sending on stream socket");
          break;
        }
        sent += n;
      } while (sent < message_len);
      close(connectionfd);
      connectionToVote.erase(connectionfd);
      connectionToHostname.erase(connectionfd);
    }
    connectionFds.clear();
  }

  void abort() {
    std::string abortMsg = "ABORT";
    // TODO: send message before log
    dtlog.log("DECISION: " + abortMsg);
    for (auto connectionfd : connectionFds) {
      if (connectionToVote[connectionfd] == 1) { // 1 is yes
        size_t message_len = strlen(abortMsg.c_str());
        size_t sent = 0;
        do {
          ssize_t n = send(connectionfd, abortMsg.c_str() + sent, message_len - sent, 0);
          if (n == -1) {
            perror("Error sending on stream socket");
            break;
          }
          sent += n;
        } while (sent < message_len);
      }
      close(connectionfd);
      connectionToVote.erase(connectionfd);
      connectionToHostname.erase(connectionfd);
    }
    connectionFds.clear();
  }
};

#endif