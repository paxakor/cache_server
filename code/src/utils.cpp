// Copyright 2016-2017, Pavel Korozevtsev.

#include <cctype>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>
#include "include/defs.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {
namespace detail {

template <typename Searcher, typename Advancer>
std::vector<string_view::iterator> find_all(string_view str, size_t n,
                                            Searcher search, Advancer advance) {
    std::vector<string_view::iterator> ret;
    string_view::iterator pos = str.begin();
    string_view::iterator offset;
    while (ret.size() < n && (offset = search(str, pos)) != str.end()) {
        ret.push_back(offset);
        pos = advance(offset);
    }
    return ret;
}

template <typename Searcher, typename Advancer>
std::vector<string_view> split_helper(string_view str, size_t n,
                                      Searcher search, Advancer advance) {
    auto delims = find_all(str, n - 1, search, advance);   // assume n > 1
    delims.push_back(str.end());
    std::vector<string_view> res;
    auto pos = str.begin();
    if (std::distance(pos, delims.front()) > 0) {
        res.emplace_back(pos, delims.front());
    }
    for (auto iter = delims.begin(); iter + 1 != delims.end();) {
        pos = advance(*iter);
        ++iter;
        if (std::distance(pos, *iter) > 0) {
            res.emplace_back(pos, *iter);
        }
    }
    DO_DEBUG(assert(std::count(res.begin(), res.end(), string_view("")) == 0));
    return res;
}

template <typename Delimiter>
std::vector<string_view> split_by_char(string_view str, size_t n,
                                       Delimiter is_delim) {
    auto search = [is_delim](string_view& str, string_view::iterator pos) {
        return std::find_if(pos, str.end(), is_delim);
    };
    auto advance = [](string_view::iterator pos) {
        return ++pos;
    };
    return split_helper(str, n, search, advance);
}

}  // namespace detail

std::vector<string_view> split(string_view str) {
    return split_n(str, std::numeric_limits<size_t>::max());
}

std::vector<string_view> split(string_view str, char delim) {
    return split_n(str, std::numeric_limits<size_t>::max(), delim);
}

std::vector<string_view> split(string_view str, string_view delim) {
    return split_n(str, std::numeric_limits<size_t>::max(), delim);
}

std::vector<string_view> split_n(string_view str, size_t n) {
    auto is_delim = [](const char ch) {
        return std::isspace(ch);
    };
    return detail::split_by_char(str, n, is_delim);
}

std::vector<string_view> split_n(string_view str, size_t n, char delim) {
    auto is_delim = [delim](const char ch) {
        return ch == delim;
    };
    return detail::split_by_char(str, n, is_delim);
}

std::vector<string_view> split_n(string_view str, size_t n, string_view delim) {
    auto search = [&delim](string_view& str, string_view::iterator pos) {
        return std::search(pos, str.end(), delim.begin(), delim.end());
    };
    auto advance = [delim](string_view::iterator pos) {
        std::advance(pos, delim.size());
        return pos;
    };
    return detail::split_helper(str, n, search, advance);
}

string_view strip(string_view text) {
    while (std::isspace(text.front())) {
        text.remove_prefix(1);
    }
    while (std::isspace(text.back())) {
        text.remove_suffix(1);
    }
    return text;
}

}  // namespace pkr
