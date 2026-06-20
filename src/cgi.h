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
 * 
 */


DynBuf *handle_cgi_buf(const char *script_path, const char *method);
#endif
