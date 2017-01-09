// Copyright 2016, Pavel Korozevtsev.

#include <signal.h>
#include "include/signal.hpp"

namespace pkr {

Server* SignalHandler::srv = nullptr;

SignalHandler::SignalHandler() {
    signal(SIGINT,  finish);
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
