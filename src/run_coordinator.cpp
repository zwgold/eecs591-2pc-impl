#include <iostream>
#include <vector>

#include "Coordinator.hpp"

// Usage: ./Coordinator num_followers
int main(int argc, char** argv) {
    // Construct Coordinator Object,
    if (argc != 2) {
        std::cout << "Usage: ./Coordinator [num_followers]" << std::endl;
        return -1;
    }

    // Parse Input
    uint16_t num_followers = (uint16_t)std::atoi(argv[1]);
    Coordinator c("coordinator_log.txt", num_followers);

    // Runs
    c.run();

    return 0;
}