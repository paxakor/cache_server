// Copyright 2016, Pavel Korozevtsev.

#pragma once

#ifdef USE_BOOST_FILESYSTEM
    #include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;
#elif __cplusplus < 201103
    #error "Sorry, stdc++ is too old for filesystem. See README"
#elif __cplusplus < 201700  // don't know when std::filesystem will be available
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif
