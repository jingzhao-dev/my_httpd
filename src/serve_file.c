#include"serve_file.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<strings.h> 
#include"dynbuf.h"
#include"hashmap.h"

static HashMap *mime_map=NULL;

/**
 * 根据文件名（路径）返回对应的MIME类型
 * 找不到匹配的扩展名时，返回 application/octet-stream
 */

 static const char* get_mime_type(const char* path){
    if(!mime_map){
        mime_map=hashmap_create_with_capacity(31);
        hashmap_put(mime_map,".html", "text/html");
        hashmap_put(mime_map,".htm",  "text/html");
        hashmap_put(mime_map,".css",  "text/css");
        hashmap_put(mime_map,".js",   "application/javascript");
        hashmap_put(mime_map,".png",  "image/png");
        hashmap_put(mime_map,".jpg",  "image/jpeg");
        hashmap_put(mime_map,".jpeg", "image/jpeg");
        hashmap_put(mime_map,".gif",  "image/gif");
        hashmap_put(mime_map,".svg",  "image/svg+xml");
        hashmap_put(mime_map,".ico",  "image/x-icon");
        hashmap_put(mime_map,".txt",  "text/plain");
        hashmap_put(mime_map,".json", "application/json");
    }
    //找到最后一个'.'的位置
    const char* dot=strrchr(path,'.');
    if(!dot){
        //没有拓展名，返回默认类型
        return "application/octet-stream";
    }
    const char *mime_type=hashmap_get(mime_map,dot);
    if(mime_type){
        return mime_type;
    }else{
        return "application/octet-stream";
    }
}



//================辅助函数，错误响应构造==============
static int append_error_response(DynBuf *response_buf,const char *status_line,const char *body){
    
    if(dbuf_append(response_buf,status_line,strlen(status_line))==-1) return -1;
    if(dbuf_append(response_buf,"Content-Type: text/plain\r\n",strlen("Content-Type: text/plain\r\n"))==-1) return -1;
    char length_buf[64];
    int content_length=snprintf(length_buf,sizeof(length_buf),"Content-Length: %zu\r\n",strlen(body));
    if(dbuf_append(response_buf,length_buf,content_length)==-1) return -1;
    if(dbuf_append(response_buf,"\r\n",2)==-1) return -1;
    if(dbuf_append(response_buf,body,strlen(body))==-1) return -1;

    return 0;
}


//================新函数==============
DynBuf *serve_static_file_buf(const char *path){
    DynBuf *response_buf=dbuf_create();
    if(!response_buf) return NULL;
    //======安全检测：防止路径穿越======
    if(strstr(path,"..")!=NULL){
        if(append_error_response(response_buf,"HTTP/1.1 403 Forbidden\r\n","403 Forbidden")==-1){
            dbuf_free(response_buf);
            return NULL;
        } 
        return response_buf;
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
    if(append_error_response(response_buf,"HTTP/1.1 404 Not Found\r\n","404 Page Not Found")==-1){
        dbuf_free(response_buf);
        return NULL;
    }
     return response_buf;
  }


//=========获取文件大小===========
fseek(f,0,SEEK_END);
long file_size=ftell(f);
fseek(f,0,SEEK_SET);


//========分配内存并读取文件内容=====
char *file_content=NULL;//防止在mollac之前跳到fail,导致file_content未初始化
file_content=(char*)malloc(file_size+1);
if(!file_content){
    fclose(f);
    if(append_error_response(response_buf,"HTTP/1.1 500 Internal Server Error\r\n","500 Internal Server Error")==-1){
        dbuf_free(response_buf);
        return NULL;
    }

        return response_buf;
}

size_t bytes_read=fread(file_content,1,file_size,f);
fclose(f);

if(bytes_read<(size_t)file_size){
    free(file_content);
    if(append_error_response(response_buf,"HTTP/1.1 500 Internal Server Error\r\n","500 Internal Server Error")==-1){
        dbuf_free(response_buf);
        return NULL;
    }

     return response_buf;
}

file_content[bytes_read]='\0';


//===========构造HTTP响应=========
const char* mime=get_mime_type(file_path);
if(dbuf_append(response_buf,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n"))==-1) goto fail;//状态行

char content_type[64];
int content_type_length=snprintf(content_type,sizeof(content_type),"Content-Type: %s\r\n",mime);//类型行
     if(dbuf_append(response_buf,content_type,content_type_length)==-1) goto fail;


char content_length[64];
int content_length_len=snprintf(content_length,sizeof(content_length),"Content-Length: %zu\r\n",bytes_read);//长度

     if(dbuf_append(response_buf,content_length,content_length_len)==-1) goto fail;
     if(dbuf_append(response_buf,"\r\n",2)==-1) goto fail;
     if(dbuf_append(response_buf,file_content,bytes_read)==-1) goto fail;

     free(file_content);

    return response_buf;


fail:
free(file_content);
dbuf_free(response_buf);
return NULL;

}

