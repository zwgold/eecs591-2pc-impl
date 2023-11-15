CXX			= g++
CXXFLAGS = -std=c++20 -Wconversion -Wall -Werror -Wextra -pedantic 
EXECUTABLE 	= main

all: Coordinator Follower

Coordinator: run_coordinator.cpp
	$(CXX) $(CXXFLAGS) run_coordinator.cpp -o Coordinator.exe

Follower: run_follower.cpp
	$(CXX) $(CXXFLAGS) run_follower.cpp -o Follower.exe

clean:
	rm -rf *.exe *.dSYM

.PHONY: clean