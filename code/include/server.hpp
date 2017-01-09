// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <stdint.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "include/client.hpp"
#include "include/socket.hpp"

namespace pkr {

class ServerConfig {
public:
    ServerConfig(const std::string&);

public:
    Port port = 31337;
    std::string working_dir = "./";
};

class Server {
public:
    Server(const ServerConfig&);
    void start();
    void finish();

private:
    void do_get(Client&, const std::string&);
    void interact_connection(Client&);

private:
    ServerSocket server_socket;
    std::string working_dir;
    bool enabled;
};

}  // namespace pkr
