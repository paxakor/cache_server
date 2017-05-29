// Copyright 2017, Pavel Korozevtsev.

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <experimental/string_view>
#include <fstream>
#include <functional>
#include <iterator>
#include <list>
#include <memory>
#include <string>
#include <thread>

#include "stopwatch.hpp"

using File = std::string;
using std::experimental::string_view;

void passert(bool val, const char* msg, bool use_perror = true) {
    if (!val) {
        if (use_perror) {
            perror(msg);
        } else {
            fprintf(stderr, "insatiable_client's error: %s\n", msg);
        }
        exit(1);
    }
}

File readfd(int fd) {
    enum { buf_sz = 4096 };
    ssize_t len;
    File file;
    char buffer[buf_sz];
    while ((len = read(fd, buffer, buf_sz)) > 0) {
        file.append(buffer, buffer + len);
    }
    return file;
}

File readfile(const std::string& fname) {
    auto fd = open(fname.c_str(), O_RDONLY);
    passert(fd > 0, "can't open file");
    const auto file = readfd(fd);
    close(fd);
    return file;
}

string_view find_after(const std::string& buffer, const std::string& pattern) {
    const auto begin = std::search(buffer.begin(), buffer.end(),
                                   pattern.begin(), pattern.end()) +
                       pattern.size();
    return {&*begin, static_cast<size_t>(std::distance(begin, buffer.end()))};
}

string_view find_response_body(const File& buffer) {
    return find_after(buffer, "\r\n\r\n");
}

int connect_to_server(const struct addrinfo* serv_addr) {
    for (auto rp = serv_addr; rp; rp = rp->ai_next) {
        auto sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue;
        }
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            return sockfd;
        }
        close(sockfd);
    }
    return -1;
}

void send_request(const std::string& request,
                  const File& file,
                  std::shared_ptr<struct addrinfo> serv_addr) {
    auto sockfd = connect_to_server(serv_addr.get());
    passert(sockfd >= 0, "can't open socket");
    passert(write(sockfd, request.c_str(), request.size()) ==
                static_cast<ssize_t>(request.size()),
            "can't write");
    const auto buffer = readfd(sockfd);
    close(sockfd);
    const auto body = find_response_body(buffer);
    passert(file == body, "files are not equal", false);
}

std::shared_ptr<struct addrinfo> make_serv_addr(const char* hostname,
                                                const char* port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    struct addrinfo* result = nullptr;
    auto s = getaddrinfo(hostname, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return nullptr;
    }
    return {result, freeaddrinfo};
}

template <typename Int, typename... Ts>
void test(Int sessions, const Ts&... args) {
    for (Int i = 0; i < sessions; ++i) {
        send_request(args...);
    }
}

template <typename... Ts>
std::list<std::thread> make_threads(int thread_count, const Ts&... args) {
    std::list<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(test<Ts...>, std::cref(args)...);
    }
    return threads;
}

int main(int argc, char** argv) {
    if (argc < 6) {
        fprintf(stderr, "usage: %s hostname port file sessions threads\n",
                argv[0]);
        return 1;
    }
    const auto request =
        "GET / HTTP/1.0\r\nHost: " + std::string(argv[1]) + "\r\n\r\n";
    const auto serv_addr = make_serv_addr(argv[1], argv[2]);
    const auto sessions = atoi(argv[4]);
    const auto thread_count = atoi(argv[5]);
    const auto file = readfile(argv[3]);
    Stopwatch sw("DDoS");
    auto threads =
        make_threads(thread_count, sessions, request, file, serv_addr);
    for (auto& th : threads) {
        th.join();
    }
    return 0;
}
