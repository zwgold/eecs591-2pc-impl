#include <iostream>
#include <vector>

#include "Follower.hpp"

// NOTE TO GROUP: IF TESTING IN MININET: USE 10.0.0.1 AND 8888 FOR HOSTNAME AND PORT, RESPECTIVELY
// Usage: ./Follower hostname portnum followername
// hostname and port are for the server 
int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "Usage: ./Follower [hostname] [portnum] [followername]" << std::endl;
        return -1;
    }

    // Parse Input
    std::string hostname = argv[1];
    uint16_t port = (uint16_t)std::atoi(argv[2]);
    std::string name = argv[3];

    // Construct Follower Object
    Follower c(hostname, port, name);

    // Connect and run initial parts
    c.connectToCoordinator();
    c.run();
    
    // If we commit, we have to hear what the final result is
    // If we abort, we can just abort and not worry
    if (c.getVote() == "COMMIT") {
        c.getAction();
        // If we failed, we have to retry
        while (c.getCoordStatus()) {
            c.connectToCoordinator();
            c.run();
            if (c.getVote() == "COMMIT") {
                c.getAction();
            }
        }
    }

    return 0;
}
