// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

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
