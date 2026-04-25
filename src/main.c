#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket.h"

int main() {
    signal(SIGPIPE, SIG_IGN);  // 防止向已关闭的socket写数据导致崩溃

    int server_fd = create_server_socket(8080);
    char buffer[4096];

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        // 打印客户端IP（你的计划第6天要求）
        printf("[INFO] %s - ", inet_ntoa(client_addr.sin_addr));

        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received request:\n%s\n", buffer);

            // 固定HTTP响应（你的计划第4天任务）
            char *response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n"
                             "\r\n"
                             "Hello, World!";
            send(client_fd, response, strlen(response), 0);
        }

        close(client_fd);
    }
    return 0;
}