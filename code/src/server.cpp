// Copyright 2016-2017, Pavel Korozevtsev.

#include <fstream>
#include <iostream>

#include "include/defs.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/server.hpp"
#include "include/utils.hpp"

namespace pkr {

Server::Server(const ServerConfig& cfg)
    : server_socket(make_server_socket(cfg.port))
    , epoll(make_epoll(server_socket))
    , working_dir(cfg.working_dir)
    , enabled(true) { }

void Server::start() {
    log.message("Server started");
    while (enabled) {
        epoll.wait();
        for (auto client : epoll) {
            interact_connection(client);
        }
        epoll.accept_all();
    }
}

void Server::finish() {
    std::cout << "Sever interrupted" << std::endl;
    enabled = false;
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

void Server::interact_connection(Socket& client) const {
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

}  // namespace pkr
