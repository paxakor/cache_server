// Copyright 2016, Pavel Korozevtsev.

#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "include/log.hpp"
#include "include/filesystem.hpp"

namespace pkr {

Logger::Logger() { }

Logger::~Logger() {
    write_to_file();
}

void Logger::set_file(const std::string& file_name) {
    log_file = file_name;
}

void Logger::set_limit(uint64_t n) {
    max_size = n;
}

void Logger::init() {
    records.reserve(max_size);
    if (fs::exists(log_file)) {
        std::string line;
        std::ifstream old_log(log_file);
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
    static const std::string error_prefix = "Error: ";
    add_rec(error_prefix + msg);
    std::cerr << "Error occured. See log file for details. Exit." << std::endl;
    write_to_file();
    std::exit(1);
}

void Logger::access(const std::string& ip, uint32_t port) {
    add_rec(std::string("New client: ") + ip + ":" + std::to_string(port));
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
    std::cout << "Saving log to file" << std::endl;
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
