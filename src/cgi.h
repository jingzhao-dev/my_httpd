#ifndef CGI_H
#define CGI_H

#include<stddef.h>
#include "dynbuf.h"

/**
 * 处理 CGI 请求。启动外部程序，捕获其输出，构造 HTTP 响应。
 * 
 * 参数：
 *   script_path:  CGI 脚本的路径（如 "cgi-bin/hello.py"）
 *   method:       请求方法（"GET" 或 "POST"）
 *   response_buf: 输出缓冲区，存放构造好的 HTTP 响应
 *   buf_size:     输出缓冲区大小
 * 
 * 返回值：
 *   0: 成功
 *  -1: 脚本执行失败
 */


//旧函数
int handle_cgi(const char *script_path,const char* method,char* response,size_t buf_size);


// 新函数
DynBuf *handle_cgi_buf(const char *script_path, const char *method);
#endif
