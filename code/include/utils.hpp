// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <utility>
#include <vector>

#include "include/string_view.hpp"

namespace pkr {

std::vector<string_view::iterator> find_all(string_view, string_view);
std::vector<string_view> split(string_view);
std::vector<string_view> split(string_view, char);
std::vector<string_view> split(string_view, string_view);
std::vector<string_view> split_n(string_view, size_t);
std::vector<string_view> split_n(string_view, size_t, char);
std::vector<string_view> split_n(string_view, size_t, string_view);
string_view strip(string_view);

template <typename T, typename U = T>
T exchange(T& obj, U&& new_val) {
    T old_val = std::move(obj);
    obj = std::forward<U>(new_val);
    return old_val;
}

}  // namespace pkr
