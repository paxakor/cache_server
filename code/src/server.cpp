// Copyright 2016, Pavel Korozevtsev.

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

#include "include/filesystem.hpp"
#include "include/server.hpp"
#include "include/socket.hpp"
#include "include/utils.hpp"

namespace pkr {

void write_headers_or_error(int client_socket, int status,
    size_t content_length, const char *data)
{
    char headers[2048];
    if (status == 200) {
        snprintf(headers, sizeof(headers),
            "HTTP/1.0 200 OK\r\n"
            "Server: Wonderful HTTP Server By Paxakor\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %lu\r\n"
            "\r\n",
            data, content_length);
    } else {
        snprintf(headers, sizeof(headers),
            "HTTP/1.0 %d %s\r\n"
            "Server: Wonderful HTTP Server By Paxakor\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %lu\r\n"
            "\r\n"
            "%s",
            status, data, strlen(data), data);
    }
    write(client_socket, headers, strlen(headers));
}

void do_get(int client_socket, const char *url, const char *client_ip,
    const uint16_t client_port, const char *htdocs_dir)
{
    char file_name[2048] = {0};
    memcpy(file_name, htdocs_dir, strlen(htdocs_dir));
    if (strcmp("/", url) == 0 || strlen(url) == 0) {
        strcat(file_name, "/index.html");
    } else {
        strcat(file_name, url);
    }
    printf("open %s\n", file_name);
    int f_id = open(file_name, O_RDONLY);
    if (f_id == -1) {
        write_headers_or_error(client_socket, 404, 0, "Not Found");
    } else {
        // write_headers_or_error(client_socket, 200, get_file_size(file_name),
        //     get_mime_type(file_name));
        write_headers_or_error(client_socket, 200, 100500,
            "mega mime type");
        char file_buffer[65536];
        while (1) {
            ssize_t byttes_read = read(f_id, file_buffer, sizeof(file_buffer));
            if (byttes_read > 0) {
                ssize_t bytes_writen = write(client_socket, file_buffer,
                    byttes_read);
            } else {
                break;
            }
        }
        close(f_id);
    }
}

void parse_command(const char *recv_buffer, enum command_t *command,
    const char *command_arg, const size_t cmd_arg_len)
{
    printf("recv_buffer: %s\n", recv_buffer);
    *command = CMD_GET;
}

void interact_connection(int client_socket, const char *client_ip,
    const uint16_t client_port, const char *htdocs_dir)
{
    char recv_buffer[16536];
    memset(recv_buffer, 0, sizeof(recv_buffer));
    ssize_t byttes_received;
    char command_arg[2000];
    enum command_t command = CMD_UNKNOWN;
    byttes_received = read(client_socket, recv_buffer, sizeof(recv_buffer));
    parse_command(recv_buffer, &command, command_arg, sizeof(command_arg));
    if (command == CMD_GET) {
        do_get(client_socket, command_arg, client_ip, client_port, htdocs_dir);
    } else if (command == CMD_POST) {
        // do_post(client_socket, command_arg, client_ip, client_port, htdocs_dir);
    }
}

void process_connection(int client_socket,
    const struct sockaddr_in *client_address, const char *htdocs_dir)
{
    char client_ip[INET_ADDRSTRLEN];
    const struct in_addr client_ip_addr = client_address->sin_addr;
    inet_ntop(AF_INET, &client_ip_addr, client_ip, sizeof(client_ip));
    const uint16_t client_port = ntohs(client_address->sin_port);
    // write_access_log(client_ip, client_port, "CONNECT", "", "OK");
    interact_connection(client_socket, client_ip, client_port, htdocs_dir);
    close(client_socket);
}

ServerConfig read_config(const std::string& file_name) {
    std::ifstream cfg_file(file_name);
    std::string line;
    ServerConfig cfg;
    while (std::getline(cfg_file, line)) {
        const auto parts = split(line, 2);
        bool error = false;
        if (parts.size() >= 2) {
            if (parts[0] == "dir") {
                cfg.pwd = parts[1];
                error |= !fs::exists(cfg.pwd);
            } else if (parts[0] == "port") {
                cfg.port = std::strtoul(parts[1].c_str(), NULL, 10);
                error |= !(0 < cfg.port && cfg.port < ((1 << 16) - 1));  // TODO
            } else {
                error = true;
            }
        } else {
            error = true;
        }
        if (error) {
            std::cerr << "Invalid config line: " << line << std::endl;
        }
    }
    return cfg;
}

Server::Server(const ServerConfig& cfg)
    : server_socket(cfg.port) {}

void Server::start() {
    while (1) {
        ClientSocket client;
        server_socket.accept(client);
        process_connection(client.get_handler(), &client.mutable_address(),
            "/home/paxakor/programs/projects/server/files");
    }
}

}  // namespace pkr
