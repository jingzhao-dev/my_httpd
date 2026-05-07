#include"serve_file.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int serve_static_file(const char* path,char *response_buf,size_t buf_size){
//======安全检测：防止路径穿越======
if(strstr(path,"..")!=NULL){
    snprintf(response_buf,buf_size,
    "HTTP/1.1 403 Forbidden\r\n"
    "Content-Type:text/plain\r\n"
    "Content-Length:15\r\n"
    "\r\n"
    "403 Forbidden");
    return -2;
}

//========确定实际文件路径========
const char* file_path;
    if(strcmp(path,"/")==0){
        file_path="www/index.html";
    }else{
        static char full_path[512];
        snprintf(full_path,sizeof(full_path),"www%s",path);
        file_path=full_path;
    }



//========打开文件==============
FILE *f=fopen(file_path,"r");
if(!f){
    const char *body="404 Page Not Found";
    snprintf(response_buf,buf_size,
     "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                strlen(body),body);
                return -1;
}



//=========获取文件大小===========
fseek(f,0,SEEK_END);
long file_size=ftell(f);
fseek(f,0,SEEK_SET);


//========分配内存并读取文件内容=====
char *file_content=(char*)malloc(file_size+1);
if(!file_content){
    fclose(f);
    const char *body="500 Internal Server Error";
    snprintf(response_buf,buf_size,
        "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                strlen(body),body);
                return -3;
}

size_t bytes_read=fread(file_content,1,file_size,f);
fclose(f);

if(bytes_read<(size_t)file_size){
    free(file_content);
    const char *body= "500 Internal Server Error";
    snprintf(response_buf,buf_size,
    "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                strlen(body),body);
                return -3;
}

file_content[bytes_read]='\0';

//===========构造HTTP响应=========
snprintf(response_buf,buf_size,
    "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",bytes_read,file_content);

             free(file_content);
             return 0;
}