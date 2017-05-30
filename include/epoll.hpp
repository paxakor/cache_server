// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <sys/epoll.h>

#include <array>
#include <iterator>

#include "include/socket.hpp"

namespace pkr {

class Epoll {
    enum : size_t { max_events = 32 };
    using events_storage = std::array<epoll_event, max_events>;

public:
    class iterator {
    public:
        // clang-format off
        using difference_type   = void;
        using iterator_category = std::input_iterator_tag;
        using pointer           = events_storage::pointer;
        using reference         = events_storage::value_type&;
        using value_type        = events_storage::value_type;
        using event_iter        = events_storage::iterator;
        // clang-format on

        iterator(event_iter, Epoll&);
        bool operator!=(iterator);
        iterator operator++();
        Socket operator*();

    private:
        void skip_serv_fd();

    private:
        event_iter iter;
        Epoll& parent;
    };

public:
    Epoll(Socket&);
    ~Epoll();
    void accept_all();
    int wait();
    iterator begin();
    iterator end();

private:
    void add(Socket&, uint32_t = EPOLLIN | EPOLLONESHOT);
    iterator::event_iter events_begin();
    iterator::event_iter events_end();
    iterator make_iterator(iterator::event_iter);

private:
    events_storage events;
    const int epoll_fd;
    Socket& server_fd;
    int valid_count;
};

Socket make_event_socket(epoll_event);

}  // namespace pkr
