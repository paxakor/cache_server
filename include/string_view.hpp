// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>

#include <string>

namespace pkr {

class string_view {
public:
    // clang-format off
    using iterator        = const char*;
    using const_iterator  = const char*;
    using pointer         = char*;
    using const_pointer   = const char*;
    using reference       = char&;
    using const_reference = const char&;
    using size_type       = std::size_t;
    using value_type      = char;
    // clang-format on

    string_view() = default;
    string_view(const string_view&) = default;
    string_view(string_view&&) = default;
    string_view& operator=(const string_view& view) = default;
    string_view(const std::string&);
    string_view(const_pointer, size_type);
    string_view(const_pointer);
    string_view(const_iterator, const_iterator);

    iterator begin() const;
    iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    const_pointer data() const;
    const_reference at(size_type) const;
    const_reference back() const;
    const_reference front() const;
    const_reference operator[](size_type) const;

    bool empty() const;
    size_type length() const;
    size_type size() const;

    void remove_prefix(size_type);
    void remove_suffix(size_type);
    void swap(string_view&);

    std::string to_string() const;
    operator std::string() const;
    string_view substr(size_type = 0, size_type = npos) const;
    size_type copy(pointer, size_type, size_type = 0) const;
    size_type find(string_view, size_type = 0) const;

public:
    enum : size_type { npos = static_cast<size_type>(-1) };

protected:
    const_pointer ptr = nullptr;
    size_type len = 0;
};

// clang-format off
bool operator==(string_view, string_view);
bool operator!=(string_view, string_view);
bool operator< (string_view, string_view);
bool operator<=(string_view, string_view);
bool operator> (string_view, string_view);
bool operator>=(string_view, string_view);
// clang-format on

std::string operator+(string_view, const std::string&);
std::string operator+(const std::string&, string_view);

}  // namespace pkr
