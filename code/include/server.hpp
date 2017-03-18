// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>
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
    ~Server();
    void start();
    void finish();
    void do_get(Socket&, const Message&) const;
    void serve(Socket&) const;
    void share_clients(ServerThread&);

public:
    bool enabled = true;
    const size_t max_threads;
    const std::string working_dir;
    Socket server_socket;
    Epoll epoll;
    std::list<std::thread> threads;
    std::mutex clients_mutex;
    std::vector<ServerThread> helpers;
};

class ServerThread {
public:
    ServerThread(Server&);
    ServerThread(const ServerThread&);
    ServerThread(ServerThread&&) = default;
    void start();

public:
    Server& parent;
    std::condition_variable cv;
    std::list<Socket> clients;
    std::mutex cv_m;
};

}  // namespace pkr
