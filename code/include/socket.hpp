// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>
#include <string>
#include "include/defs.hpp"
#include "include/string_view.hpp"

namespace pkr {

class DescriptorRef;
class DescriptorHolder;
class FileDescriptor;

class DescriptorRef {
    friend DescriptorHolder accept_client(DescriptorRef);
public:
    DescriptorRef(const FileDescriptor&);
protected:
    int handler;
};

class DescriptorHolder {
    friend class FileDescriptor;
    friend DescriptorHolder accept_client(DescriptorRef);
    friend DescriptorHolder make_server_socket(Port);
public:
    DescriptorHolder(DescriptorHolder&&);
protected:
    DescriptorHolder(int);
protected:
    int handler;
};

class FileDescriptor {
    friend class DescriptorRef;
public:
    enum : size_t { max_request_size = 65536 };
public:
    virtual ~FileDescriptor();
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor(FileDescriptor&&);
    FileDescriptor(DescriptorHolder&&);
    ssize_t read(void*, size_t);
    ssize_t write(const void*, size_t);
protected:
    FileDescriptor(int);
protected:
    int handler;
};

using Socket = FileDescriptor;

std::string read(FileDescriptor&, size_t = FileDescriptor::max_request_size);
ssize_t write(FileDescriptor&, string_view);

DescriptorHolder accept_client(DescriptorRef);
DescriptorHolder make_server_socket(Port);

ssize_t write_failure(Socket&, int, string_view);
ssize_t write_response(Socket&, int, size_t);

}  // namespace pkr
