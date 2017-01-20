// Copyright 2016-2017, Pavel Korozevtsev.

#include <cctype>
#include <algorithm>
#include <string>
#include <vector>
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

std::string strip(string_view text) {
    while (std::isspace(text.front())) {
        text.remove_prefix(1);
    }
    while (std::isspace(text.back())) {
        text.remove_suffix(1);
    }
    return text.to_string();
}

std::vector<size_t> find_all(string_view str, string_view pattern) {
    std::vector<size_t> ret;
    size_t pos = 0;
    size_t offset = 0;
    while ((pos = str.find(pattern)) != string_view::npos) {
        ret.push_back(pos + offset);
        str.remove_prefix(pos + pattern.size());
        offset += pos + pattern.size();
    }
    return ret;
}

std::vector<std::string> split(string_view str, char delim) {
    auto is_delim = [delim](const char ch) -> bool {
        if (delim) {
            return ch == delim;
        } else {
            return std::isspace(ch);
        }
    };

    size_t pos = 0;
    std::vector<std::string> res;
    while (pos < str.size()) {
        while (pos < str.size() && is_delim(str[pos])) {
            ++pos;
        }
        size_t len = 0;
        while ((pos + len) < str.size() && !is_delim(str[pos + len])) {
            ++len;
        }
        if (len > 0) {
            res.push_back(str.substr(pos, len));
        }
        pos += len + 1;
    }
    if (pos < str.size()) {
        res.push_back(str.substr(pos, str.size() - pos));
    }
    return res;
}

std::vector<std::string> split(string_view str, string_view delim) {
    const auto matches = find_all(str, delim);
    size_t i = 0;
    size_t pos = 0;
    std::vector<std::string> ret;

    auto advance_pos = [&]() {
        for (; i < matches.size() && pos == matches[i]; ++i) {
            pos += delim.size();
        }
    };

    advance_pos();
    for (; i < matches.size(); ++i) {
        if (i < matches.size()) {
            ret.push_back(str.substr(pos, matches[i] - pos));
            pos = matches[i] + delim.size();
        }
        advance_pos();
    }
    if (pos < str.size()) {
        ret.push_back(str.substr(pos, str.size() - pos));
    }
    return ret;
}

std::vector<std::string> split_n(string_view str, size_t max_cnt, char delim) {
    auto is_delim = [delim](const char ch) -> bool {
        if (delim) {
            return ch == delim;
        } else {
            return std::isspace(ch);
        }
    };

    size_t pos = 0;
    std::vector<std::string> res;
    while (pos < str.size() && res.size() + 1 < max_cnt) {
        while (pos < str.size() && is_delim(str[pos])) {
            ++pos;
        }
        size_t len = 0;
        while ((pos + len) < str.size() && !is_delim(str[pos + len])) {
            ++len;
        }
        if (len > 0) {
            res.push_back(str.substr(pos, len));
        }
        pos += len + 1;
    }
    if (pos < str.size()) {
        res.push_back(str.substr(pos, str.size() - pos));
    }
    return res;
}

std::vector<std::string> split_n(string_view str, size_t max_cnt, string_view delim) {
    const auto matches = find_all(str, delim);
    size_t i = 0;
    size_t pos = 0;
    std::vector<std::string> ret;

    auto advance_pos = [&]() {
        for (; i < matches.size() && pos == matches[i]; ++i) {
            pos += delim.size();
        }
    };

    advance_pos();
    for (; i < matches.size() && ret.size() + 1 < max_cnt; ++i) {
        if (i < matches.size()) {
            ret.push_back(str.substr(pos, matches[i] - pos));
            pos = matches[i] + delim.size();
        }
        advance_pos();
    }
    if (pos < str.size()) {
        ret.push_back(str.substr(pos, str.size() - pos));
    }
    return ret;
}

const char* search(const char* begin, const char* end, string_view str) {
    return std::search(begin, end, str.begin(), str.end());
}

}  // namespace pkr
