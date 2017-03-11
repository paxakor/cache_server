// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstring>
#include "include/client.hpp"
#include "include/socket.hpp"
#include "include/string_view.hpp"

namespace pkr {

Client::Client(ClientSocket socket)
    : socket(std::move(socket)) {
    const auto client_ip_addr = socket.get_address().sin_addr;
    inet_ntop(AF_INET, &client_ip_addr, &ip[0], ip.size());
    port = ntohs(socket.get_address().sin_port);
}


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
    return write({headers, strlen(headers)});
}

ssize_t Client::write_response(int status, size_t file_size) {
    char headers[2048];
    snprintf(headers, sizeof(headers),
        "HTTP/1.0 %d OK\r\n"
        "Content-Length: %lu\r\n"
        "\r\n", status, file_size);
    return write({headers, strlen(headers)});
}

std::string Client::read(size_t nbyte) {
    return ::pkr::read(socket, nbyte);
}

ssize_t Client::write(string_view sv) {
    return ::pkr::write(socket, sv);
}

std::string read(Client& cl, size_t nbyte) {
    return cl.read(nbyte);
}

ssize_t write(Client& cl, string_view sv) {
    return cl.write(sv);
}

}  // namespace pkr
