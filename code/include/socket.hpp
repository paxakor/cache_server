// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <sys/epoll.h>

#include <cstddef>

#include <string>

#include "include/defs.hpp"
#include "include/string_view.hpp"

namespace pkr {

class FileDescriptor {
public:
    using handler_type = int;
    enum : size_t { max_request_size = 65536 };

public:
    explicit FileDescriptor(int = -1);
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&);
    ~FileDescriptor();
    FileDescriptor& operator=(FileDescriptor&&);

    bool valid() const;
    handler_type get_system_handler() const;
    handler_type release();
    int make_nonblock();

    ssize_t read(char*, ssize_t);
    ssize_t write(const char*, ssize_t);

protected:
    handler_type handler;
};

using Socket = FileDescriptor;

FileDescriptor accept_client(FileDescriptor&);
FileDescriptor make_server_socket(Port port);

ssize_t write(FileDescriptor&, string_view);
ssize_t write_failure(Socket&, int, string_view);
ssize_t write_response(Socket&, int, size_t);
std::string read(FileDescriptor&, size_t = FileDescriptor::max_request_size);

}  // namespace pkr
