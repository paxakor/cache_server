// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

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

}  // namespace pkr
