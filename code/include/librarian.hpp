// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <string>
#include <tuple>
#include <unordered_map>

#include "include/string_view.hpp"

namespace pkr {

class Librarian {
public:
    // clang-format off
    using file_type = std::string;
    using file_ptr  = const file_type*;
    using hash_type = std::string;
    // clang-format on

    file_ptr add(string_view, string_view, std::string);
    file_ptr find(string_view, string_view);

private:
    hash_type hash(string_view, string_view);

    std::unordered_map<hash_type, file_type> storage;
};

}  // namespace pkr
