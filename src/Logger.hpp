#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <iostream>
#include <fstream>
#include<string>

using namespace std;

class Logger {
private:
    string logfile;
    ifstream readstream;
    ofstream writestream;

public:
    Logger(string logfile_in) logfile(logfile_in) {
        readstream = ifstream(logfile);
        writestream = ofstream(logfile);

        readstream.open();
        writestream.open();

        if(!(readstream.is_open() and writestream.is_open())) {
            throw "Failed to open logfile";
        }
    }

    Logger& operator=(Logger&) = delete;
    Logger(Logger&) = delete;

    ~Logger() {
        readstream.close();
        writestream.close();
    }



    void write(string to_write) {
        writestream << to_write << "\n";
    }


    //REQUIRES: logfile is cleared after each transaction
    bool log_contains(string query) {
        bool found = false;
        string line;
        while (getline(readstream, line)) {
            if(line == query) {
                found = true;
            }
        }

        readstream.clear();
        readstream.seekg (0, ios::beg);
        return found;
    } 
};

#endif