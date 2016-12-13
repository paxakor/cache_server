// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <map>
#include <string>
#include <vector>
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
    Method method;
    std::string URI;
    Header head;
    std::string body;
};

Message parse_header(string_view);

}  // namespace pkr
