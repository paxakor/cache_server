// Copyright 2016-2017, Pavel Korozevtsev.

#include <fstream>
#include <list>
#include <thread>
#include <vector>

#include "include/defs.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/server.hpp"
#include "include/utils.hpp"

namespace pkr {

Server::Server(const ServerConfig& cfg)
    : max_threads(cfg.max_threads)
    , working_dir(cfg.working_dir)
    , server_socket(make_server_socket(cfg.port))
    , epoll(make_epoll(server_socket))
    , helpers(max_threads, *this)
{
    for (auto& helper : helpers) {
        threads.emplace_back([&helper] { helper.start(); });
    }
}

Server::~Server() {
    for (auto& thread : threads) {
        thread.join();
    }
}

void Server::start() {
    log.message("Server started");
    size_t index = 0;
    while (enabled) {
        epoll.wait();
        std::thread accepter([this] { epoll.accept_all(); });
        share_clients(helpers[index]);
        index = (index + 1) % max_threads;
        accepter.join();
    }
}

void Server::finish() {
    log.print("Server interrupted");
    enabled = false;
    for (auto& helper : helpers) {
        helper.cv.notify_all();
    }
}

void Server::do_get(Socket& client, const Message& msg) const {
    log.message(msg.url + " requested");
    std::string file_name = working_dir;
    file_name += (msg.url == "/" || msg.url == "") ? "index.html" : msg.url;
    std::ifstream file(file_name);
    if (!file) {
        log.message(file_name + " not found. 404");
        write_failure(client, 404, "Not Found");
    } else {
        log.message(file_name + " opened");
        write_response(client, 200, fs::file_size(file_name));
        char buffer[max_request_size];
        do {
            file.read(buffer, sizeof(buffer));
            const auto len = file.gcount();
            if (write(client, {buffer, static_cast<size_t>(len)}) != len) {
                log.message("socket.write error");
            }
        } while (file.good());
    }
}

void Server::serve(Socket& client) const {
    const auto request = read(client);
    const auto header = find_header(request);
    const auto msg = parse_header(header);
    switch (msg.method) {
        case Method::GET:
            do_get(client, msg);
            break;
        case Method::POST:
            // do_post(client, cmd_arg);
            break;
        default:
            write_failure(client, 400, "Bad Request");
            log.message("invalid command");
            break;
    }
}

void Server::share_clients(ServerThread& dest) {
    {
        std::lock_guard<std::mutex> lock(dest.cv_m);
        std::move(epoll.begin(), epoll.end(), std::back_inserter(dest.clients));
    }
    dest.cv.notify_all();
}

ServerThread::ServerThread(Server& p)
    : parent(p) {}

ServerThread::ServerThread(const ServerThread& other)
    : ServerThread(other.parent) {}

void ServerThread::start() {
    while (parent.enabled || !clients.empty()) {
        std::unique_lock<std::mutex> lock(cv_m);
        while (!clients.empty()) {
            Socket client(std::move(clients.front()));
            clients.pop_front();
            parent.serve(client);
        }
        cv.wait(lock, [this] { return !clients.empty() || !parent.enabled; });
    }
}

}  // namespace pkr
