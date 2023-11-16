#include <iostream>
#include <vector>

#include "Coordinator.hpp"

// Usage: ./Coordinator hostname portnum
int main(void) {
    Coordinator c("coordinator_log.txt");

    c.send_and_get_vote();

    return 0;
}