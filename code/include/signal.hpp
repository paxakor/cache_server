// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <signal.h>
#include "singleton.hpp"

namespace pkr {

class SignalHandler {
protected:
    friend class Singleton<SignalHandler>;
    SignalHandler();

public:
    SignalHandler(const SignalHandler&) = delete;
    SignalHandler& operator=(const SignalHandler&) = delete;

private:
    static void finish(int);
};

extern SignalHandler& handler;

}  // namespace pkr
