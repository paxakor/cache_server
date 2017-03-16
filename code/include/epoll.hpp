// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <sys/epoll.h>
#include <array>
#include "include/socket.hpp"

namespace pkr {

class Epoll;
Epoll make_epoll(DescriptorRef);
DescriptorHolder make_event_socket(epoll_event ee);

class Epoll {
    friend Epoll make_epoll(DescriptorRef);
    enum : size_t { max_events = 32 };
    using events_storage = std::array<epoll_event, max_events>;
public:

    class iterator {
    public:
        using event_iter = events_storage::iterator;
        using pointer    = events_storage::pointer;
        using value_type = events_storage::value_type;

        iterator(const event_iter&, const event_iter&, int);
        bool operator!=(iterator);
        iterator operator++();
        Socket operator*();
    private:
        void skip_serv_fd();
    private:
        event_iter iter;
        const event_iter end;
        const int server_fd;
    };

    void accept_all();
    void wait();
    iterator begin();
    iterator end();

private:
    Epoll(int);
    void add(int);
    iterator::event_iter events_begin();
    iterator::event_iter events_end();
    iterator make_iterator(iterator::event_iter);

private:
    events_storage events;
    const int epoll_fd;
    const int server_fd;
    int valid_count;
};

}  // namespace pkr
