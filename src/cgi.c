#include"cgi.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include "dynbuf.h"

//===============新函数==================
DynBuf *handle_cgi_buf(const char *script_path, const char *method){
    DynBuf *response_buf=dbuf_create();
    if(!response_buf)   return NULL;

    int pipefd[2];
    if(pipe(pipefd)==-1){
        if(dbuf_append(response_buf,"HTTP/1.1 500 Internal Server Error\r\n",strlen("HTTP/1.1 500 Internal Server Error\r\n"))==-1) goto fail;
        if(dbuf_append(response_buf,"Content-Type: text/plain\r\n",strlen("Content-Type: text/plain\r\n"))==-1)  goto fail;
        char content_length_buf[64];
        const char *body_buf="500 CGI Pipe Error";
        int content_length_len=snprintf(content_length_buf,sizeof(content_length_buf),"Content-Length: %zu\r\n",strlen(body_buf));
        if(dbuf_append(response_buf,content_length_buf,content_length_len)==-1)  goto fail;
        if(dbuf_append(response_buf,"\r\n",2)==-1) goto fail;
        if(dbuf_append(response_buf,body_buf,strlen(body_buf))==-1)  goto fail;
        return response_buf;
    }

    pid_t pid=fork();
    if(pid==-1){
        close(pipefd[0]);
        close(pipefd[1]);
        if(dbuf_append(response_buf,"HTTP/1.1 500 Internal Server Error\r\n",strlen("HTTP/1.1 500 Internal Server Error\r\n"))==-1) goto fail;
        if(dbuf_append(response_buf,"Content-Type: text/plain\r\n",strlen("Content-Type: text/plain\r\n"))==-1)  goto fail;
        char content_length_buf[64];
        const char *body_buf="500 CGI Fork Error";
        int content_length_len=snprintf(content_length_buf,sizeof(content_length_buf),"Content-Length: %zu\r\n",strlen(body_buf));
        if(dbuf_append(response_buf,content_length_buf,content_length_len)==-1)  goto fail;
        if(dbuf_append(response_buf,"\r\n",2)==-1) goto fail;
        if(dbuf_append(response_buf,body_buf,strlen(body_buf))==-1)  goto fail;
       
        return response_buf;
    }

    if(pid==0){
        //子进程：执行脚本
        close(pipefd[0]);//关闭读端，子进程只写
        dup2(pipefd[1],STDOUT_FILENO);//吧标准输出重定向到管道写端
        close(pipefd[1]);

        //设置CGI环境变量
        setenv("REQUEST_METHOD",method,1);
        setenv("SCRIPT_NAME",script_path,1);

        //执行脚本
        execl(script_path,script_path,NULL);

        //如果execl返回，说明执行失败
        printf("Content-Type: text/plain\r\n\r\n");
        printf("Error: Cannot execute CGI script\n");
        exit(1);    
    }else{
        //父进程：读取子进程的输出
        close(pipefd[1]);   //关闭写端，父进程只读


        DynBuf *cgi_body=dbuf_create();
        if(!cgi_body)  return NULL;
        char read_buf[4096];
        ssize_t n;
        while((n=read(pipefd[0],read_buf,sizeof(read_buf)))>0){
          if(dbuf_append(cgi_body,read_buf,n)==-1){
            dbuf_free(cgi_body);
            dbuf_free(response_buf);
            close(pipefd[0]);

            //等待子进程结束
            waitpid(pid,NULL,0);

            return NULL;
          }
        }
        close(pipefd[0]);

        //等待子进程结束
        waitpid(pid,NULL,0);

        //构造HTTP响应

        if(dbuf_append(response_buf,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n"))==-1) goto fail;
        if(dbuf_append(response_buf,"Content-Type: text/plain\r\n",strlen("Content-Type: text/plain\r\n"))==-1)  goto fail;
        char content_length_buf[64];
        int content_length_len=snprintf(content_length_buf,sizeof(content_length_buf),"Content-Length: %zu\r\n",dbuf_len(cgi_body));
        if(dbuf_append(response_buf,content_length_buf,content_length_len)==-1)  goto fail;
        if(dbuf_append(response_buf,"\r\n",2)==-1) goto fail;
        if(dbuf_append(response_buf,dbuf_data(cgi_body),dbuf_len(cgi_body))==-1)  goto fail;
       
        dbuf_free(cgi_body);
        return response_buf;
       

    fail:
    dbuf_free(cgi_body);
    dbuf_free(response_buf);

    return NULL;

    }
}
