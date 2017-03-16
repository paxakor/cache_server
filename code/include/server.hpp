// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include "include/args_parser.hpp"
#include "include/epoll.hpp"
#include "include/parser.hpp"
#include "include/socket.hpp"

namespace pkr {

class Server {
public:
    enum : size_t { max_request_size = FileDescriptor::max_request_size };

public:
    Server(const ServerConfig&);
    void start();
    void finish();
    void do_get(Socket&, const Message&) const;
    void interact_connection(Socket&) const;

private:
    Socket server_socket;
    Epoll epoll;
    std::string working_dir;
    bool enabled;
};

}  // namespace pkr
