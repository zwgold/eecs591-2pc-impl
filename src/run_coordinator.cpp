#include <iostream>
#include <vector>

#include "Coordinator.hpp"

// Usage: ./Coordinator hostname portnum
int main(void) {
    Coordinator c("coordinator_log.txt");

    c.send_vote_req();

    return 0;
}