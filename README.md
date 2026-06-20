## 编译和运行

```bash
# 编译
make

# 运行
./my_httpd

# 测试
curl http://localhost:8080/
curl http://localhost:8080/cgi-bin/hello.py

# 内存检查
valgrind --leak-check=full ./my_httpd


## 自建数据结构
- `DynBuf`（动态字节缓冲区）
- `HashMap`（开放寻址法哈希表）
- `LinkedList`（开发中）


## 架构概览
### 请求处理流程

            socket.c (创建服务socket->bind->listen)
              ↓
客户端请求 → main.c (accept/recv)
              ↓
           main.c (分发)
          /        \
         /          \
   CGI 请求        静态文件请求
        ↓              ↓
   cgi.c           serve_file.c
   fork+exec        fopen+fread
        ↓              ↓
         \            /
          ↓          ↓
        clib/dynbuf.c (构造响应)
              ↓
           main.c (send)
              ↓
           close



### 模块职责

| 模块 | 文件 | 职责 |
|------|------|------|
| 网络层 | `src/socket.c` | 创建 socket、bind、listen、accept |
| 主控层 | `src/main.c` | accept 循环、请求分发、发送响应、日志 |
| 解析层 | `src/parser.c` | 解析 HTTP 请求行（方法、路径、版本） |
| 静态文件 | `src/serve_file.c` | 读取文件、MIME 映射、构造 200/404/403 响应 |
| 动态请求 | `src/cgi.c` | fork+exec 外部脚本、管道捕获输出、构造响应 |
| 基础库 | `~/clib/` | 通用数据结构：DynBuf、HashMap |

### 关键设计决策

- **统一用 `DynBuf *` 构造 HTTP 响应**：不再用 `char response[4096]` 固定数组。`serve_file.c` 和 `cgi.c` 都返回 `DynBuf *`，`main.c` 统一调用 `dbuf_data()` 和 `dbuf_len()` 获取数据后发送，发送后 `dbuf_free()` 释放。
- **MIME 类型查找用哈希表**：`serve_file.c` 中的 `get_mime_type` 不再遍历结构体数组，而是调用 `hashmap_get()`，在 O(1) 时间内完成。
- **自建数据结构独立于项目**：`DynBuf`、`HashMap` 放在 `~/clib/` 下，有自己的头文件和独立测试，不依赖任何 HTTP 逻辑。


## 开发里程碑
- v1.0：单线程静态文件服务器
- v1.1：CGI 支持
- v1.2：DynBuf 替换固定缓冲区
- v1.3：哈希表替换 MIME 线性查找

## 学习笔记
开发日志见 `devlog.md`，记录了每个阶段的关键问题和认知突破。