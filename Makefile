CC = gcc
CFLAGS = -Wall -Wextra -g -O0
TARGET = my_httpd
SRCS = src/main.c src/socket.c src/parser.c src/serve_file.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

.PHONY: all clean
