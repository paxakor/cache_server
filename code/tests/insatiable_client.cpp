// Copyright 2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "stopwatch.hpp"

int main(int argc, char** argv) {
    enum { FILESZ = 4096 };
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    if (argc < 3) {
        fprintf(stderr, "usage: %s hostname port count\n", argv[0]);
        return 1;
    }
    portno = atoi(argv[2]);
    n = atoi(argv[3]);
    server = gethostbyname(argv[1]);
    if (!server) {
        fprintf(stderr, "ERROR, no such host\n");
        return 1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    Stopwatch sw("DUDOS");
    const std::string request = "GET / HTTP/1.0\r\nHost: " +
        std::string(argv[1]) + "\r\n\r\n";
    char file[FILESZ];
    for (int i = 0; i < n; ++i) {
        char buffer[FILESZ] = {0};
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            return 1;
        }
        if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&serv_addr),
            sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            return 1;
        }
        if (write(sockfd, request.c_str(), request.size()) < 0) {
            perror("ERROR writing to socket");
            return 1;
        }
        ssize_t sz = 0;
        ssize_t len;
        while ((len = read(sockfd, buffer + sz, sizeof(buffer) - sz)) > 0) {
            sz += len;
        }
        buffer[sz] = 0;
        close(sockfd);
        if (i == 0) {
            strncpy(file, buffer, FILESZ);
        } else {
            if (strncmp(file, buffer, FILESZ)) {
                printf("insatiable_client's error: files are not equal\n");
                return 1;
            }
        }
    }
    return 0;
}
