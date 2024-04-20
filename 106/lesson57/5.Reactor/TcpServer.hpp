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
#include "Protocol.hpp"

namespace tcpserver
{
    class Connection;
    class TcpServer;

    static const uint16_t defaultport=8080;
    static const int num=64;

    using func_t=std::function<void (Connection *)>;
    //using hander_t=std::function<void (const std::string &package)>;

    //每个套接字都要有自己的缓冲区
    class Connection
    {
    public:
        Connection(int sock,TcpServer *tsp):sock_(sock),tsp_(tsp)
        {}
        void Register(func_t r,func_t s,func_t e)
        {
            recver_=r;
            sender_=s;
            excepter_=e;
        }
        ~Connection()
        {}
        void Close()
        {
            close(sock_);
        }
    public:
        int sock_;
        std::string inbuffer_;//输入缓冲区
        std::string outbuffer_;//输出缓冲区

        func_t recver_;//从sock_读
        func_t sender_;//向sock_写
        func_t excepter_;//处理sock_IO的时候的异常事件

        TcpServer *tsp_;//可以省略
        uint64_t lasttime;
    };

    class TcpServer
    {
    private:
        void Recver(Connection* conn)
        {
            conn->lasttime=time(nullptr);
            char buffer[1024];
            while(true)
            {
                ssize_t s=recv(conn->sock_,buffer,sizeof(buffer)-1,0);
                if(s>0)
                {
                    buffer[s]=0;
                    conn->inbuffer_+=buffer;//将读到的数据入队列
                    logMessage(DEBUG,"\n%s",conn->inbuffer_);
                    service_(conn);               
                }
                else if(s==0)
                {
                    if(conn->excepter_)
                    {
                        conn->excepter_(conn);
                        return;
                    }
                }
                else
                {
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                        break;
                    else if(errno==EINTR)
                        continue;
                    else
                    {
                        if(conn->excepter_)
                        {
                            conn->excepter_(conn);
                            return;
                        }
                    }

                }
            }
        }
        //每个sock都要有自己的发送缓冲区
        void Sender(Connection* conn)
        {
            conn->lasttime=time(nullptr);
            while(true)
            {
                ssize_t s=send(conn->sock_,conn->outbuffer_.c_str(),conn->outbuffer_.size(),0);
                if(s>0)
                {
                    if(conn->outbuffer_.empty())
                    {
                        //EnableReadWrite(conn,true,false);
                        break;
                    }
                    else
                    {
                        conn->outbuffer_.erase(0,s);
                    }
                }
                else
                {
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                        break;
                    else if(errno==EINTR)
                        continue;
                    else
                    {
                        if(conn->excepter_)
                        {
                            conn->excepter_(conn);
                            return;
                        }                  
                    }
                }
            }
            //如果没有发送完毕，需要对对应的sock开启对写事件的关心，如果发完了，我们要关闭对写事件的关心
            if(!conn->outbuffer_.empty())
                conn->tsp_->EnableReadWrite(conn,true,true);
            else
                conn->tsp_->EnableReadWrite(conn,true,false);
        }
        void Excepter(Connection* conn)
        {
            logMessage(DEBUG,"Excepter begin");
            epoller_.Control(conn->sock_,0,EPOLL_CTL_DEL);
            conn->Close();
            connections_.erase(conn->sock_);

            logMessage(DEBUG,"关闭%d 文件描述符的所有资源",conn->sock_);
            delete conn;

        }
        void Accepter(Connection *conn)
        {
            for (;;)
            {
                std::string clientip;
                uint16_t clientport;
                int err = 0;
                int sock = sock_.Accept(&clientip, &clientport, &err);
                if (sock > 0)
                {
                    AddConnection(
                        sock, EPOLLIN | EPOLLET,
                        std::bind(&TcpServer::Recver, this, std::placeholders::_1),
                        std::bind(&TcpServer::Sender, this, std::placeholders::_1),
                        std::bind(&TcpServer::Excepter, this, std::placeholders::_1));

                    logMessage(DEBUG, "get a new link, info: [%s:%d]", clientip.c_str(), clientport);
                }
                else
                {
                    if(err==EAGAIN||err==EWOULDBLOCK)
                        break;
                    else if(err==EINTR)
                        continue;
                    else
                        break;
                }
            }
        }
void AddConnection(int sock, uint32_t events, func_t recver, func_t sender, func_t excepter)
        {
            // 1. 首先要为该sock创建Connection，并初始化，并添加到connections_
            if (events & EPOLLET)
                Util::SetNonBlock(sock);
            Connection *conn = new Connection(sock, this); //?
            // 2. 给对应的sock设置对应回调处理方法
            conn->Register(recver, sender, excepter);

            // 2. 其次将sock与它要关心的事件"写透式"注册到epoll中，让epoll帮我们关心
            bool r = epoller_.AddEvent(sock, events);
            assert(r);
            (void)r;

            // 3. 将kv添加到connections_
            connections_.insert(std::pair<int, Connection *>(sock, conn));

            logMessage(DEBUG, "add new sock : %d in epoll and unordered_map", sock);
        }

        bool IsConnectionExists(int sock)
        {
            auto iter=connections_.find(sock);
            return iter!=connections_.end();
        }

        void Loop(int timeout)
        {
            int n=epoller_.Wait(revs_,num_,timeout);
            for(int i=0;i<n;i++)
            {
                int sock=revs_[i].data.fd;
                uint32_t events=revs_[i].events;

                //将所有的异常问题，全部转化为读写问题
                if(events & EPOLLERR)
                    events |=(EPOLLIN|EPOLLOUT);
                if(events & EPOLLHUP)
                    events |=(EPOLLIN|EPOLLOUT);

                //listen事件就绪
                if((events & EPOLLIN) && IsConnectionExists(sock) && connections_[sock]->recver_) 
                    connections_[sock]->recver_(connections_[sock]);
                if((events & EPOLLOUT) && IsConnectionExists(sock) && connections_[sock]->sender_)
                    connections_[sock]->sender_(connections_[sock]);
            }
        }
    public:
        TcpServer(func_t func,uint16_t port=defaultport):service_(func),port_(port),revs_(nullptr)
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
            AddConnection(sock_.Fd(),EPOLLIN|EPOLLET,std::bind(&TcpServer::Accepter,this,std::placeholders::_1),nullptr,nullptr);

            revs_=new struct epoll_event[num];
            num_=num;
        }

        void EnableReadWrite(Connection *conn,bool readable,bool writeable)
        {
            uint32_t event=(readable?EPOLLIN:0)|(writeable?EPOLLOUT:0)|EPOLLET;
            epoller_.Control(conn->sock_,event,EPOLL_CTL_MOD);

        }

        //事件派发器
        void Dispatcher()
        {
            int timeout=1000;
            while(true)
            {
                Loop(timeout);
                logMessage(DEBUG,"time out...");

                //遍历connetions_,计算每一个连接已经有多长时间没有动了
            }
        }
        ~TcpServer()
        {    
            sock_.Close();
            epoller_.Close();
            if(nullptr==revs_)
                delete []revs_;
        }
    private:
        uint16_t port_;
        Sock sock_;
        Epoller epoller_;
        std::unordered_map<int,Connection *> connections_;
        struct epoll_event *revs_;
        int num_;
        //hander_t hander_;
        func_t service_;
    };
}