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
    enum : size_t { MAX_REQUEST_SIZE = 65536 };
public:
    FileDescriptor(int);
    virtual ~FileDescriptor();
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&);
    ssize_t read(char*, size_t);
    ssize_t write(const void*, size_t);

protected:
    int handler;
};

std::string read(FileDescriptor&, size_t);
ssize_t write(FileDescriptor&, string_view);

class Socket : public FileDescriptor {
public:
    Socket(const Socket&) = delete;
    Socket(Socket&&) = default;
    sockaddr_in get_address() const;
    sockaddr_in& mutable_address();

protected:
    Socket(int);

protected:
    sockaddr_in address;
};

class ClientSocket;
class ServerSocket;
ClientSocket accept_client(ServerSocket&);

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
