CXX			= g++
CXXFLAGS = -std=c++20 -Wconversion -Wall -Werror -Wextra -pedantic 
EXECUTABLE 	= main

all: main

main: main.cpp Coordinator.cpp
	$(CXX) $(CXXFLAGS) main.cpp Coordinator.cpp -o $(EXECUTABLE)

clean:
	rm -rf main *.dSYM

.PHONY: clean