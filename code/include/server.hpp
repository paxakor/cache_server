// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include "include/client.hpp"
#include "include/parser.hpp"
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
    enum : size_t { MAX_REQUEST_SIZE = FileDescriptor::MAX_REQUEST_SIZE };

public:
    Server(const ServerConfig&);
    void start();
    void finish();
    void do_get(Client&, const Message&);

private:
    void interact_connection(Client&);

private:
    ServerSocket server_socket;
    std::string working_dir;
    bool enabled;
};

}  // namespace pkr
