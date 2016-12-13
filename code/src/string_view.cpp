// Copyright 2016, Pavel Korozevtsev.

#include <string.h>
#include <algorithm>
#include <string>
#include <utility>
#include "include/string_view.hpp"

namespace pkr {

string_view::size_type string_view::npos = static_cast<size_type>(-1);

string_view::string_view(const std::string& s)
    : ptr(s.data())
    , len(s.size()) { }

string_view::string_view(const_pointer p, string_view::size_type l)
    : ptr(p)
    , len(l) { }

string_view::string_view(const_pointer p)
    : ptr(p)
    , len(strlen(p)) { }


string_view::iterator string_view::begin() {
    return ptr;
}

string_view::iterator string_view::end() {
    return ptr + len;
}

string_view::const_iterator string_view::cbegin() const {
    return ptr;
}

string_view::const_iterator string_view::cend() const {
    return ptr + len;
}

string_view::const_pointer string_view::data() const {
    return ptr;
}

string_view::const_reference string_view::at(string_view::size_type pos) const {
    return pos < len ? ptr[pos] : back();
}

string_view::const_reference string_view::back() const {
    return ptr[len - 1];
}

string_view::const_reference string_view::front() const {
    return ptr[0];
}

string_view::const_reference string_view::operator[](
    string_view::size_type pos) const {
    return ptr[pos];
}

bool string_view::empty() const {
    return len == 0;
}

string_view::size_type string_view::length() const {
    return size();
}

string_view::size_type string_view::size() const {
    return len;
}

void string_view::remove_prefix(string_view::size_type n) {
    ptr += n;
}

void string_view::remove_suffix(string_view::size_type n) {
    len -= n;
}

void string_view::swap(string_view& sv) {
    using std::swap;
    swap(ptr, sv.ptr);
    swap(len, sv.len);
}

std::string string_view::to_string() const {
    return {ptr, len};
}

string_view::operator std::string() const {
    return to_string();
}

string_view string_view::substr(string_view::size_type pos,
    string_view::size_type cnt) const {
    pos = std::min(pos, size());
    cnt = std::min(cnt, size() - pos);
    return {ptr + pos, cnt};
}

string_view::size_type string_view::copy(pointer dest,
    string_view::size_type cnt, string_view::size_type pos) const {
    cnt = std::min(cnt, size() - pos);
    strncpy(dest, ptr + pos, cnt);
    return cnt;
}

string_view::size_type string_view::find(string_view sv,
    string_view::size_type pos) const {
    for (size_type i = pos; i + sv.len <= len; ++i) {
        if (string_view(ptr + i, sv.size()) == sv) {
            return i;
        }
    }
    return npos;
}

bool operator==(string_view lhs, string_view rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(string_view lhs, string_view rhs) {
    return !(lhs == rhs);
}

bool operator< (string_view lhs, string_view rhs) {
    for (size_t i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return lhs[i] < rhs[i];
        }
    }
    return lhs.size() < rhs.size();
}

bool operator<=(string_view lhs, string_view rhs) {
    for (size_t i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return lhs[i] < rhs[i];
        }
    }
    return lhs.size() <= rhs.size();
}

bool operator> (string_view lhs, string_view rhs) {
    return !(lhs <= rhs);
}

bool operator>=(string_view lhs, string_view rhs) {
    return !(lhs < rhs);
}

}  // namespace pkr
