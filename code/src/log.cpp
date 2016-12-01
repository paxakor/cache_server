// Copyright 2016, Pavel Korozevtsev.

#include <cstdint>
#include <cstdlib>
#include <ctime>
// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "include/log.hpp"
#include "include/filesystem.hpp"

namespace pkr {

Logger::Logger()
    : Logger("server_log.txt", 1024) {}

Logger::~Logger() {
    write_to_file();
}

Logger::Logger(const std::string& log_file_name, const uint64_t size)
    : log_file(log_file_name)
    , max_size(size) {
    records.reserve(max_size);
    if (fs::exists(log_file_name)) {
        std::string line;
        std::ifstream old_log(log_file_name);
        while (std::getline(old_log, line) && records.size() < max_size) {
            records.push_back(std::move(line));
            ++last_rec_pos;
        }
    }
    if (last_rec_pos == max_size) {
        last_rec_pos = 0;
    }
}

void Logger::error(const std::string& msg) {
    static const std::string error_prefix = "ERROR: ";
    add_rec(error_prefix + msg);
    std::cerr << "Error occured. See log file for details. Exit." << std::endl;
    write_to_file();
    std::exit(1);
}

void Logger::add_rec(const std::string& msg) {
    const auto tm = std::time(nullptr);
    std::string rec = std::string(std::ctime(&tm)) + "  " + msg;
    if (records.size() < max_size) {
        records.push_back(std::move(rec));
    } else {
        records[last_rec_pos] = std::move(rec);
    }
    // last_rec_pos = (last_rec_pos + 1) % max_size;
    ++last_rec_pos;
    if (last_rec_pos == max_size) {
        last_rec_pos = 0;
    }
}

void Logger::write_to_file() {
    std::ofstream logf(log_file);
    uint64_t rec_pos = records.size() < max_size ? 0 : last_rec_pos;
    if (records.size() == max_size) {
        last_rec_pos = (last_rec_pos + max_size - 1) % max_size;
    }
    while (rec_pos != last_rec_pos) {
        logf << records[rec_pos] << '\n';
        ++rec_pos;
    }
    records.clear();
    last_rec_pos = 0;
}

}  // namespace pkr
