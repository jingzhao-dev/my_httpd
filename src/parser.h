#ifndef PARSER_H
#define PARSER_H
/**
 * 解析HTTP请求行，提取方法、路径、协议版本。
 * 
 * 参数：
 *   raw_request: 原始的HTTP请求报文（会被修改）
 *   method:      输出参数，指向请求方法（如 "GET"）的指针，调用者不需释放
 *   path:        输出参数，指向请求路径（如 "/index.html"）的指针，调用者不需释放
 *   version:     输出参数，指向协议版本（如 "HTTP/1.1"）的指针，调用者不需释放
 * 
 * 返回值：
 *   0: 成功
 *  -1: 格式错误（请求行为空或字段不完整）
 */
int parse_request_line(char *raw_request, char **method, char **path, char **version);
 #endif