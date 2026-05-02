#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>     // sleep
#include<signal.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include "socket.h"
#include<arpa/inet.h>
#include <errno.h>



int main() {
    signal(SIGPIPE, SIG_IGN);

    int server_fd = create_server_socket(8080);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create server socket\n");
        return 1;
    }

    char buffer[4096];

    // 把响应字符串定义在这里，避免每次循环重新构造
    const char *response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, World!";

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        printf("Waiting for a client to connect...\n");

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("--- New client connected from %s:%d ---\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("========== HTTP Request (raw) ==========\n");
    printf("%s", buffer);
    printf("========================================\n");

    // 结构化日志输出（依赖 \0）
    char log_buffer[4096];
    strncpy(log_buffer, buffer, sizeof(log_buffer) - 1);
    log_buffer[sizeof(log_buffer) - 1] = '\0';

    char *first_line = strtok(log_buffer, "\r\n");
    if (first_line) {
        printf("[INFO] %s - \"%s\"\n",
               inet_ntoa(client_addr.sin_addr),
               first_line);
    }
            // ===== 新增：发送HTTP响应 =====
            ssize_t sent_bytes = send(client_fd, response, strlen(response), 0);
            if (sent_bytes < 0) {
                perror("send");
            } else if (sent_bytes < (ssize_t)strlen(response)) {
                // send可能只发送了一部分（少见，但理论存在）
                printf("Warning: Only sent %zd of %zu bytes\n", sent_bytes, strlen(response));
            } else {
                printf("Response sent successfully (%zd bytes)\n", sent_bytes);
            }
            // ===== 新增结束 =====

        } else if (n == 0) {
            printf("Client closed the connection without sending data.\n");
        } else {
            if(errno==ECONNRESET){
                printf("Client forcefully closed the conection.\n");
            }else{
                perror("recv");
            }
        }

        close(client_fd);
        printf("--- Connection closed, waiting for next client ---\n\n");
    }

    close(server_fd);
    return 0;
}