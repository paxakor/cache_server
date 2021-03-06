// Copyright 2016-2017, Pavel Korozevtsev.

#include <unistd.h>

#include <iterator>
#include <utility>

#include "include/epoll.hpp"
#include "include/log.hpp"

namespace pkr {

Socket make_event_socket(epoll_event ee) {
    return Socket{ee.data.fd};
}

Epoll::Epoll(Socket& serv)
    : epoll_fd(epoll_create1(0))
    , server_fd(serv)
    , valid_count(0) {
    add(server_fd, EPOLLIN);
}

Epoll::~Epoll() {
    close(epoll_fd);
}

int Epoll::wait() {
    do {
        valid_count = epoll_wait(epoll_fd, events.data(), max_events, -1);
    } while (valid_count < 0 && errno == EINTR);
    if (valid_count < 0) {
        log.error("epoll_wait failed: " + Logger::errstr());
    }
    return valid_count;
}

void Epoll::accept_all() {
    Socket sock;
    while ((sock = accept_client(server_fd)).valid()) {
        add(sock);
        sock.release();
    }
}

void Epoll::add(Socket& sock, uint32_t flags) {
    sock.make_nonblock();
    const auto fd = sock.get_system_handler();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = flags;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
        log.message("epoll_ctl (add) failed: " + Logger::errstr());
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
    return {ei, *this};
}


Epoll::iterator::iterator(event_iter pos, Epoll& par)
    : iter(std::move(pos))
    , parent(par) {
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
    while (iter != parent.events_end() &&
           iter->data.fd == parent.server_fd.get_system_handler()) {
        ++iter;
    }
}

}  // namespace pkr
