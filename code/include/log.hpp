// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pkr {

class Logger {
public:
    Logger();
    Logger(const std::string&, uint64_t);
    ~Logger();
    void error(const std::string&);

private:
    void add_rec(const std::string&);
    void write_to_file();

private:
    const std::string log_file;
    const uint64_t max_size;
    uint64_t last_rec_pos = 0;
    std::vector<std::string> records;
};

}  // namespace pkr
