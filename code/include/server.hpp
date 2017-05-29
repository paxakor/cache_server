// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <condition_variable>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "include/args_parser.hpp"
#include "include/epoll.hpp"
#include "include/librarian.hpp"
#include "include/net_utils.hpp"
#include "include/parser.hpp"
#include "include/socket.hpp"

namespace pkr {

class ServerThread;

class Server {
public:
    Server(const ServerConfig&);
    ~Server();
    void start();
    void finish();
    void share_clients(ServerThread&);
    std::string do_get(Message);
    std::string do_something(Message);
    std::string forward(Message);
    void serve(Socket&);

public:
    volatile bool enabled = true;
    const size_t max_threads;
    const std::string working_dir;
    Socket server_socket;
    Epoll epoll;
    std::list<std::thread> threads;
    std::mutex clients_mutex;
    std::vector<ServerThread> helpers;
    Librarian librarian;
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
