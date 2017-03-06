// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstring>
#include "include/client.hpp"
#include "include/socket.hpp"

namespace pkr {

Client::Client(ClientSocket socket)
    : socket(std::move(socket)) {}


Port Client::get_port() const {
    return port;
}

std::string Client::get_ip() const {
    return {ip.begin(), ip.begin() + strlen(ip.data())};
}

ssize_t Client::write_failure(int status, const std::string& msg) {
    char headers[2048];
    snprintf(headers, sizeof(headers), "HTTP/1.0 %d %s\r\n", status,
        msg.c_str());
    return write(headers, strlen(headers));
}

ssize_t Client::write_response(int status, size_t file_size) {
    char headers[2048];
    snprintf(headers, sizeof(headers),
        "HTTP/1.0 %d OK\r\n"
        "Content-Length: %lu\r\n"
        "\r\n", status, file_size);
    return write(headers, strlen(headers));
}

std::string Client::read_header() {
    return socket.read_header();
}

}  // namespace pkr
