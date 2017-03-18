// Copyright 2017, Pavel Korozevtsev.

#pragma once

#include <sys/epoll.h>
#include <array>
#include <iterator>
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
        using difference_type   = void;
        using iterator_category = std::input_iterator_tag;
        using pointer           = events_storage::pointer;
        using reference         = events_storage::value_type&;
        using value_type        = events_storage::value_type;
        using event_iter        = events_storage::iterator;

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

    void accept_all();
    int wait();
    iterator begin();
    iterator end();

private:
    Epoll(int);
    void add(int);
    void add(int, uint32_t);
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
