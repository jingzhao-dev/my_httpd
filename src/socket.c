#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket.h"

void die(const char *msg) {
    perror(msg);
    exit(1);
}

int create_server_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);//创建客户端socket
    if (sockfd < 0) die("socket");

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        die("setsockopt");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)//绑定bind
        die("bind");

    if (listen(sockfd, 10) < 0)//监听listen
        die("listen");

    printf("Server listening on port %d\n", port);
    return sockfd;
}