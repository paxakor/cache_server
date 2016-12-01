// Copyright 2016, Pavel Korozevtsev.

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "include/socket.hpp"

namespace pkr {

Socket::Socket(int h)
    : handler(h) {}

Socket::~Socket() {
    close(handler);
}

int Socket::get_handler() const {
    return handler;
}

void Socket::set_handler(int h) {
    handler = h;
}

sockaddr_in Socket::get_address() const {
    return address;
}

sockaddr_in& Socket::mutable_address()  {
    return address;
}


ServerSocket::ServerSocket(uint16_t port)
    : Socket(socket(AF_INET, SOCK_STREAM, 0)) {
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    bind(handler, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
    listen(handler, SOMAXCONN);
}

void ServerSocket::accept(ClientSocket& client) {
    auto& client_address = client.mutable_address();
    socklen_t client_address_size = sizeof(client_address);
    client.set_handler(::accept(handler,
        reinterpret_cast<sockaddr*>(&client_address), &client_address_size));
}

}  // namespace pkr
