#ifndef FOLLOWER_HPP_
#define FOLLOWER_HPP_


// C Header Files
#include <arpa/inet.h>
#include <netdb.h>  
#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


// C++ Header Files
#include <chrono>
#include <string>

#include "Logger.hpp"

class Follower {
private:
    // Socket variables
    std::string host;
    uint16_t port;
    int socketfd;
    struct sockaddr_in addr;

    std::string vote;

    Logger dtlog;

    int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, uint16_t port) {
        // Step (1): Specify Socket Family
        addr->sin_family = AF_INET;

        // Step (2): Specify Socket Address (hostname).
        struct hostent *host = gethostbyname(hostname);
        if (host == NULL) {
            fprintf(stderr, "%s: unknown host\n", hostname);
            return -1;
        }
        memcpy(&addr->sin_addr, host->h_addr, host->h_length);

        // Step (3): Set the port value.
        addr->sin_port = htons(port);

        return 0;
    }

public:
    Follower(std::string &hostname, uint16_t& port_in, std::string logfile) : host(hostname), port(port_in), dtlog(logfile) {
        socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (make_client_sockaddr(&addr, host.c_str(), port) == -1) {
            exit(1);
        }
    }

    ~Follower() {
        close(socketfd);
    }

    // Establishes connection to coordinator
    void connectToCoordinator() {
        if (connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("Error connecting stream socket");
            exit(1);
        }
    }

    // Wait for the vote request and send it
    void init_vote() {
        // Part 1: Wait for the VOTE-REQ
        char msg[101];
        memset(msg, 0, sizeof(msg));
        size_t recvd = 0;
        ssize_t rval;
        do {
            rval = recv(socketfd, msg + recvd, 100 - recvd, 0);
            if (rval == -1) {
                perror("Error reading stream message");
                exit(1);
            }
            recvd += rval;
        } while (rval > 0 && recvd != 8);

        // Print out that we got the messages
        std::string message(msg);
        std::cout << msg << std::endl;

        // Part 2: Send the vote, seed it randomly for each follower
        uint t = (uint)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        srand(t);
        vote = (rand() % 3) ? "COMMIT" : "ABORT"; // 2/3 chance of being a COMMIT
        size_t message_len = strlen(vote.c_str());
        size_t sent = 0;
        do {
            ssize_t n = send(socketfd, vote.c_str() + sent, message_len - sent, 0);
            if (n == -1) {
                perror("Error sending on stream socket");
                exit(1);
            }
            sent += n;
        } while (sent < message_len);
    }

    const std::string getVote() const {
        return vote;
    }

    void commit_phase() {

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