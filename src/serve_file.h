#ifndef SERVE_FILE_H
#define SERVE_FILE_H
#include <stddef.h>
#include "dynbuf.h"
 

/**
根据URL路径，读取并返回静态文件内容。

参数：
    path  路径

*/


DynBuf *serve_static_file_buf(const char *path);
#endif