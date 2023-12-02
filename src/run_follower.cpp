#include <iostream>
#include <vector>

#include "Follower.hpp"

// Usage: ./Follower followername hostname portnum
// hostname and port are for the server

// IF TESTING IN MININET: USE 10.0.0.1 AND 8888 FOR HOSTNAME AND PORT, RESPECTIVELY
int main(int argc, char **argv) {
    // TODO: Add stuff for taking in follower #
    if (argc != 4) {
        std::cout << "Usage: ./Follower folowername hostname portnum" << std::endl;
        return -1;
    }

    std::string hostname = argv[1];
    uint16_t port = (uint16_t)std::atoi(argv[2]);
    std::string filename = "follower.txt";
    std::string name = argv[3];
    Follower c(hostname, port, filename, name);
    c.connectToCoordinator();
    c.run();

    return 0;
}
