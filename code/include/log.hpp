// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstdint>
#include <list>
#include <queue>
#include <string>
#include "include/singleton.hpp"

namespace pkr {

class Logger {
protected:
    friend class Singleton<Logger>;
    Logger() = default;

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    ~Logger();

    void set_file(const std::string&);

    void access(const std::string&, uint32_t);
    void error(const std::string&);
    void fatal_error(const std::string&);
    void message(const std::string&);

    void write_to_file();

private:
    void add_rec(const std::string&);

private:
    std::string log_file = "log.txt";
    std::queue<std::string, std::list<std::string>> records;
};

extern Logger& log;

void die(std::string);

}  // namespace pkr
