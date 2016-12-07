// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <cstdint>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

namespace pkr {

class Socket {
public:
    Socket() = default;
    Socket(int);
    virtual ~Socket();
    int get_handler() const;
    void set_handler(int);
    sockaddr_in get_address() const;
    sockaddr_in& mutable_address();

protected:
    int handler;
    sockaddr_in address;
};

class ClientSocket : public Socket {
};

class ServerSocket : public Socket {
public:
    ServerSocket(uint16_t);
    void accept(ClientSocket&);
};

}  // namespace pkr
