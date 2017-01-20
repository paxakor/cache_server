// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <array>
#include <string>
#include "include/socket.hpp"

namespace pkr {

class Client {
public:
    Client(ServerSocket&);
    ClientSocket& mutable_socket();
    const ClientSocket& get_socket() const;
    Port get_port() const;
    std::string get_ip() const;
    ssize_t write_failure(int, const std::string&);
    ssize_t write_response(int, size_t);

private:
    ClientSocket socket;
    Port port;
    std::array<char, INET_ADDRSTRLEN> ip;
};

}  // namespace pkr
