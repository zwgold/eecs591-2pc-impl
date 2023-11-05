#ifndef COORDINATOR_HPP_
#define COORDINATOR_HPP_

#include <algorithm>
#include <iostream>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Follower.hpp"


class Coordinator : public std::thread {
private:
    static constexpr uint8_t NO_COHORTS = 2;

public:

    Coordinator();

    void yes();

    void no();

    void ack();

    void start_vote(Follower& follower);

    void run();

    std::counting_semaphore<2> coor{0};

    //std::unordered_map<
    std::vector<int> acks;
    std::vector<int> votes;
    std::vector<Follower> followers;
};

#endif