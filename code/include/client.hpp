// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <array>
#include <string>
#include <utility>
#include "include/socket.hpp"

namespace pkr {

class Client {
public:
    Client(ClientSocket);
    Port get_port() const;
    std::string get_ip() const;
    ssize_t write_failure(int, const std::string&);
    ssize_t write_response(int, size_t);
    template<typename... Ts> ssize_t write(Ts&&...);
    std::string read_header();

private:
    ClientSocket socket;
    Port port;
    std::array<char, INET_ADDRSTRLEN> ip;
};

template<typename... Ts>
ssize_t Client::write(Ts&&... vars) {
    return socket.write(std::forward<Ts>(vars)...);
}

}  // namespace pkr
