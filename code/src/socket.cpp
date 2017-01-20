// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstddef>
#include <cstring>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
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

std::string Socket::read_header() {
    std::string header;
    // There is a finite-state automaton emulation.
    // States:
    //   0 -- nothing special
    //   1 -- "\r" found
    //   2 -- "\r\n" found
    //   3 -- "\r\n\r" found
    //   4 -- "\r\n\r\n" found
    int state = 0;
    char need = '\r';
    while (buffer_in.size() > 0 || fill_buffer() > 0) {
        auto it = std::find(buffer_in.begin(), buffer_in.end(), need);
        while (it != buffer_in.end() && *it == need && state != 4) {
            ++state;
            ++it;
            need = (need == '\n') ? '\r' : '\n';
        }
        header.append(buffer_in.begin(), it);
        buffer_in.remove_prefix(it - buffer_in.begin());
        if (state == 4) {
            break;
        }
        if (*it != need) {
            state = 0;
        }
    }
    return header;
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
