#ifndef FOLLOWER_HPP_
#define FOLLOWER_HPP_

#include <iostream>
#include <vector>

class Follower {
public:

    Follower();

    void query_to_commit() {

    }

    void commit() {

    }

    void rollback() {

    }

    void end() {

    }

    void run() {
        
    }

    
};

#endif


/*
class Cohort(Thread):
    def __init__(self, uname, coord):
        Thread.__init__(self)
        self.uname = uname
        self.coord = coord
        self.do = None
        self.undo = None
        self.sem = Semaphore(0)
        self.lock = Lock()
        self.account = random.randint(MIN_ACCOUNT, MAX_ACCOUNT)
        self._log_extra = dict(user=uname)

    def query_to_commit(self):
        ## Voting phase:
        # 3. Each cohort replies with an agreement message (cohort votes Yes to
        # commit), if the cohort's actions succeeded, or an abort message
        # (cohort votes No, not to commit), if the cohort experiences a failure
        # that will make it impossible to commit.
        if self.res:
            LOG.info('vote YES', extra=self._log_extra)
            self.coord.yes()
        else:
            LOG.info('vote NO', extra=self._log_extra)
            self.coord.no()

    def commit(self):
        self.commit = True

    def rollback(self):
        self.commit = False

    def end(self):
        self.sem.release()

    def run(self):
        LOG.debug('BEFORE {}'.format(self.account), extra=self._log_extra)

        # executing operation and saving result
        self.lock.acquire()

        ## Voting phase:
        # 2. The cohorts execute the transaction up to the point where they
        # will be asked to commit. They each write an entry to their undo log
        # and an entry to their redo log.
        for do in self.do:
            do()

        self.res = self.account >= MIN_ACCOUNT and self.account <= MAX_ACCOUNT
        self.coord.start_voting(self)

        LOG.debug('DURING {}'.format(self.account), extra=self._log_extra)

        # waiting for the end of voting phase
        self.sem.acquire()
        ## Commit phase:
        if self.commit:
            # 2. Each cohort completes the operation ...
            LOG.info('commit', extra=self._log_extra)
        else:
            # 2. Each cohort undoes the transaction using the undo log ...
            for undo in self.undo:
                undo()
            LOG.info('rollback', extra=self._log_extra)
        # 2. ... and releases all the locks and resources held during the
        # transaction.
        self.lock.release()

        # 3. Each cohort sends an acknowledgment to the coordinator.
        self.coord.ack()

        LOG.debug('AFTER {}'.format(self.account), extra=self._log_extra)
        */