// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "include/log.hpp"
#include "include/socket.hpp"
#include "include/utils.hpp"

namespace pkr {

int set_nonblock(int fd) {
    int flags = 1;
#ifdef O_NONBLOCK
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    return ioctl(fd, FIOBIO, &flags);
#endif
}

DescriptorRef::DescriptorRef(const FileDescriptor& fd)
    : Handler{fd.handler} {}

DescriptorHolder::DescriptorHolder(DescriptorHolder&& other)
    : Handler{std::move(other.handler)} {
    other.handler = -1;
}

DescriptorHolder::DescriptorHolder(int h)
    : Handler{h} {}

FileDescriptor::FileDescriptor(int h)
    : Handler{h} {
    if (handler < 0) {
        log.error("Invalid fildes");
    } else {
        set_nonblock(handler);
    }
}

FileDescriptor::FileDescriptor(FileDescriptor&& other)
    : Handler{std::move(other.handler)} {
    other.handler = -1;
}

FileDescriptor::FileDescriptor(DescriptorHolder&& other)
    : Handler{std::move(other.handler)} {
    other.handler = -1;
}

FileDescriptor::~FileDescriptor() {
    if (handler != -1) {
        close(handler);
    }
}

ssize_t FileDescriptor::read(char* dest, ssize_t nbyte) {
    ssize_t sz = 0;
    ssize_t len;
    do {
        len = ::read(handler, dest + sz, nbyte - sz);
        if (len > 0) {
            sz += len;
        }
    } while (sz < nbyte && len > 0);
    return sz;
}

ssize_t FileDescriptor::write(const char* buf, ssize_t nbyte) {
    ssize_t sz = 0;
    ssize_t len;
    do {
        len = ::write(handler, buf + sz, nbyte - sz);
        if (len > 0) {
            sz += len;
        }
    } while (sz < nbyte && len > 0);
    return sz;
}

std::string read(FileDescriptor& fd, size_t nbyte) {
    std::vector<char> buf(nbyte);
    const auto len = fd.read(buf.data(), nbyte);
    if (len <= 0) {
        log.message("Unable to read from socket");
    }
    return {buf.data(), buf.data() + len};
}

ssize_t write(FileDescriptor& fd, string_view buf) {
    return fd.write(buf.data(), buf.size());
}

DescriptorHolder make_server_socket(Port port) {
    DescriptorHolder serv(socket(AF_INET, SOCK_STREAM, 0));
    sockaddr_in address;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    auto try_bind = [&address, &serv] {
        return bind(serv.handler, reinterpret_cast<const sockaddr*>(&address),
                    sizeof(address)) == 0;
    };
    enum { tries = 8 };
    for (int i = 0; i < tries && !try_bind(); ++i) {
        log.message("Unable to create server socket on port " +
                    std::to_string(port) + " (bind failed): " +
                    Logger::errstr());
        address.sin_port = htons(++port);
    }
    if (listen(serv.handler, SOMAXCONN) < 0) {
        log.fatal_error("Unable to create server socket (listen failed): " +
                        Logger::errstr());
    } else {
        log.print("Server socket was bound to port " + std::to_string(port));
    }
    return serv;
}

int accept_client_helper(int serv_sock_fd) {
    sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    const auto new_client_handler =
        ::accept(serv_sock_fd, reinterpret_cast<sockaddr*>(&client_address),
                 &client_address_size);
    if (new_client_handler >= 0) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, ip, sizeof(ip));
        log.access(ip, ntohs(client_address.sin_port));
    }
    return new_client_handler;
}

DescriptorHolder accept_client(DescriptorRef serv_sock) {
    return {accept_client_helper(serv_sock.handler)};
}

ssize_t write_failure(Socket& cls, int status, string_view msg) {
    char headers[256];
    snprintf(headers, sizeof(headers), "HTTP/1.0 %d %s\r\n", status,
             msg.to_string().c_str());
    return write(cls, {headers, strlen(headers)});
}

ssize_t write_response(Socket& cls, int status, size_t file_size) {
    char headers[256];
    snprintf(headers, sizeof(headers),
             "HTTP/1.0 %d OK\r\n"
             "Content-Length: %lu\r\n"
             "\r\n",
             status, file_size);
    return write(cls, {headers, strlen(headers)});
}

}  // namespace pkr
