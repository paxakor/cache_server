// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <array>
#include <string>
#include <utility>
#include "include/socket.hpp"
#include "include/string_view.hpp"

namespace pkr {

class Client {
public:
    Client(ClientSocket);
    Port get_port() const;
    std::string get_ip() const;
    std::string read(size_t);
    ssize_t write_failure(int, const std::string&);
    ssize_t write_response(int, size_t);
    ssize_t write(string_view);

private:
    ClientSocket socket;
    Port port;
    std::array<char, INET_ADDRSTRLEN> ip;
};

std::string read(Client&, size_t = FileDescriptor::MAX_REQUEST_SIZE);
ssize_t write(Client&, string_view);

}  // namespace pkr
