// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <signal.h>
#include "include/server.hpp"

namespace pkr {

class SignalHandler {
public:
    SignalHandler();
    void set_server(Server&);

private:
    static void finish(int);

private:
    static Server* srv;
};

}  // namespace pkr
