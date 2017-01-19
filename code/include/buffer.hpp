// Copyright 2016-2017, Pavel Korozevtsev.

#pragma once

#include <cstddef>
#include <cstring>

namespace pkr {

class Socket;

template <size_t N>
class Buffer {
public:
    friend class Socket;
    // Buffer() = default;
    // Buffer(const Buffer&) = default;
    // Buffer& operator=(const Buffer&) = default;
    bool contain(size_t) const;
    bool empty() const;
    bool has_space(size_t) const;
    const char* begin() const;
    const char* end() const;
    const char* data() const;
    size_t capacity() const;
    size_t size() const;
    ssize_t read(char*, size_t);
    ssize_t read_all(char*);
    ssize_t write(const char*, size_t);
    void drop();

protected:
    char buf[N];
    size_t len = 0;
};

template <size_t N>
bool Buffer<N>::contain(size_t n) const {
    return size() >= n;
}

template <size_t N>
bool Buffer<N>::empty() const {
    return size() == 0;
}

template <size_t N>
bool Buffer<N>::has_space(size_t n) const {
    return n + len <= capacity();
}

template <size_t N>
const char* Buffer<N>::begin() const {
    return buf;
}

template <size_t N>
const char* Buffer<N>::end() const {
    return buf + len;
}

template <size_t N>
const char* Buffer<N>::data() const {
    return buf;
}

template <size_t N>
size_t Buffer<N>::capacity() const {
    return N;
}

template <size_t N>
size_t Buffer<N>::size() const {
    return len;
}

template <size_t N>
ssize_t Buffer<N>::read(char* dest, size_t n) {
    if (!contain(n)) {
        return -1;
    }
    memcpy(dest, buf, n);
    len -= n;
    for (size_t i = 0; i < len; ++i) {
        buf[i] = buf[i + n];
    }
    return n;
}

template <size_t N>
ssize_t Buffer<N>::read_all(char* dest) {
    return read(dest, size());
}

template <size_t N>
ssize_t Buffer<N>::write(const char* src, size_t n) {
    if (!has_space(n)) {
        return -1;
    }
    memcpy(buf + len, src, n);
    len += n;
}

template <size_t N>
void Buffer<N>::drop() {
    len = 0;
}

}  // namespace pkr
