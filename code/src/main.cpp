// Copyright 2016, Pavel Korozevtsev.

#include "include/args_parser.hpp"
#include "include/log.hpp"
#include "include/server.hpp"

using namespace pkr;

int main(int argc, char** argv) {
    ServerArgs args(argc, argv);
    Server srv(args.config_file());
    srv.start();
    return 0;
}
