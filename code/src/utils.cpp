// Copyright 2016, Pavel Korozevtsev.

#include <cctype>
#include <string>
#include <vector>
#include "include/utils.hpp"

namespace pkr {

std::vector<std::string> split(const std::string& str,
    const size_t max_count, const char delim) {
    auto is_delim = [delim](const char ch) -> bool {
        if (delim) {
            return ch == delim;
        } else {
            return std::isspace(ch);
        }
    };

    size_t pos = 0;
    std::vector<std::string> res;
    while (pos < str.size() && (max_count == 0 || res.size() + 1 < max_count)) {
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

}  // namespace pkr
