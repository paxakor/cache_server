// Copyright 2017, Pavel Korozevtsev.

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "include/log.hpp"
#include "include/net_utils.hpp"
#include "include/socket.hpp"

namespace pkr {

std::shared_ptr<addrinfo> make_serv_addr(string_view host,
                                         string_view service) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    struct addrinfo* result = nullptr;
    auto err_code = getaddrinfo(host.data(), service.data(), &hints, &result);
    if (err_code != 0) {
        char err_msg[256];
        snprintf(err_msg, sizeof(err_msg), "Unable to find addrinfo for %s: %s",
                 host.data(), gai_strerror(err_code));
        log.error(err_msg);
        return nullptr;
    }
    return {result, freeaddrinfo};
}

Socket connect_to_server(std::shared_ptr<addrinfo> s_addr) {
    for (auto rp = s_addr.get(); rp; rp = rp->ai_next) {
        auto sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue;
        }
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            return Socket{sockfd};
        }
        close(sockfd);
    }
    if (s_addr) {
        log.error("Unable to connect to " + std::string(s_addr->ai_canonname));
    } else {
        log.error("Unable to connect to (nullptr-addrinfo)");
    }
    return Socket{-1};
}

}  // namespace pkr
