#include <iostream>
#include <unordered_map>
#include <vector>

class Coordinator {
    private:

    std::vector<int> votes;
    std::vector<int> acks;
    std::vector<int> cohorts;
    public:

    Coordinator() {}

    void yes() {
        votes.push_back(1);
    }

    void no() {
        votes.push_back(0);
    }

    void ack() {
        acks.push_back(1); // 1 == True
    }

    void start_vote() {

    }

    void run() {

    }
};

class Follower {

};


int main() {
    
}