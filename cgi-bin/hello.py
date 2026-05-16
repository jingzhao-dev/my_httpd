#!/usr/bin/env python3

# 这是 CGI 脚本必须输出的第一行：HTTP 头部
print("Content-Type: text/plain")
print()  # 空行，分隔头部和正文

# 下面是正文
print("Hello from CGI!")
print("This content was generated dynamically by a Python script.")
