#pragma once

#include <iostream>
#include <functional>
#include <cassert>
#include <unordered_map>
#include "Log.hpp"
#include "Sock.hpp"
#include "Err.hpp"
#include "Epoller.hpp"
#include "Util.hpp"

namespace tcpserver
{
    class Connection;
    class TcpServer;

    static const uint16_t defaultport=8080;

    using func_t=std::function<void (Connection*)>;

    //每个套接字都要有自己的缓冲区
    class Connection
    {
    public:
        Connection(int sock):sock_(sock),tsp_(nullptr)
        {}
        void Register(func_t r,func_t s,func_t e)
        {
            recver_=r;
            sender_=s;
            excepter_=e;
        }
        ~Connection()
        {}
    public:
        int sock_;
        std::string inbuffer_;//输入缓冲区
        std::string outbuffer_;//输出缓冲区

        func_t recver_;//从sock_读
        func_t sender_;//向sock_写
        func_t excepter_;//处理sock_IO的时候的异常事件

        TcpServer *tsp_;//可以省略
    };

    class TcpServer
    {
    public:
        TcpServer(uint16_t port=defaultport):port_(port)
        {}

        void InitServer()
        {
            //1.创建socket
            sock_.Socket();
            sock_.Bind(port_);
            sock_.Listen();
            //2.构建epoll
            epoller_.Create();
            //3.将目前唯一的一个套接字添加到epoller中
            //listensock_也是一个socket,也要看作成为一个Connection
            AddConnection(sock_.Fd(),EPOLLIN|EPOLLET,std::bind(&TcpServer::Accepter,this,std::placeholders::-1),nullptr,nullptr);
        }

        void Accepter(Connection* conn)
        {

        }
        void AddConnection(int sock,uint32_t events,func_t recver,func_t sender,func_t excepter)
        {
            //1.首先要为该sock创建Conneticon,并初始化，并添加到connections_
            if(events & EPOLLET)
                Util::SetNonBlock(sock_.Fd());
            Connection *conn=new Connection(sock);
            //2.给对应的sock设置对应的回调方法
            conn->Register(recver,sender,excepter);

            //2.将sock与它要关心的事件”写透式“注册到epoll中，让epoll帮我们关心
            bool r=epoller_.AddEvent(sock,EPOLLIN|EPOLLET);
            assert(r);
            void(r);
            //3.将kv添加到connections_中
            connections_.insert(std::pair<int,Connection*>(sock,conn));
        }
        void Dispatch()
        {
            while(true)
            {
                // int n=epoller_wait();
                // for(int i=0;i<n;i++)
                // {
                //     int sock=
                //     connections_[sock]->recv
                }
            }
        }
        ~TcpServer()
        {          
        }
    private:
        uint16_t port_;
        Sock sock_;
        Epoller epoller_;
        std::unordered_map<int,Connection*> connections_;
    };
}