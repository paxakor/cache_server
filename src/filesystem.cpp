// Copyright 2016-2017, Pavel Korozevtsev.

#include <sys/stat.h>

#include "include/filesystem.hpp"

namespace pkr {
namespace filesystem {

bool exists(const path& pathname) {
    struct stat st;
    return stat(pathname.c_str(), &st) == 0;
}

bool is_directory(const path& pathname) {
    struct stat st;
    return stat(pathname.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

std::uintmax_t file_size(const path& pathname) {
    struct stat st;
    stat(pathname.c_str(), &st);
    return st.st_size;
}

}  // namespace filesystem
}  // namespace pkr
