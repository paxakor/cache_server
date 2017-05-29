// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <netdb.h>
#include <sys/socket.h>

#include <memory>
#include <string>

#include "include/parser.hpp"
#include "include/socket.hpp"
#include "include/string_view.hpp"

namespace pkr {

std::shared_ptr<addrinfo> make_serv_addr(string_view, string_view);
Socket connect_to_server(std::shared_ptr<addrinfo>);

}  // namespace pkr
