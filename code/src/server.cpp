// Copyright 2016, Pavel Korozevtsev.

#include <arpa/inet.h>
#include <ext/stdio_filebuf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "include/client.hpp"
#include "include/filesystem.hpp"
#include "include/log.hpp"
#include "include/server.hpp"
#include "include/socket.hpp"
#include "include/utils.hpp"

namespace pkr {

void Server::do_get(Client& client, const std::string& url) {
    std::string file_name = working_dir;
    if (*file_name.rbegin() != '/') {
        file_name += "/";
    }
    if (url == "/"  || url == "") {
        file_name += "index.html";
    } else {
        file_name += url;
    }
    log.message("open " + file_name);
    std::ifstream file(file_name);
    if (!file) {
        client.write_response(404, "Not Found");
    } else {
        client.write_response(200, "mega mime type", fs::file_size(file_name));
        char buffer[65536];
        do {
            file.read(buffer, sizeof(buffer));
            client.mutable_socket().write(buffer, file.gcount());
        } while (file.good());

        // the following code makes the same: it copies file into socket
        // (possible, it will be useful in the future)
        // const int hndl = client.get_socket().get_handler();
        // __gnu_cxx::stdio_filebuf<char> filebuf(hndl, std::ios::out);
        // std::ostream socket_stream(&filebuf);
        // socket_stream << file.rdbuf();
    }
}

Server::Command Server::parse_command(const std::string& buffer,
    const char *cmd_arg, const size_t cmd_arg_len) {
    printf("buffer: %s\n", buffer.c_str());
    return Command::get;
}

void Server::interact_connection(Client& client) {
    char cmd_arg[2000] = {0};
    char header[16 * 1024] = {0};  // max header size = 16KiB
    const auto bytes = client.mutable_socket().read(header, sizeof(header));
    if (bytes >= static_cast<decltype(bytes)>(sizeof(header))) {
        log.message("Entity Too Large");
        client.write_response(413, "Entity Too Large");
        return;
    }
    const auto command = parse_command(header, cmd_arg, sizeof(cmd_arg));
    switch (command) {
        case Command::get:
            do_get(client, cmd_arg);
            break;
        case Command::post:
            // do_post(client, cmd_arg);
            break;
        default:
            log.message("invalig command");
            break;
    }
}

ServerConfig::ServerConfig(const std::string& file_name) {
    std::ifstream cfg_file(file_name);
    std::string line, err_msg;
    uint64_t line_number = 0;
    while (std::getline(cfg_file, line)) {
        ++line_number;
        const auto parts = split(line, 2);
        if (parts.size() >= 2) {
            if (parts[0] == "dir") {
                working_dir = parts[1];
                fs::path p(working_dir);
                if (!fs::exists(p) || !fs::is_directory(p)) {
                    err_msg = "no such directory";
                }
            } else if (parts[0] == "port") {
                port = std::strtoul(parts[1].c_str(), NULL, 10);
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
            log.error("Invalid config line " + std::to_string(line_number) +
                ": " + err_msg + ".");
        }
    }
}

Server::Server(const ServerConfig& cfg)
    : server_socket(cfg.port)
    , working_dir(cfg.working_dir) { }

void Server::start() {
    while (true) {
        Client client(server_socket);
        log.access(client.get_ip(), client.get_port());
        interact_connection(client);
    }
}

}  // namespace pkr
