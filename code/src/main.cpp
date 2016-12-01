// Copyright 2016, Pavel Korozevtsev.

#include "include/server.hpp"

int main(int /* argc */, char** /* argv */) {
    const auto config = pkr::read_config();
    pkr::Server srv(config);
    srv.start();
    return 0;
}
