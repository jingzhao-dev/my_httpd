CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -I/home/zhaojing/clib/include
TARGET = my_httpd
SRCS = src/main.c src/socket.c src/parser.c src/serve_file.c src/cgi.c
OBJS = $(SRCS:.c=.o)

# 新增：dynbuf 的编译产物路径
DYNBUF_OBJ = /home/zhaojing/clib/src/dynbuf.o

all: $(TARGET)

# 依赖里加上 $(DYNBUF_OBJ)
$(TARGET): $(OBJS) $(DYNBUF_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

.PHONY: all clean
