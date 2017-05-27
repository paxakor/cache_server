// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <netdb.h>
#include <sys/socket.h>

#include <memory>
#include <string>

#include "include/parser.hpp"
#include "include/socket.hpp"

namespace pkr {

std::shared_ptr<addrinfo> make_serv_addr(const std::string&);
Socket connect_to_server(std::shared_ptr<addrinfo>);

}  // namespace pkr
