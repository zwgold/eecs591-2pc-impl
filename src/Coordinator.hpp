#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

#include <iostream>
#include <vector>

#include "Follower.hpp"


class Coordinator {
public:

    Coordinator();

    void yes();

    void no();

    void ack();

    void start_vote();

    void run();


    std::vector<int> acks;
    std::vector<int> votes;
    std::vector<Follower> followers;
};

#endif