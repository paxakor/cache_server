// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "include/args_parser.hpp"

namespace pkr {

ArgList::ArgList(int argc, char** argv)
    : args(argv, argv + argc) { }

bool ArgList::check_flag(const std::string& flag) const {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

int ArgList::value_id(const std::string& key) const {
    const auto key_iter = std::find(args.begin(), args.end(), key);
    if (key_iter == args.end()) {
        return -1;
    } else if (key_iter + 1 == args.end()) {
        return -2;
    } else {
        return key_iter - args.begin() + 1;
    }
}

std::string ArgList::get_arg(const int n) const {
    return args[n];
}


ServerArgs::ServerArgs(int argc, char** argv)
    : list(argc, argv) {
        if (list.check_flag("-h") || list.check_flag("--help")) {
            print_usage();
        }
    }

std::string ServerArgs::config_file() const {
    const auto arg_id = list.value_id("-c");
    if (arg_id == -2) {
        print_usage();
    }
    if (arg_id == -1) {
        return "config.txt";
    }
    return list.get_arg(arg_id);
}

void ServerArgs::print_usage() const {
    std::cout <<
        "Usage: " << list.get_arg(0) << " [flags]...\n"
        "Flags:\n"
        "  -c <file>:  read configuration from <file>\n"
        "  -h, --help: print this message and exit\n"
        << std::flush;
        std::exit(0);
}


}  // namespace pkr
