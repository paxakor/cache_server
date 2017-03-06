// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>
#include <arpa/inet.h>
#include <string>
#include "include/buffer.hpp"
#include "include/string_view.hpp"

namespace pkr {

class ClientSocket;
class ServerSocket;
ClientSocket accept_client(ServerSocket&);

using Port = uint16_t;

class Socket {
public:
    virtual ~Socket();
    Socket(const Socket&) = delete;
    Socket(Socket&&) = default;
    sockaddr_in get_address() const;
    sockaddr_in& mutable_address();
    std::string read(size_t);
    ssize_t read(char*, size_t);
    ssize_t write(const std::string&);
    ssize_t write(const void*, size_t);

protected:
    Socket(int);
    int get_handler() const;
    void set_handler(int);
    ssize_t read_unbuf(char*, size_t);
    ssize_t fill_buffer();

protected:
    Buffer<1024> buffer_in;  // buffer size = 1KiB
    int handler;
    sockaddr_in address;
};

class ClientSocket : public Socket {
    friend ClientSocket accept_client(ServerSocket&);

public:
    ClientSocket(const ClientSocket&) = delete;
    ClientSocket(ClientSocket&&) = default;
    std::string read_header();

protected:
    ClientSocket() : Socket(0) {}
};

class ServerSocket : public Socket {
    friend ClientSocket accept_client(ServerSocket&);

public:
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket(ServerSocket&&) = default;
    ServerSocket(Port);
};

}  // namespace pkr
