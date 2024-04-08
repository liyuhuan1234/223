#pragma once

#include "log.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

namespace server
{
    enum {
        USAGE_ERR=1,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };

    static const uint16_t gport=8080;
    static const int gbacklog=5;
    class TcpServer;

    class ThreadData
    {
    public:
        ThreadData(TcpServer *self,int sock):_self(self),_sock(sock)
        {}
    public:
        TcpServer *_self;
        int _sock;
    };

    class TcpServer
    {
    public:
        TcpServer(const uint16_t &port=gport):_listensock(-1),_port(port)
        {}
        void initServer()
        {
            //1.创建socket文件套接字对象
           _listensock=socket(AF_INET,SOCK_STREAM,0);
            if(_listensock<0)
            {
                logMessage(FATAL,"create socket error");
                exit(SOCKET_ERR);
            }
            logMessage(NORMAL,"create socket success: %d",_listensock);

            //2.bind绑定自己的网络信息
            struct sockaddr_in local;
            memset(&local,0,sizeof(local));
            local.sin_family=AF_INET;
            local.sin_port=htons(_port);
            local.sin_addr.s_addr=INADDR_ANY;
            if(bind(_listensock,(struct sockaddr*)&local,sizeof(local))<0)
            {
                logMessage(FATAL,"bind socket error");
                exit(BIND_ERR);
            }
            logMessage(NORMAL,"bind socket success");
            // logMessage(ERROR,"bind socket success");
            // logMessage(WARNING,"bind socket success");
            // logMessage(FATAL,"bind socket success");


            //3.设置socket为监听状态
            if(listen(_listensock,gbacklog)<0)
            {
                logMessage(FATAL,"listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL,"listen socket success");
        }
        void start()
        {
            signal(SIGCHLD,SIG_IGN);
            for(;;)
            {
                //4.server获取新链接
                //sock，和client进行通信的fd
                struct sockaddr_in peer;
                socklen_t len=sizeof(peer);
                int sock=accept(_listensock,(struct sockaddr*)&peer,&len);
                if(sock<0)
                {
                    logMessage(ERROR,"accept error,next");
                    continue;
                }
                logMessage(NORMAL,"accept a new link success");
                std::cout<<"sock: "<<sock<<std::endl;

                //5.这里就是一个sock，未来通信就用这个sock，面向字节流的，后续全部都是文件操作
                //version1
                // serviceIO(sock);
                // close(sock);//对已经使用完毕的sock，要关闭这个sock，否则会导致文件描述符泄漏

                //version2 多进程版
                // pid_t id=fork();
                // if(id==0)
                // {
                //     //child
                //     close(_listensock);
                //     //if(fork()>0) exit(0);
                //     serviceIO(sock);
                //     close(sock);//对已经使用完毕的sock，要关闭这个sock，否则会导致文件描述符泄漏
                //     exit(0);
                // }
                // close(sock);
                // //father
                // pid_t ret=waitpid(id,nullptr,0);
                // if(ret>0)
                // {
                //     std::cout<<"wait success: "<<ret<<std::endl;
                // }

                // version3 多线程版
                pthread_t tid;
                ThreadData* td=new ThreadData(this,sock);
                pthread_create(&tid,nullptr,threadRoutine,td);

                pthread_join(tid,nullptr);

                //version4 线程池

            }
        }
        static void *threadRoutine(void *args)
        {
            pthread_detach(pthread_self());
            ThreadData *td=static_cast<ThreadData*>(args);

            td->_self->serviceIO(td->_sock);
            delete td;
            close(td->_sock);
            return nullptr;
        }
        void serviceIO(int sock)
        {
            char buffer[1024];
            while(true)
            {
                ssize_t n=read(sock,buffer,sizeof(buffer)-1);
                if(n>0)
                {
                    //目前我们把读到的数据当成字符串
                    buffer[n]=0;
                    std::cout<<"recv message: "<<buffer<<std::endl;

                    std::string outbuffer=buffer;
                    outbuffer+="server[echo]";

                    write(sock,outbuffer.c_str(),outbuffer.size());//多路转接
                }
                else if(n==0)
                {
                    //代表client退出
                    logMessage(NORMAL,"client quit,me too!");
                    break;
                }
            }
        }
        ~TcpServer(){}
    private:
        int _listensock; //不是用来通信的，而是用来监听链接
        uint16_t _port;
    };
}