// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstring>
#include <algorithm>
#include <iostream>
#include <iterator>
#include "include/epoll.hpp"
#include "include/log.hpp"

namespace pkr {

Epoll make_epoll(DescriptorRef serv) {
    return {serv.handler};
}

DescriptorHolder make_event_socket(epoll_event ee) {
    return {ee.data.fd};
}

Epoll::Epoll(int serv_fd)
    : epoll_fd(epoll_create1(0))
    , server_fd(serv_fd)
    , valid_count(0) {
    struct epoll_event serv_event;
    serv_event.data.fd = server_fd;
    serv_event.events = EPOLLIN;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &serv_event) < 0) {
        log.message(Logger::errstr());
    }
}

void Epoll::wait() {
    do {
        valid_count = epoll_wait(epoll_fd, events.data(), max_events, -1);
    } while (valid_count < 0 && errno == EINTR);
    if (valid_count < 0) {
        log.error("epoll_wait failed: " + Logger::errstr());
    }
}

void Epoll::accept_all() {
    auto comp = [this](const epoll_event& ev) {
        return ev.data.fd == server_fd;
    };
    for (auto pos = events_begin();
        (pos = std::find_if(pos, events_end(), comp)) != events_end(); ++pos) {
        const auto fd = accept_client_helper(server_fd);
        if (fd >= 0) {
            add(fd);
        }
    }
}

void Epoll::add(int fd) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    set_nonblock(fd);
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
        log.message("epoll_ctl failed: " + Logger::errstr());
    }
}

Epoll::iterator Epoll::begin() {
    return make_iterator(events_begin());
}

Epoll::iterator Epoll::end() {
    return make_iterator(events_end());
}

Epoll::iterator::event_iter Epoll::events_begin() {
    return std::begin(events);
}

Epoll::iterator::event_iter Epoll::events_end() {
    auto pos = events_begin();
    std::advance(pos, valid_count);
    return pos;
}

Epoll::iterator Epoll::make_iterator(Epoll::iterator::event_iter ei) {
    return {ei, events_end(), server_fd};
}


Epoll::iterator::iterator(const event_iter& p, const event_iter& e, int sfd)
    : iter(p), end(e), server_fd(sfd) {
    skip_serv_fd();
}

bool Epoll::iterator::operator!=(Epoll::iterator other) {
    return iter != other.iter;
}

Epoll::iterator Epoll::iterator::operator++() {
    ++iter;
    skip_serv_fd();
    return *this;
}

Socket Epoll::iterator::operator*() {
    return make_event_socket(*iter);
}

void Epoll::iterator::skip_serv_fd() {
    while (iter != end && iter->data.fd == server_fd) {
        ++iter;
    }
}

}  // namespace pkr
