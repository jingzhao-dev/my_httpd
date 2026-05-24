#ifndef SERVE_FILE_H
#define SERVE_FILE_H
#include <stddef.h>
#include "dynbuf.h"
 

/**
根据URL路径，读取并返回静态文件内容。

参数：
    path：              请求路径
    response_buf：      输出缓冲区，存放构造好的完整HTTP响应
    buf_size:           输出缓冲区大小

返回值：
    0：成功读取文件，响应已放入response_buf
    -1:文件不存在（404）
    -2：路径含非法字符（403 Forbidden）
    -3：其他错误（文件读取失败等）
*/

//旧函数，暂时保留
int serve_static_file(const char* path,char* response_buf,size_t buf_size);




//新函数
DynBuf *serve_static_file_buf(const char *path);
#endif