# 2-Phase Commit C++ Implementation
This implementation of 2 Phase Commit (2PC) is a basic example to use for our EECS 591 Project focusing on eBPF optimizations based on the Electrode paper. (Originally designed for optimizing Multi-Paxos.) This README will also discuss the extensions done to Electrode to accompany this project.

## Structure
### `topology.py`
This file will set up the Mininet instance to test the implementation. An image of what the topology looks like is provided in `topology.png` for reference. To run you will use the command `sudo python3 toplology.py` to create the toplology. From there, use Miniet commands to run and set up the Coordinator/Follower scheme. (For instance, to open up XTerminals for different nodes, you can run `xterm h1 h2 h3` to launch hosts 1 through 3)

You can also omit this and run `sudo mn`, which spins up a simple Mininet topology of 2 hosts and a switch. From here, you may add your own configurations. (Or, you can rewrite `topology.py`/your own file to create one)

### `src/Coordinator.hpp`
This header file contains the implementation of the Coordinator for 2PC. How this works is a Coordinator class on start up is passed in with the log file they write to. (We default use `coordinator_log.txt`) Then, the Coordinator creates a socket on start up that listens for multiple connections, accepting each one by one (until we either hit the number of participants needed OR when the timeout variable is met)

When servicing followers, the coordinator follows the standard 2PC protocol for interacting with followers. (Sending a vote, waiting for replies, aggregating) Appropriate logging and timeout protocols are implemented, as well. 

NOTE: We usually use node `h1` for the Coordinator. This corresonds to IP (in Mininet) `10.0.0.1`, used for the `hostname` argument for running the Follower. (see below)

### `src/run_coordinator.cpp`
This contains the main function to run the Coordinator. Once compiled (via `make` in `\src`) you can run this using `./Coordinator.exe [num_followers]`. `num_followers` is the number of followers we want our Coordinator to deal with.

### `src/Follower.hpp`
This header file contains the implementation of the Follower for 2PC. How this works is a Follower class on start up, with appropriate arguments (specified below) is instantiated, with a log file created. (`follower_followername.txt`)
When running, a Follower will perform the same 2PC tasks of receiving a vote, voting (halting when it is an ABORT), and waiting for the final vote. Appropriate logging and timeout protocols are implemented. 

### `src/run_follower.cpp`
This contains the main function to run a Follower. Once compiled (via `make` in `\src`) you can run this using `./Follower.exe [hostname] [portnum] [followername]`. `hostname` specifies the IP Address of the Coordinator. `portnum` specifies the port that the Coordinator is on. (We use `8888`) `followername` is the name of the follower on that machine, which is used for debugging purposes. (In a more realistic system, we could use `gethostname` for each different machine. That does not work for our testing protocol)

### `src/Logger.hpp`
This file defines the logging mechanisms used in 2PC. This includes simple file writing and reading.

### `Electrode-Extension`
This is the repository for our fork of the original Electrode paper's repo. This is a git submodule to reduce the amount of code we would have to include in the ZIP file. Description of the code is detailed in the original paper. The important files to note are as follows:
* `setup.sh` is a Bash script which dynamically checks for proper kernel versioning and runs the required setup for clients and replicas as stated in the Electrode README.
* `gendata.py` is a custom Python script to automatically generate latency and throughput data for a set amount of replicas on CloudLab, modelling the methods of data collection stated in the paper.
* `opt3replica.csv` is the raw data generated from the `gendata.py` script for 3 replicas. Analogously, `opt5replica.csv` is the raw data for 5 replicas.
* `maxtput.csv` is a sample of attempts at maximizing throughput. The data shown caused the CloudLab servers to crash multiple times, hence our reasoning for not including maximizing throughput in our data.

### `Miscellaneous`
* `eBPF Final Report`: This contains the final report for the EECS 591 project
* `Instructions for Running Electrode on CloudLab`: A personalized guide that our group wrote to demonstrate how to properly set up an Electrode experiment to be run on CloudLab.
* `Email Correspondences`: Records of email communication with the lead code manager for Electrode, Yang Zhou (yangzhou@g.harvard.edu).

### Notes
* To run, run `Coordinator.exe` FIRST, and then each Follower. (running a Follower without a Coordinator will cause it to fail)
* Timeouts are 5 seconds for all related parts.
* Socket timeouts are 1 seconds to deal with conditions for multithreading.
* Participant lists are calculated by those who have sent a vote already. This can mean we send vote request before we can properly include the recently obtained vote. This is ok, since this better reflects a multithreaded system.

## Potential Changes
* Update termination protocol to rely on the logging of other followers. We omitted this since our system really did not need this part.
* Have the port configurable rather than just 8888.
* Look into changes for socket timeouts when handling multi-threaded code.
* Explore potential fine-grained locking at the Coordinator level to improve participant list sent. (Perhaps we can just have a text file of all Follower names for reference to send out, but we felt this was not in the spirit of distributed design)
* Have Followers wait indefinitely until Coordinator is live, rather than requiring the Coordinator to run first and then have Followers run.
* Creat TCP Object that covers the socket code to make things more clean and OOP.