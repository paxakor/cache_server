// Copyright 2016, Pavel Korozevtsev.

#include <string>
#include <vector>
#include "include/log.hpp"
#include "include/parser.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

Message parse_header(string_view text) {
    Message msg;
    msg.method = Method::UNKNOWN;
    const auto lines = split(text, "\r\n", 2);
    const auto& request = lines[0];
    const auto args = split(request);
    if (args.size() < 3) {
        if (request.size() <= 80) {
            log.message("invalid request line: \n" + request + "\n");
        } else {
            log.message("invalid request line (too long to show)");
        }
        return msg;
    }
    if (args[0] == "GET") {
        msg.method = Method::GET;
    } else if (args[0] == "HEAD") {
        msg.method = Method::HEAD;
    } else if (args[0] == "POST") {
        msg.method = Method::POST;
    }
    msg.URI = args[1];
    return msg;
}

}  // namespace pkr
