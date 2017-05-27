// Copyright 2016-2017, Pavel Korozevtsev.

#include "include/server.hpp"
#include "include/defs.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/utils.hpp"

namespace pkr {

Server::Server(const ServerConfig& cfg)
    : max_threads(cfg.max_threads)
    , working_dir(cfg.working_dir)
    , server_socket(make_server_socket(cfg.port))
    , epoll(server_socket)
    , helpers(max_threads, *this) {
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
    log.message(msg.url + " at " + msg.head.at("Host") + " requested");
    forward(client, msg);
}

bool Server::forward(Socket& client, const Message& msg) const {
    log.message("Forwarding to " + msg.head.at("Host"));
    const auto& host = msg.head.at("Host");
    auto host_sock = connect_to_server(make_serv_addr(host, msg.service));
    if (!host_sock.valid()) {
        log.error("Unable to download from " + host + " (connect failed)");
        return;
    }
    const auto req = join_message(msg);
    if (write(host_sock, req) != static_cast<ssize_t>(req.size())) {
        log.error("Unable to send request to " + host);
        return;
    }
    return read(host_sock);
}

void Server::serve(Socket& client) const {
    const auto request = read(client);
    Message msg(request);

    using VPS = std::vector<std::pair<string_view, string_view>>;
    for (auto pp : VPS{{"Connection", "close"}, {"Proxy-Connection", ""}}) {
        auto pos = msg.head.find(pp.first);
        if (pos != msg.head.end()) {
            if (pp.second.size() == 0) {
                msg.head.erase(pos);
            } else {
                pos->second = pp.second;
            }
        }
    }
    if (!forward(client, msg)) {
        write_failure(client, 500, "Internal Server Error");
    } else {
        write(client, response);
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
    : parent(p) {
}

ServerThread::ServerThread(const ServerThread& other)
    : ServerThread(other.parent) {
}

void ServerThread::start() {
    while (parent.enabled || !clients.empty()) {
        std::unique_lock<std::mutex> lock(cv_m);
        while (!clients.empty()) {
            Socket client(std::move(clients.front()));
            clients.pop_front();
            client.make_nonblock();
            parent.serve(client);
        }
        cv.wait(lock, [this] { return !clients.empty() || !parent.enabled; });
    }
}

}  // namespace pkr
