// Copyright 2016-2017, Pavel Korozevtsev.

#include <cerrno>
#include <cstddef>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <memory>
#include <string>
#include "include/log.hpp"
#include "include/socket.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

FileDescriptor::FileDescriptor(int h)
    : handler(h) {
    if (handler < 0) {
        log.error("invalid fildes");
    }
}

FileDescriptor::FileDescriptor(FileDescriptor&& other)
    : handler(std::move(other.handler)) {
    other.handler = -1;
}

FileDescriptor::~FileDescriptor() {
    if (handler != -1) {
        close(handler);
    }
}

ssize_t FileDescriptor::read(char* dest, size_t nbyte) {
    return ::read(handler, dest, nbyte);
}

ssize_t FileDescriptor::write(const void* buf, size_t nbyte) {
    return ::write(handler, buf, nbyte);
}

std::string read(FileDescriptor& fd, size_t nbyte) {
    std::string str(nbyte, 0);
    const ssize_t len = fd.read(const_cast<char*>(str.data()), nbyte);
    if (len < 0) {
        return "";
    }
    str.shrink_to_fit();
    return str;
}

ssize_t write(FileDescriptor& fd, string_view buf) {
    return fd.write(buf.data(), buf.size());
}

Socket::Socket(int h)
    : FileDescriptor(h) {}

sockaddr_in Socket::get_address() const {
    return address;
}

sockaddr_in& Socket::mutable_address()  {
    return address;
}

ServerSocket::ServerSocket(Port port)
    : Socket(socket(AF_INET, SOCK_STREAM, 0)) {
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (bind(handler, reinterpret_cast<const sockaddr*>(&address),
        sizeof(address)) < 0 || listen(handler, SOMAXCONN) < 0) {
        die("Unable to create socket: ");
    }
}

ClientSocket accept_client(ServerSocket& serv_sock) {
    ClientSocket client;
    auto& client_address = client.mutable_address();
    socklen_t client_address_size = sizeof(client_address);
    const auto new_client_handler = ::accept(serv_sock.handler,
        reinterpret_cast<sockaddr*>(&client_address), &client_address_size);
    if (new_client_handler < 0) {
        die("Unable to accept: ");
    }
    client.handler = new_client_handler;
    return client;
}

}  // namespace pkr
