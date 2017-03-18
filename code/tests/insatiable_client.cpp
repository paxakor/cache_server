// Copyright 2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <functional>
#include <iterator>
#include <list>
#include <thread>

#include "stopwatch.hpp"

enum { FILESZ = 4096 };
using File = char[FILESZ];

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

const char* find_response_body(const File& buffer) {
    static const char pattern[] = "\r\n\r\n";
    return strlen(pattern) + std::search(buffer, buffer + strlen(buffer),
                                         std::begin(pattern),
                                         std::begin(pattern) + strlen(pattern));
}

void send_request(const std::string& request,
                  const File file,
                  const sockaddr_in& serv_addr) {
    const auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    passert(sockfd >= 0, "can't open socket");
    passert(connect(sockfd, reinterpret_cast<const sockaddr*>(&serv_addr),
                    sizeof(serv_addr)) >= 0,
            "can't connect");
    passert(write(sockfd, request.c_str(), request.size()) ==
                static_cast<ssize_t>(request.size()),
            "can't write");
    File buffer;
    ssize_t sz = 0;
    ssize_t len;
    while ((len = read(sockfd, buffer + sz, FILESZ - sz)) > 0) {
        sz += len;
    }
    close(sockfd);
    buffer[sz] = 0;
    const auto body = find_response_body(buffer);
    passert(strncmp(file, body, FILESZ) == 0, "files are not equal", false);
}

sockaddr_in make_serv_addr(const char* hostname, int portno) {
    sockaddr_in serv_addr;
    hostent* server = gethostbyname(hostname);
    if (!server) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    return serv_addr;
}

void readfile(const char* fname, File& file) {
    FILE* fp = fopen(fname, "r");
    passert(fp, "can't open file");
    size_t sz = 0;
    size_t len;
    while (sz + 1 < FILESZ && (len = fread(file + sz, 1, FILESZ - sz, fp))) {
        passert(!ferror(fp), "can't read file");
        sz += len;
    }
    fclose(fp);
    file[sz] = 0;
}

template <typename Int, typename... Ts>
void test(Int sessions, const Ts&... args) {
    for (int i = 0; i < sessions; ++i) {
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
    if (argc < 4) {
        fprintf(stderr, "usage: %s hostname port file sessions threads\n",
                argv[0]);
        return 1;
    }
    const auto request =
        "GET / HTTP/1.0\r\nHost: " + std::string(argv[1]) + "\r\n\r\n";
    const auto serv_addr = make_serv_addr(argv[1], atoi(argv[2]));
    const auto sessions = atoi(argv[4]);
    const auto thread_count = atoi(argv[5]);
    File file;
    readfile(argv[3], file);
    Stopwatch sw("DDoS");
    auto threads =
        make_threads(thread_count, sessions, request, file, serv_addr);
    for (auto& th : threads) {
        th.join();
    }
    return 0;
}
