// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <map>
#include <string>

#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

using Header = std::map<std::string, std::string>;

enum class Method {
    UNKNOWN,
    GET,
    HEAD,
    POST,
};

struct Message {
    Method method = Method::UNKNOWN;
    std::string url;
    Header head;
    std::string body;
};

Message parse_header(string_view);
std::string debug_header(const Message&);
string_view find_header(string_view);

}  // namespace pkr
