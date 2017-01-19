// Copyright 2016, Pavel Korozevtsev.

#include "include/args_parser.hpp"
#include "include/signal.hpp"
#include "include/log.hpp"
#include "include/server.hpp"

using namespace pkr;

int main(int argc, char** argv) {
    SignalHandler shndl;
    log.init();
    ServerArgs args(argc, argv);
    Server srv(args.config_file());
    shndl.set_server(srv);
    srv.start();
    log.write_to_file();
    return 0;
}
