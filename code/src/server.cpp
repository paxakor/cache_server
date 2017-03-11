// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "include/client.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/parser.hpp"
#include "include/server.hpp"
#include "include/socket.hpp"
#include "include/utils.hpp"

namespace pkr {

Server::Server(const ServerConfig& cfg)
    : server_socket(cfg.port)
    , working_dir(cfg.working_dir)
    , enabled(true) { }

void Server::start() {
    log.message("Server started");
    while (enabled) {
        Client client(accept_client(server_socket));
        DO_DEBUG(std::cout << "Connection established" << std::endl);
        log.access(client.get_ip(), client.get_port());
        interact_connection(client);
    }
}

void Server::finish() {
    std::cout << "Sever interrupted" << std::endl;
    enabled = false;
}

void Server::do_get(Client& client, const Message& msg) {
    log.message(msg.url + " requested");
    std::string file_name = working_dir;
    file_name += (msg.url == "/" || msg.url == "") ? "index.html" : msg.url;
    std::ifstream file(file_name);
    if (!file) {
        log.message(file_name + " not found. 404");
        client.write_failure(404, "Not Found");
    } else {
        log.message(file_name + " opened");
        client.write_response(200, fs::file_size(file_name));
        char buffer[MAX_REQUEST_SIZE];
        do {
            file.read(buffer, sizeof(buffer));
            const auto len = file.gcount();
            if (write(client, {buffer, static_cast<size_t>(len)}) != len) {
                log.message("socket.write error");
            }
        } while (file.good());
    }
}

void Server::interact_connection(Client& client) {
    const auto request = read(client);
    const auto header = find_header(request);
    const auto msg = parse_header(header);
    DO_DEBUG(std::cout << debug_header(msg) << std::endl);
    switch (msg.method) {
        case Method::GET:
            do_get(client, msg);
            break;
        case Method::POST:
            // do_post(client, cmd_arg);
            break;
        default:
            client.write_failure(400, "Bad Request");
            log.message("invalid command");
            break;
    }
}

ServerConfig::ServerConfig(const std::string& file_name) {
    std::ifstream cfg_file(file_name);
    std::string line, err_msg;
    uint64_t line_number = 0;
    while (std::getline(cfg_file, line)) {
        ++line_number;
        const auto parts = split_n(line, 2);
        if (parts.size() >= 2) {
            if (parts[0] == "dir") {
                working_dir = parts[1];
                if (!fs::is_directory(working_dir)) {
                    err_msg = "no such directory";
                }
                if (*working_dir.rbegin() != '/') {
                    working_dir += "/";
                }
            } else if (parts[0] == "port") {
                port = std::strtoul(std::string(parts[1].data(),
                    parts[1].size()).c_str(), NULL, 10);
                if (!(1000 < port && port < ((1 << 16) - 1))) {  // TODO
                    err_msg = "invalid port";
                }
            } else {
                err_msg = "no such option";
            }
        } else {
            err_msg = "too few arguments";
        }
        if (!err_msg.empty()) {
            log.fatal_error("Invalid config line " +
                std::to_string(line_number) + ": " + err_msg + ".");
        }
    }
}

}  // namespace pkr
