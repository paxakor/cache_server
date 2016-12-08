// Copyright 2016, Pavel Korozevtsev.

#include <signal.h>
#include "include/log.hpp"
#include "include/signal.hpp"

namespace pkr {

SignalHandler::SignalHandler() {
    signal(SIGABRT, finish);
    signal(SIGFPE,  finish);
    signal(SIGILL,  finish);
    signal(SIGINT,  finish);
    signal(SIGSEGV, finish);
    signal(SIGTERM, finish);
}

void SignalHandler::finish(int sig) {
    log.write_to_file();
    signal(sig, SIG_DFL);
    raise(sig);
}

SignalHandler& handler = Singleton<SignalHandler>::get_inctance();

}  // namespace pkr
