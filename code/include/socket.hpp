// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>
#include <sys/epoll.h>
#include <string>
#include "include/defs.hpp"
#include "include/string_view.hpp"

namespace pkr {

struct Handler {
    Handler(int h) : handler(h) {}
protected:
    int handler;
};

class DescriptorRef;
class DescriptorHolder;
class FileDescriptor;
class Epoll;

class DescriptorRef : public Handler {
    friend DescriptorHolder accept_client(DescriptorRef);
    friend Epoll make_epoll(DescriptorRef);
public:
    DescriptorRef(const FileDescriptor&);
};

class DescriptorHolder : public Handler {
    friend class FileDescriptor;
    friend DescriptorHolder accept_client(DescriptorRef);
    friend DescriptorHolder make_server_socket(Port);
    friend DescriptorHolder make_event_socket(epoll_event);
public:
    DescriptorHolder(DescriptorHolder&&);
protected:
    DescriptorHolder(int);
};

class FileDescriptor : public Handler {
    friend class DescriptorRef;
public:
    enum : size_t { max_request_size = 65536 };
public:
    virtual ~FileDescriptor();
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&);
    FileDescriptor(DescriptorHolder&&);
    ssize_t read(char*, ssize_t);
    ssize_t write(const char*, ssize_t);
protected:
    FileDescriptor(int);
};

using Socket = FileDescriptor;

std::string read(FileDescriptor&, size_t = FileDescriptor::max_request_size);
ssize_t write(FileDescriptor&, string_view);

DescriptorHolder accept_client(DescriptorRef);
DescriptorHolder make_server_socket(Port);
int accept_client_helper(int);
int set_nonblock(int);

ssize_t write_failure(Socket&, int, string_view);
ssize_t write_response(Socket&, int, size_t);

}  // namespace pkr
