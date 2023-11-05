#include <algorithm>
#include <iostream>
#include <semaphore>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Coordinator.hpp"
#include "Follower.hpp"


// 1 --> Yes/True, 0 --> No/False

Coordinator::Coordinator() {}

void Coordinator::yes() {
    votes.push_back(1);
}

void Coordinator::no() {
    votes.push_back(0);
}

void Coordinator::ack() {
    acks.push_back(1);
}

void Coordinator::start_vote(Follower& follower) {
    followers.push_back(follower);
    coor.release();
}

void Coordinator::run() {
    // Acquire semaphore
    coor.acquire();


    /*for (auto follower : followers) {
        // call function here
    }*/

    if ((long int)votes.size() == std::count_if(votes.begin(), votes.end(), [](int i) { return i == 1; })) {

    }
}

/*


class Coordinator(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.start_sem = Semaphore(0)
        self.cohorts = []
        self.votes = []
        self.acks = []
        self._log_extra = dict(user='COORD')

    def yes(self):
        self.votes.append(True)

    def no(self):
        self.votes.append(False)

    def ack(self):
        self.acks.append(True)

    def start_voting(self, cohort):
        self.cohorts.append(cohort)
        self.start_sem.release()

    def run(self):
        self.start_sem.acquire(NO_COHORTS)

        ## Voting Phase:
        # 1. The coordinator sends a query to commit message to all cohorts and
        # waits until it has received a reply from all cohorts.
        for cohort in self.cohorts:
            LOG.info('query_to_commit to {}'.format(cohort.uname), extra=self._log_extra)
            cohort.query_to_commit()

        ## Commit Phase:
        # If the coordinator received an agreement message from all cohorts
        # during the commit-request phase
        if all(self.votes):
            # 1. The coordinator sends a commit message to all the cohorts.
            LOG.info('Committing', extra=self._log_extra)
            for cohort in self.cohorts:
                cohort.commit()
        # If any cohort votes No during the commit-request phase (or the
        # coordinator's timeout expires)
        else:
            # 1. The coordinator sends a rollback message to all the cohorts.
            LOG.warning('Rolling back', extra=self._log_extra)
            for cohort in self.cohorts:
                cohort.rollback()

        if all(self.acks):
            LOG.info('END', extra=self._log_extra)
        else:
            LOG.error('KO something went wrong while receiving acks', extra=self._log_extra)

        for cohort in self.cohorts:
            cohort.end()
            
            */