// Copyright 2016, Pavel Korozevtsev.
// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <string>
#include <vector>

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

}  // namespace pkr
