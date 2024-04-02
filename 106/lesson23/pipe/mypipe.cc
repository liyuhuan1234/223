#include <iostream>
#include <unistd.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstring>

using namespace std;

//父进程进行读取，子进程写入
int main()
{
    //第一步：创建管道文件，确定读写端
    int fds[2];
    int n=pipe(fds);
    assert(n==0);

    //第二步：fork
    pid_t id=fork();
    assert(id>=0);
    if(id==0)
    {
        //子进程进行写入
        close(fds[0]);
        //子进程的通信代码
        //string msg="hello,i am child";
        const char *s="我是子进程，我正在给你发消息";
        int cnt=0;
        while(true)
        {
            cnt++;
            char buffer[1024];
            snprintf(buffer,sizeof buffer,"child->parent say:%s[%d][%d]",s,cnt,getpid());
            //写端写满时 也会阻塞 等读端读取
            write(fds[1],buffer,strlen(buffer));
            //每隔一秒写一次
            sleep(1);
        }

        //子进程
        close(fds[1]);
        exit(0);
    }

    //父进程进行读取
    close(fds[1]);

    //父进程的通信代码
    while(true)
    {
        char buffer[1024];
        //如果管道中没有了数据，读端在读，默认会直接阻塞当前正在读取的进程
        ssize_t s=read(fds[0],buffer,sizeof(buffer)-1);
        if(s>0) buffer[s]=0;
        cout<<"Get Message# "<<buffer<<"|my pid:"<<getpid()<<endl;
        //父进程没有进行sleep
    }

    n=waitpid(id,nullptr,0);
    assert(n==id);
    close(fds[0]);


    //[0]:读取
    //[1]:写入
    // cout<<"fds[0]:"<<fds[0]<<endl;
    // cout<<"fds[1]:"<<fds[1]<<endl;

    return 0;
}