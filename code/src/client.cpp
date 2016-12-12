// Copyright 2016, Pavel Korozevtsev.

#include <string.h>
#include "include/client.hpp"
#include "include/socket.hpp"

namespace pkr {

Client::Client(ServerSocket& server_socket) {
    server_socket.accept(socket);
    const auto client_ip_addr = socket.get_address().sin_addr;
    inet_ntop(AF_INET, &client_ip_addr, &ip[0], ip.size());
    port = ntohs(socket.get_address().sin_port);
}

ClientSocket& Client::mutable_socket() {
    return socket;
}

const ClientSocket& Client::get_socket() const {
    return socket;
}

Port Client::get_port() const {
    return port;
}

std::string Client::get_ip() const {
    return {ip.begin(), ip.begin() + strlen(&ip[0])};
}

ssize_t Client::write_response(int status, const std::string& msg,
    size_t file_size) {
    char headers[2048];
    if (status == 200) {
        snprintf(headers, sizeof(headers),
            "HTTP/1.0 200 OK\r\n"
            "Server: Wonderful HTTP Server By Paxakor\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %lu\r\n"
            "\r\n",
            msg.c_str(), file_size);
    } else {
        snprintf(headers, sizeof(headers),
            "HTTP/1.0 %d %s\r\n", status, msg.c_str());
    }
    return mutable_socket().write(headers, strlen(headers));
}

}  // namespace pkr