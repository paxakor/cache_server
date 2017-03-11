#include <cstring>
#include <iostream>
#include <iterator>
#include "include/utils.hpp"

int test_pattern() {
    std::string pattern;
    std::cin >> pattern;
    std::istream_iterator<char> begin(std::cin), end;
    std::string input(begin, end);
    for (const auto& line : pkr::split(input, pattern)) {
        std::cout << line.to_string() << '\n';
    }
    return 0;
}

int test_space() {
    std::cin >> std::noskipws;
    std::istream_iterator<char> begin(std::cin), end;
    std::string input(begin, end);
    for (const auto& line : pkr::split(input)) {
        std::cout << line.to_string() << "\n";
    }
    return 0;
}

int main(int argc, const char **argv) {
    if (argc < 2) {
    } else if (strcmp(argv[1], "pattern") == 0) {
        return test_pattern();
    } else if (strcmp(argv[1], "space") == 0) {
        return test_space();
    }
    std::cerr << __FILE__ ": invalid arguments" << std::endl;
    return 1;
}
