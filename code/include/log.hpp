// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "singleton.hpp"

namespace pkr {

class Logger {
protected:
    friend class Singleton<Logger>;
    Logger();

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    ~Logger();

    void init();
    void set_file(const std::string&);
    void set_limit(uint64_t);

    void error(const std::string&);
    void access(const std::string&, uint32_t);

private:
    void add_rec(const std::string&);
    void write_to_file();

private:
    std::string log_file = "log.txt";
    uint64_t max_size = 1024;
    uint64_t last_rec_pos = 0;
    std::vector<std::string> records;
};

#define log (Singleton<Logger>::get_inctance())

}  // namespace pkr
