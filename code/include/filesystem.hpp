// Copyright 2016, Pavel Korozevtsev.

#pragma once

#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#else
    #ifdef USE_EXPSTL_FILESYSTEM
        #include <experimental/filesystem>
        namespace fs = std::experimental::filesystem;
#else
    #ifdef USE_STL_FILESYSTEM
        #include <filesystem>
        namespace fs = std::filesystem;
#else

#include <cstdint>
#include <string>

namespace pkr {
namespace filesystem {

using path = std::string;
bool exists(const path&);
bool is_directory(const path&);
std::uintmax_t file_size(const path&);

}  // namespace filesystem
}  // namespace pkr

namespace fs = pkr::filesystem;

#endif
#endif
#endif
