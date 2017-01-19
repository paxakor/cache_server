// Copyright 2016, Pavel Korozevtsev.

#include <cstddef>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include "include/buffer.hpp"
#include "include/socket.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

Socket::Socket(int h)
    : handler(h) { }

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

std::string Socket::read(size_t nbyte) {
    std::string buf(nbyte, 0);
    read(const_cast<char*>(buf.data()), nbyte);
    buf.resize(strlen(buf.data()));
    return buf;
}

ssize_t Socket::read(char* dest, size_t nbyte) {
    const auto bf_sz = buffer_in.size();
    if (nbyte > bf_sz) {
        buffer_in.read_all(dest);
        nbyte -= bf_sz;
        dest += bf_sz;
        return read_unbuf(dest, nbyte);
    } else {
        buffer_in.read(dest, nbyte);
        return nbyte;
    }
}

ssize_t Socket::read_unbuf(char* dest, size_t nbyte) {
    return ::read(get_handler(), dest, nbyte);
}

std::string Socket::read_until(string_view delim) {
    std::string buf;
    while (fill_buffer() > 0) {
        const auto entry = search(buffer_in.begin(), buffer_in.end(), delim);
        buf.append(buffer_in.begin(), entry);
        if (entry != buffer_in.end()) {
            break;
        }
        buffer_in.drop();
    }
    return buf;
}

ssize_t Socket::write(const std::string& buf) {
    return write(buf.data(), buf.size());
}

ssize_t Socket::write(const void* buf, size_t nbyte) {
    return ::write(get_handler(), buf, nbyte);
}

ssize_t Socket::fill_buffer() {
    const auto len = buffer_in.size();
    const auto nb = read_unbuf(buffer_in.buf + len, buffer_in.capacity() - len);
    if (nb > 0) {
        buffer_in.len += nb;
    }
    return nb;
}

ServerSocket::ServerSocket(Port port)
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
