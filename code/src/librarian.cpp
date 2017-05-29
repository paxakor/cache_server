// Copyright 2017, Pavel Korozevtsev.

#include <string>
#include <tuple>
#include <unordered_map>

#include "include/librarian.hpp"
#include "include/string_view.hpp"
#include "include/utils.hpp"

namespace pkr {

Librarian::file_ptr
Librarian::add(string_view host, string_view url, std::string page) {
    return &storage.emplace(hash(host, url), std::move(page)).first->second;
}

Librarian::file_ptr Librarian::find(string_view host, string_view url) {
    const auto pos = storage.find(hash(host, url));
    return pos == storage.end() ? nullptr : &pos->second;
}

Librarian::hash_type Librarian::hash(string_view host, string_view url) {
    return host.to_string() + '/' + url.to_string();
}

}  // namespace pkr
