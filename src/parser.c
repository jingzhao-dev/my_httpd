#include "parser.h"
#include <string.h>
#include <stdio.h>

int parse_request_line(char *raw_request, char **method, char **path, char **version) {
    if (!raw_request) return -1;

    // 找到第一个 CRLF，截断为独立请求行
    char *line_end = strstr(raw_request, "\r\n");
    if (!line_end) {
        fprintf(stderr, "Parse error: no CRLF found\n");
        return -1;
    }
    *line_end = '\0';  // 现在 raw_request 只包含第一行，以 \0 结尾

    // 手动查找第一个空格，分割 method
    char *first_space = strchr(raw_request, ' ');
    if (!first_space) {
        fprintf(stderr, "Parse error: incomplete request line\n");
        return -1;
    }
    *first_space = '\0';
    *method = raw_request;           // method 指向开头

    // 跳过可能存在的多个空格，找到 path 的开始
    char *path_start = first_space + 1;
    while (*path_start == ' ') path_start++;
    if (*path_start == '\0') {
        fprintf(stderr, "Parse error: incomplete request line\n");
        return -1;
    }

    // 查找下一个空格，分割 path 和 version
    char *second_space = strchr(path_start, ' ');
    if (!second_space) {
        fprintf(stderr, "Parse error: incomplete request line\n");
        return -1;
    }
    *second_space = '\0';
    *path = path_start;              // path 指向路径

    // 跳过空格，找到 version 的开始
    char *version_start = second_space + 1;
    while (*version_start == ' ') version_start++;
    if (*version_start == '\0') {
        fprintf(stderr, "Parse error: incomplete request line\n");
        return -1;
    }
    *version = version_start;        // version 指向协议版本

    return 0;
}