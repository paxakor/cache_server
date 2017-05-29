// Copyright 2016-2017, Pavel Korozevtsev.

#include <algorithm>
#include <iterator>
#include <vector>

#include "include/log.hpp"
#include "include/parser.hpp"
#include "include/utils.hpp"

namespace pkr {

void parse_first_line(string_view line, Message& msg) {
    const auto args = split(line);
    if (args.size() < 3) {
        if (line.size() <= 80) {
            log.message("invalid request line: \n" + line + "\n");
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

void parse_header(string_view header, Message& msg) {
    const auto lines = split(header, "\r\n");
    if (lines.size() == 0) {
        log.error("empty header");
        return;
    }
    parse_first_line(lines[0], msg);
    for (auto it = lines.begin() + 1; it != lines.end() && it->size(); ++it) {
        const auto parts = split_n(*it, 2, ':');
        if (parts.size() == 2) {
            msg.head[parts[0]] = strip(parts[1]);
        }
    }
    const auto host_pos = msg.head.find("Host");
    if (host_pos == msg.head.end()) {
        msg.method = Method::UNKNOWN;
        return;
    }
    auto& host = host_pos->second;
    {
        const auto pos = std::search(msg.url.begin(), msg.url.end(),
                                     host.begin(), host.end());
        if (pos != msg.url.end()) {
            msg.url = string_view(pos + host.size(), msg.url.end());
        }
    }
    {
        const auto pos = std::find(host.begin(), host.end(), ':');
        if (pos != host.end()) {
            msg.service = std::string(pos + 1, host.end());
            host = std::string(host.begin(), pos);
        }
    }
}

Message::Message(string_view req)
    : original(req) {
    static const string_view delim("\r\n\r\n");
    auto pos = std::search(req.begin(), req.end(), delim.begin(), delim.end());
    parse_header({req.begin(), pos}, *this);
    body = pos == req.end() ? "" : string_view{pos + delim.size(), req.end()};
}

std::string join_first_line(const Message& msg) {
    std::string line;
    if (msg.method == Method::GET) {
        line += "GET ";
    } else if (msg.method == Method::HEAD) {
        line += "HEAD ";
    } else if (msg.method == Method::POST) {
        line += "POST ";
    } else {
        line += "UNKNOWN ";
    }
    line += msg.url;
    line += " HTTP/" + msg.version + "\r\n";
    return line;
}

std::string join_header(const Message& msg) {
    std::string header;
    header.reserve(2048);
    for (const auto& param : msg.head) {
        header += param.first;
        header += ": ";
        header += param.second;
        header += "\r\n";
    }
    return header;
}

std::string join_message(const Message& msg) {
    return join_first_line(msg) + join_header(msg) + "\r\n" + msg.body;
}

}  // namespace pkr
