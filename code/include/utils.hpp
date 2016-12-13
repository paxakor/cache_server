// Copyright 2016, Pavel Korozevtsev.

#pragma once

#include <string>
#include <vector>
#include "include/string_view.hpp"

namespace pkr {

std::vector<std::string> split(string_view, string_view, size_t);
std::vector<std::string> split(string_view, size_t = 0, char = 0);
const char* search(const char*, const char*, string_view);

}  // namespace pkr
