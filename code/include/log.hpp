// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstdint>
#include <list>
#include <queue>
#include <string>
#include "include/defs.hpp"

namespace pkr {

class Logger {
public:
    ~Logger();
    void set_file(const std::string&);
    void write_to_file();

    void access(const std::string&, Port);
    void error(const std::string&);
    void fatal_error(const std::string&);
    void message(const std::string&);
    void print(const std::string&);

    static std::string errstr();

private:
    void add_rec(const std::string&);

private:
    std::string log_file = "log.txt";
    std::queue<std::string, std::list<std::string>> records;
};

extern Logger log;

}  // namespace pkr
