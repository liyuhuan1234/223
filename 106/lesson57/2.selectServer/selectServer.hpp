#pragma once

#include <iostream>
#include "sock.hpp"

namespace select_ns
{
    static const int defaultport=8080;
    static const int fdnum=sizeof(fd_set)*8;
    static const int defaultfd=-1;
    class SelectServer
    {
    public:
        SelectServer(int port=defaultport):_port(port),_listensock(-1),fdarray(nullptr)
        {
        }
        void Print()
        {
            std::cout<<"fd list: ";
            for(int i=0;i<fdnum;i++)
            {
                if(fdarray[i]!=defaultfd) std::cout<<fdarray[i]<<" ";
            }
            std::cout<<std::endl;
        }
        void HandlerEvent(fd_set &rfds)
        {
            //目前一定是listensock
            if(FD_ISSET(_listensock,&rfds))
            {
                //select告诉我，listensock读就绪
                std::string clientip;
                uint16_t clientport=0;
                int sock=Sock::Accept(_listensock,&clientip,&clientport);
                if(sock<0) return;
                logMessage(NORMAL,"accept success [%s:%d]",clientip.c_str(),clientport);

                //将新的sock托管给select的本质，其实就是将sock添加到fdarray数组中
                int i=0;
                for(;i<fdnum;i++)
                {
                    if(fdarray[i]!=defaultfd) continue;
                    else break;
                }
                if(i==fdnum)
                {
                    logMessage(WARNING,"server if full,please wait");
                    close(sock);
                }
                else
                {
                    fdarray[i]=sock;
                }
                Print();
            }
        }
        void initServer()
        {
            _listensock=Sock::Socket();
            Sock::Bind(_listensock,_port);
            Sock::Listen(_listensock);

            fdarray=new int[fdnum];
            for(int i=0;i<fdnum;i++) fdarray[i]=defaultfd;
            fdarray[0]=_listensock;
        }
        void start()
        {
            for(;;)
            {
                fd_set rfds;
                FD_ZERO(&rfds);
                int maxfd=fdarray[0];

                for(int i=0;i<fdnum;i++)
                {
                    if(fdarray[i]==defaultfd) 
                        continue;
                    FD_SET(_listensock,&rfds);//将合法fd全部添加到读文件描述符集中

                    if(maxfd<fdarray[i]) maxfd=fdarray[i];
                }

                struct timeval timeout={1,0};
                //int n=select(_listensock+1,&rfds,nullptr,nullptr,&timeout);
                //一般而言，要用select,需要程序员自己维护一个保存合法fd的数组
                int n=select(maxfd+1,&rfds,nullptr,nullptr,nullptr);
                switch(n)
                {
                case 0:
                    logMessage(NORMAL,"timeout...");
                    break;
                case -1:
                    logMessage(WARNING,"select error,code: %d,err string: %s",errno,strerror(errno));
                    break;
                default:
                    //有事件就绪，目前只有一个监听事件
                    logMessage(NORMAL,"get a new link...");
                    HandlerEvent(rfds);
                    break;
                }
                // std::string clientip;
                // uint16_t clientport=0;
                // int sock=Sock::Accept(_listensock,&clientip,&clientport);
                // if(sock<0) continue;
                //开始服务器的处理逻辑

            }
        }
        ~SelectServer()
        {
            if(_listensock<0) close(_listensock);
            if(fdarray) delete []fdarray;
        }
    private:
        int _port;
        int _listensock;
        int *fdarray;
    };
}