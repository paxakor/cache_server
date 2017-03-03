// Copyright 2016-2017, Pavel Korozevtsev.

#include <string>
#include <vector>
#include "include/log.hpp"
#include "include/parser.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

void parse_starting_line(string_view request, Message& msg) {
    const auto args = split(request);
    if (args.size() < 3) {
        if (request.size() <= 80) {
            log.message("invalid request line: \n" + request + "\n");
        } else {
            log.message("invalid request line (too long to show)");
        }
        return;
    }
    if (args[0] == "GET") {
        msg.method = Method::GET;
    } else if (args[0] == "HEAD") {
        msg.method = Method::HEAD;
    } else if (args[0] == "POST") {
        msg.method = Method::POST;
    }
    msg.url = args[1];
}

Message parse_header(string_view header) {
    Message msg;
    const auto lines = split(header, "\r\n");
    parse_starting_line(lines[0], msg);
    for (auto it = lines.begin() + 1; it != lines.end(); ++it) {
        const auto parts = split_n(*it, 2, ':');
        if (parts.size() != 2) {
            if (it->size() != 0) {
                log.message("invalid header line: " + *it);
            }
            continue;
        }
        msg.head[parts[0]] = strip(parts[1]);
    }
    return msg;
}

std::string debug_header(const Message& msg) {
    std::string header;
    header.reserve(2048);
    if (msg.method == Method::GET) {
        header += "GET ";
    } else if (msg.method == Method::HEAD) {
        header += "HEAD ";
    } else if (msg.method == Method::POST) {
        header += "POST ";
    } else {
        header += "UNKNOWN ";
    }
    header += msg.url;
    header += " HTTP/xxx\r\n";
    for (const auto& param : msg.head) {
        header += param.first;
        header += ": ";
        header += param.second;
        header += "\r\n";
    }
    return header;
}

}  // namespace pkr
