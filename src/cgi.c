#include"cgi.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int handle_cgi(const char *script_path,const char *method,char *response_buf,size_t buf_size){
    int pipefd[2];
    if(pipe(pipefd)==-1){
        snprintf(response_buf, buf_size,
         "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: 25\r\n"
                 "\r\n"
                 "500 CGI Pipe Error");
                 return -1;
    }

    pid_t pid=fork();
    if(pid==-1){
        close(pipefd[0]);
        close(pipefd[1]);
        snprintf(response_buf,buf_size,
        "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: 25\r\n"
                 "\r\n"
                 "500 CGI Fork Error");
                 return -1;
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

        char cgi_output[4096];
        ssize_t total=0;
        ssize_t n;
        while((n=read(pipefd[0],cgi_output+total,sizeof(cgi_output)-total-1))>0){
          total+=n;
        }
        cgi_output[total]='\0';
        close(pipefd[0]);

        //等待子进程结束
        waitpid(pid,NULL,0);

        //构造HTTP响应
        snprintf(response_buf,buf_size,
        "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zd\r\n"
                 "\r\n"
                 "%s",
                total,cgi_output);


                return 0;

    }
}