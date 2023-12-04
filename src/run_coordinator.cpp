#include <iostream>
#include <vector>

#include "Coordinator.hpp"

// Usage: ./Coordinator
int main(void) {
    // Construct Coordinator Object,
    Coordinator c("coordinator_log.txt");

    // Run (will run until termination)
    c.run();

    return 0;
}