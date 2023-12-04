#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <iostream>
#include <fstream>
#include <string>

class Logger {
private:
    std::string logfile;
    std::ifstream readstream;
    std::ofstream writestream;

public:
    Logger(std::string logfile) : logfile(logfile) {
        readstream = std::ifstream(logfile);
        writestream = std::ofstream(logfile);

        if(!(readstream.is_open() && writestream.is_open())) {
            throw "Failed to open logfile";
        }
    }

    Logger& operator=(Logger&) = delete;
    Logger(Logger&) = delete;

    ~Logger() {
        readstream.close();
        writestream.close();
    }

    void log(std::string to_write) {
        writestream << to_write << "\n";
    }

    // REQUIRES: logfile is cleared after each transaction
    bool log_contains(std::string query) {
        bool found = false;
        std::string line;
        while (getline(readstream, line)) {
            if (line == query) {
                found = true;
                break;
            }
        }

        readstream.clear();
        readstream.seekg (0, std::ios::beg);
        return found;
    } 
};

#endif