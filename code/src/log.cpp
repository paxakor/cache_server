// Copyright 2016-2017, Pavel Korozevtsev.

#include <cerrno>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include "include/log.hpp"

namespace pkr {

Logger::~Logger() {
    write_to_file();
}

void Logger::set_file(const std::string& file_name) {
    log_file = file_name;
}

void Logger::access(const std::string& ip, Port port) {
    add_rec(std::string("New client: ") + ip + ":" + std::to_string(port));
}

void Logger::error(const std::string& msg) {
    static const std::string prefix = "Error: ";
    add_rec(prefix + msg);
    write_to_file();
}

void Logger::fatal_error(const std::string& msg) {
    std::cerr << "Error occured. See log file for details. Exit." << std::endl;
    error(msg);
    throw std::runtime_error(msg);
}

void Logger::message(const std::string& msg) {
    static const std::string prefix = "Message: ";
    add_rec(prefix + msg);
}

void Logger::print(const std::string& msg) {
    std::cout << msg << std::endl;
    message(msg);
}

void Logger::add_rec(const std::string& msg) {
    static std::mutex mt;
    mt.lock();
    records.push(std::to_string(std::time(nullptr)) + " " + msg);
    if (records.size() > 0x4000) {
        write_to_file();
    }
    mt.unlock();
}

void Logger::write_to_file() {
    std::cout << "Saving log to file" << std::endl;
    std::ofstream logf(log_file, std::ios::app);
    while (!records.empty()) {
        logf << records.front() << '\n';
        records.pop();
    }
}

std::string Logger::errstr() {
    return {strerror(errno)};
}

Logger log;

}  // namespace pkr
