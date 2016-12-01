// Copyright 2016, Pavel Korozevtsev.

#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <string>
#include "include/log.hpp"
#include "include/server.hpp"

using namespace pkr;

struct Args {
    Args(const int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-l") {
                log_file = argv[i + 1];
            } else if (arg == "-ln") {
                logmax = std::strtol(argv[i + 1], nullptr, 10);
            } else if (arg == "-c") {
                config_file = argv[i + 1];
            }
        }
    }

    std::string config_file = "config.txt";
    std::string log_file = "log.txt";
    uint64_t logmax = 1024;
};

int main(int argc, char** argv) {
    Args args(argc, argv);
    Logger log(args.log_file, args.logmax);
    const auto config = read_config(args.config_file, log);
    Server srv(config, log);
    srv.start();
    return 0;
}
