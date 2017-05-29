// Copyright 2016-2017, Pavel Korozevtsev.

#include <csignal>

#include "include/server.hpp"
#include "include/signal.hpp"

namespace pkr {

Server* SignalHandler::srv = nullptr;

SignalHandler::SignalHandler() {
    signal(SIGINT, finish);
    signal(SIGTERM, finish);
}

void SignalHandler::set_server(Server& s) {
    srv = &s;
}

void SignalHandler::finish(int /* sig */) {
    if (srv) {
        srv->finish();
    }
}

}  // namespace pkr
