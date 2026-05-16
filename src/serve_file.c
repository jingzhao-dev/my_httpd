#include"serve_file.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <strings.h> 

//MIME映射表
typedef struct{
    const char* extension;
    const char* mime_type;
}mime_map_t;

static const mime_map_t mime_map[]={
    {".html", "text/html"},
    {".htm",  "text/html"},
    {".css",  "text/css"},
    {".js",   "application/javascript"},
    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif",  "image/gif"},
    {".svg",  "image/svg+xml"},
    {".ico",  "image/x-icon"},
    {".txt",  "text/plain"},
    {".json", "application/json"},
    {NULL,    "application/octet-stream"}  // 默认兜底
};

/**
 * 根据文件名（路径）返回对应的MIME类型
 * 找不到匹配的扩展名时，返回 application/octet-stream
 */

 static const char* get_mime_type(const char* path){
    //找到最后一个'.'的位置
    const char* dot=strrchr(path,'.');
    if(!dot){
        //没有拓展名，返回默认类型
        return "application/octet-stream";
    }

    //遍历映射表
    for(int i=0;mime_map[i].extension!=NULL;i++){
        if(strcasecmp(dot,mime_map[i].extension)==0){//strcasecmp(dot, ...)：忽略大小写的字符串比较
            return mime_map[i].mime_type;
        }
    }
    //没找到匹配的，返回兜底类型
    return mime_map[sizeof(mime_map)/sizeof(mime_map[0])-1].mime_type;
     }


     
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
const char* mime=get_mime_type(file_path);
snprintf(response_buf,buf_size,
    "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "\r\n"
             "%s",mime,bytes_read,file_content);

             free(file_content);
             return 0;
}