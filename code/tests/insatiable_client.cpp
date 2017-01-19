// Copyright 2016-2017, Pavel Korozevtsev.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "stopwatch.hpp"

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char** argv) {
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    if (argc < 3) {
       fprintf(stderr, "usage: %s hostname port count\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    n = atoi(argv[3]);
    server = gethostbyname(argv[1]);
    if (!server) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    Stopwatch sw("DUDOS");
    for (int i = 0; i < n; ++i) {
        static const char* request = "GET / HTTP/1.1\r\n\r\n";
        char buffer[256] = {0};
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            error("ERROR opening socket");
        }
        if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&serv_addr),
            sizeof(serv_addr)) < 0) {
            error("ERROR connecting");
        }
        if (write(sockfd, request, strlen(request)) < 0) {
            error("ERROR writing to socket");
        }
        if (read(sockfd, buffer, sizeof(buffer) - 1) < 0) {
            error("ERROR reading from socket");
        }
        close(sockfd);
    }
    return 0;
}
