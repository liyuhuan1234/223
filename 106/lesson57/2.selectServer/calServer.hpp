#pragma once

#include "log.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include "protocol.hpp"

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

    //const Request &req:输入型
    //Response &resp:输出型
    typedef std::function<bool(const Request &req,Response &resp)> func_t;

    //保证解耦
    void handlerEntery(int sock,func_t func)
    {
        std::string inbuffer;
        while(true)
        {
            //1.读取
            //1.1怎么保证读到的消息是一个完整的请求
            std::string req_text,req_str;
            //1.2保证req_text里面一定是一个完整的请求："content_len"\r\n"x op y"\r\n
            if(!recvPackage(sock,inbuffer,&req_text)) 
                return;

            std::cout<<"带报头的请求\n"<<req_text<<std::endl;

            if(!deLength(req_text,&req_str)) 
                return;

            std::cout<<"去掉报头的正文\n"<<req_str<<std::endl;

            //2.反序列化
            //2.1 得到一个结构化的请求对象
            Request req;
            if(!req.deserialize(req_str)) return;

            //3.计算处理
            //3.1得到一个结构化的响应
            Response resp;
            func(req,resp);

            //4.对响应resp进行序列化
            //4.1 得到了一个字符串
            std::string resp_str;
            resp.serialize(&resp_str);

            std::cout<<"计算完成，序列化响应:\n"<<req_str<<std::endl;

            //5.发送响应
            //5.1 构建成为一个完整的报文
            std::string send_string=enLength(resp_str);

            std::cout<<"构建完成完整的响应:\n"<<send_string<<std::endl;

            send(sock,send_string.c_str(),send_string.size(),0);//有问题
        }
    }

    class CalServer
    {
    public:
        CalServer(const uint16_t &port=gport):_listensock(-1),_port(port)
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

            //3.设置socket为监听状态
            if(listen(_listensock,gbacklog)<0)
            {
                logMessage(FATAL,"listen socket error");
                exit(LISTEN_ERR);
            }
            logMessage(NORMAL,"listen socket success");
        }
        void start(func_t func)
        {
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

                //version2 多进程版
                pid_t id=fork();
                if(id==0)
                {
                    //child
                    close(_listensock);
                    //if(fork()>0) exit(0);
                    //serviceIO(sock);
                    handlerEntery(sock,func);

                    close(sock);//对已经使用完毕的sock，要关闭这个sock，否则会导致文件描述符泄漏
                    exit(0);
                }
                close(sock);
                //father
                pid_t ret=waitpid(id,nullptr,0);
                if(ret>0)
                {
                    logMessage(NORMAL,"wait child success");
                }
            }
        }

        ~CalServer(){}
    private:
        int _listensock; //不是用来通信的，而是用来监听链接
        uint16_t _port;
    };
}