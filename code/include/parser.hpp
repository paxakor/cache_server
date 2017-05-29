// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <map>
#include <string>

#include "include/string_view.hpp"

namespace pkr {

using Header = std::map<std::string, std::string>;

enum class Method {
    UNKNOWN,
    GET,
    HEAD,
    POST,
};

struct Message {
    Message(string_view);

    Method method = Method::UNKNOWN;
    std::string service = "http";
    std::string version = "1.0";
    string_view url;

    Header head;
    string_view body;
    string_view original;
};

std::string join_message(const Message&);

}  // namespace pkr
