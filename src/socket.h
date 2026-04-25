// src/socket.h
#ifndef SOCKET_H
#define SOCKET_H

// 设计意图：函数成功返回套接字fd，失败则通过die直接终止程序
// 这样上层main.c代码会非常干净，无需反复if判断
int create_server_socket(int port);

#endif
