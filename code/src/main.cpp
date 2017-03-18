// Copyright 2016-2017, Pavel Korozevtsev.

#include "include/args_parser.hpp"
#include "include/log.hpp"
#include "include/server.hpp"
#include "include/signal.hpp"

using namespace pkr;

int main(int argc, char** argv) {
    SignalHandler shndl;
    ServerArgs args(argc, argv);
    Server srv(args.config_file());
    shndl.set_server(srv);
    srv.start();
    log.write_to_file();
    return 0;
}
