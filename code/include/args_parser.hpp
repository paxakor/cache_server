// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include <vector>
#include "include/defs.hpp"

namespace pkr {

class ArgList {
public:
    ArgList(int, char**);
    bool check_flag(const std::string&) const;
    int value_id(const std::string&) const;
    std::string get_arg(const int) const;

private:
    const std::vector<std::string> args;
};

class ServerArgs {
public:
    ServerArgs(int, char**);
    std::string config_file() const;

private:
    void print_usage() const;

private:
    const ArgList list;
};

class ServerConfig {
public:
    ServerConfig(const std::string&);
private:
    void error(const std::string&, size_t);

public:
    Port port = 31337;
    std::string working_dir = "./";
};

}  // namespace pkr
