CXX			= g++
CXXFLAGS = -std=c++20 -Wconversion -Wall -Werror -Wextra -pedantic 
EXECUTABLE 	= main

all: Coordinator

Coordinator: run_coordinator.cpp
	$(CXX) $(CXXFLAGS) run_coordinator.cpp -o Coordinator

clean:
	rm -rf Coordinator *.dSYM

.PHONY: clean