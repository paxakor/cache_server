// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>
#include <arpa/inet.h>
#include <string>
#include "include/defs.hpp"
#include "include/string_view.hpp"

namespace pkr {

class FileDescriptor {
public:
    enum : size_t { max_request_size = 65536 };
public:
    FileDescriptor(int);
    virtual ~FileDescriptor();
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&);
    ssize_t read(void*, size_t);
    ssize_t write(const void*, size_t);

protected:
    int handler;
};

std::string read(FileDescriptor&, size_t = FileDescriptor::max_request_size);
ssize_t write(FileDescriptor&, string_view);

class Socket : public FileDescriptor {
public:
    Socket(const Socket&) = delete;
    Socket(Socket&&) = default;

protected:
    Socket(int);
};


class Epoll;
class ClientSocket;
class ServerSocket;
ClientSocket accept_client(ServerSocket&);
Epoll make_epoll(ServerSocket&);

class ClientSocket : public Socket {
    friend ClientSocket accept_client(ServerSocket&);

public:
    ClientSocket(const ClientSocket&) = delete;
    ClientSocket(ClientSocket&&) = default;
    ssize_t write_failure(int, const std::string&);
    ssize_t write_response(int, size_t);

protected:
    ClientSocket(int);
};

class ServerSocket : public Socket {
    friend ClientSocket accept_client(ServerSocket&);
    friend Epoll make_epoll(ServerSocket&);

public:
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket(ServerSocket&&) = default;
    ServerSocket(Port);
};

ssize_t write_failure(ClientSocket&, int, string_view);
ssize_t write_response(ClientSocket&, int, size_t);

}  // namespace pkr
