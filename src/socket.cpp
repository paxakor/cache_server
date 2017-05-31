// Copyright 2016-2017, Pavel Korozevtsev.

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include <utility>

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

FileDescriptor::FileDescriptor(int h)
    : handler{h} {
}

FileDescriptor::FileDescriptor(FileDescriptor&& other)
    : handler{std::move(other.handler)} {
    other.handler = -1;
}

FileDescriptor::~FileDescriptor() {
    if (valid()) {
        close(handler);
    }
    handler = -1;
}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) {
    this->~FileDescriptor();
    new (this) FileDescriptor(std::move(other));
    return *this;
}

bool FileDescriptor::valid() const {
    return handler != -1;
}

FileDescriptor::handler_type FileDescriptor::get_system_handler() const {
    return handler;
}

FileDescriptor::handler_type FileDescriptor::release() {
    return exchange(handler, -1);
}

int FileDescriptor::make_nonblock() {
    return set_nonblock(get_system_handler());
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

Socket make_server_socket(Port port) {
    Socket serv(socket(PF_INET, SOCK_STREAM, 0));
    sockaddr_in address;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    auto try_bind = [&address, &serv] {
        return bind(serv.get_system_handler(),
                    reinterpret_cast<const sockaddr*>(&address),
                    sizeof(address)) == 0;
    };
    enum { tries = 8 };
    for (int i = 0; i < tries && !try_bind(); ++i) {
        log.message("Unable to create server socket on port " +
                    std::to_string(port) +
                    " (bind failed): " + Logger::errstr());
        address.sin_port = htons(++port);
    }
    if (listen(serv.get_system_handler(), SOMAXCONN) < 0) {
        log.fatal_error("Unable to create server socket (listen failed): " +
                        Logger::errstr());
    } else {
        log.print("Server socket was bound to port " + std::to_string(port));
    }
    return serv;
}

Socket accept_client(Socket& serv_sock_fd) {
    sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    Socket client(accept(serv_sock_fd.get_system_handler(),
                         reinterpret_cast<sockaddr*>(&client_address),
                         &client_address_size));
    if (client.valid()) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_address.sin_addr, ip, sizeof(ip));
        log.access(ip, ntohs(client_address.sin_port));
    }
    return client;
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
             "Content-Length: %zu\r\n"
             "\r\n",
             status, file_size);
    return write(cls, {headers, strlen(headers)});
}

}  // namespace pkr
