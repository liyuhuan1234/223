#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include "err.hpp"
#include "log.hpp"
#include "sock.hpp"


namespace epoll_ns
{
    static const int defaultport=8080;
    static const int size=128;
    static const int defaultvalue=-1;
    class EpollServer
    {
    public:
        EpollServer(uint16_t port=defaultport):_port(port),_listensock(-1),_epfd(-1)
        {}
        void initServer()
        {
            //1.创建socket
            _listensock=Sock::Socket();
            Sock::Bind(_listensock,_port);
            Sock::Listen(_listensock);
            //2.创建epoll模型
            _epfd=epoll_create(size);
            if(_epfd<0)
            {
                logMessage(FATAL,"epoll create error: %s",strerror(errno));
                exit(EPOLL_CREATE_ERR);
            }
            //3.添加listensock到epoll中
            struct epoll_event ev;
            ev.events=EPOLLIN;
            ev.data.fd=_listensock;
            epoll_ctl(_epfd,EPOLL_CTL_ADD,_listensock,&ev);
        }
        void start()
        {
            for(;;)
            {
                int n=epoll_wait();
            }
        }
        ~EpollServer()
        {
            if(_listensock!=defaultvalue)
                close(_listensock);
            if(_epfd!=defaultvalue)
                close(_epfd);
        }
    private:
        uint16_t _port;
        int _listensock;
        int _epfd;
    };
}

