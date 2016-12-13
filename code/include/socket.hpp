// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <arpa/inet.h>
#include <stddef.h>
#include <string>
#include "include/buffer.hpp"
#include "include/string_view.hpp"

namespace pkr {

using Port = uint16_t;

class Socket {
public:
    Socket() = default;
    Socket(int);
    virtual ~Socket();
    int get_handler() const;
    void set_handler(int);
    sockaddr_in get_address() const;
    sockaddr_in& mutable_address();
    std::string read(size_t);
    ssize_t read(char*, size_t);
    ssize_t read_unbuf(char*, size_t);
    std::string read_until(string_view);
    ssize_t write(const std::string&);
    ssize_t write(const void*, size_t);

protected:
    ssize_t fill_buffer();

protected:
    Buffer<1024> buffer_in;  // buffer size = 1KiB
    int handler;
    sockaddr_in address;
};

class ClientSocket : public Socket {
};

class ServerSocket : public Socket {
public:
    ServerSocket(Port);
    void accept(ClientSocket&);
};

}  // namespace pkr
