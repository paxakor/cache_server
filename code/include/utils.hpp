// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include <vector>
#include "include/string_view.hpp"

namespace pkr {

std::string strip(string_view);
std::vector<std::string> split(string_view, char = 0);
std::vector<std::string> split(string_view, string_view);
std::vector<std::string> split_n(string_view, size_t, char = 0);
std::vector<std::string> split_n(string_view, size_t, string_view);
const char* search(const char*, const char*, string_view);

}  // namespace pkr
