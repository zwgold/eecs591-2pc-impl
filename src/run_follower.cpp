#include <iostream>
#include <vector>

#include "Follower.hpp"

// Usage: ./Follower hostname portnum
// hostname and port are for the server
int main(int argv, char** argc) {
    if (argv != 3) {
        std::cout << "Usage: ./Follower hostname portnum" << std::endl;
        return -1;
    }

    std::string hostname = argc[1];
    uint16_t port = (uint16_t)std::atoi(argc[2]);
    Follower c(hostname, port);

    return 0;
}