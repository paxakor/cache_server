// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "include/args_parser.hpp"
#include "include/defs.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/utils.hpp"

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


ServerConfig::ServerConfig(const std::string& file_name) {
    std::ifstream cfg_file(file_name);
    std::string line;
    size_t line_number = 0;
    while (std::getline(cfg_file, line)) {
        ++line_number;
        const auto parts = split_n(line, 2);
        if (parts.size() < 2) {
            error("too few arguments", line_number);
        }
        if (parts[0] == "dir") {
            working_dir = parts[1];
            if (!fs::is_directory(working_dir)) {
                error("no such directory", line_number);
            }
            if (working_dir.back() != '/') {
                working_dir += "/";
            }
        } else if (parts[0] == "port") {
            port = std::strtoul(parts[1].data(), NULL, 10);
            if (!(1000 < port && port < ((1 << 16) - 1))) {  // TODO
                error("invalid port", line_number);
            }
        } else if (parts[0] == "max_threads") {
            max_threads = std::strtoul(parts[1].data(), NULL, 10);
        } else {
            error("no such option", line_number);
        }
    }
}

void ServerConfig::error(const std::string& msg, size_t nline) {
    log.fatal_error("Invalid config line " +
        std::to_string(nline) + ": " + msg + ".");
}

}  // namespace pkr
