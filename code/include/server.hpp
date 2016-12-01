// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <stdint.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "include/socket.hpp"

namespace pkr {

struct ServerConfig {
    std::string pwd;
    uint16_t port = 31337;
};

class Server {
public:
    Server(const ServerConfig&);
    void start();
private:
    ServerSocket server_socket;
};

enum command_t
{
    CMD_UNKNOWN,
    CMD_GET,
    CMD_POST
};

void write_headers_or_error(int client_socket, int status,
    size_t content_length, const char *data);

void do_get(int client_socket, const char *url, const char *client_ip,
    const uint16_t client_port, const char *htdocs_dir);

void parse_command(const char *recv_buffer, enum command_t *command,
    const char *command_arg, const size_t cmd_arg_len);

void interact_connection(int client_socket, const char *client_ip,
    const uint16_t client_port, const char *htdocs_dir);

void process_connection(int client_socket,
    const struct sockaddr_in *client_address, const char *htdocs_dir);

ServerConfig read_config(const std::string& file_name = "config.txt");

}  // namespace pkr
