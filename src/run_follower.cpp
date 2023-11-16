#include <iostream>
#include <vector>

#include "Follower.hpp"

// Usage: ./Follower hostname portnum
// hostname and port are for the server

// IF TESTING IN MININET: USE 10.0.0.1 AND 8888 FOR HOSTNAME AND PORT, RESPECTIVELY
int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage: ./Follower hostname portnum" << std::endl;
        return -1;
    }

    std::string hostname = argv[1];
    uint16_t port = (uint16_t)std::atoi(argv[2]);
    Follower c(hostname, port, "follower.txt");
    c.connectToCoordinator();
    c.init_vote();

    return 0;
}