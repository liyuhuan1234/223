#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <poll.h>
#include "sock.hpp"

namespace poll_ns
{
    static const int defaultport=8080;
    static const int num=2048;
    static const int defaultfd=-1;

    using func_t=std::function<std::string (const std::string&)>;

    class PollServer
    {
    public:
        PollServer(func_t f,int port=defaultport):_func(f),_port(port),_listensock(-1),_rfds(nullptr)
        {
        }

        void initServer()
        {
            _listensock=Sock::Socket();
            Sock::Bind(_listensock,_port);
            Sock::Listen(_listensock);

            _rfds=new struct pollfd[num];
            for(int i=0;i<num;i++) 
                ResetItem(i);

            _rfds[0].fd=_listensock;
            _rfds[0].events=POLLIN;

        }

        void Print()
        {
            std::cout<<"fd list: ";
            for(int i=0;i<num;i++)
            {
                if(_rfds[i].fd!=defaultfd) std::cout<<_rfds[i].fd<<" ";
            }
            std::cout<<std::endl;
        }

        
        void ResetItem(int i)
        {
            _rfds[i].fd=defaultfd;
            _rfds[i].events=0;
            _rfds[i].revents=0;
        }

        void Accepter(int listensock)
        {
            logMessage(DEBUG,"Accepter in");
            //select告诉我，listensock读就绪
            std::string clientip;
            uint16_t clientport=0;
            int sock=Sock::Accept(_listensock,&clientip,&clientport);
            if(sock<0) return;
            logMessage(NORMAL,"accept success [%s:%d]",clientip.c_str(),clientport);

            //将新的sock托管给select的本质，其实就是将sock添加到fdarray数组中
            int i=0;
            for(;i<num;i++)
            {
                if(_rfds[i].fd!=defaultfd) continue;
                else break;
            }
            if(i==num)
            {
                logMessage(WARNING,"server if full,please wait");
                close(sock);
            }
            else
            {
                _rfds[i].fd=sock;
                _rfds[i].events=POLLIN;
                _rfds[i].revents=0;
            }
            Print();
            logMessage(DEBUG,"Accepter out");
        }


        void Recver(int pos)
        {
            logMessage(DEBUG,"in Recver");
            //1.读取request
            char buffer[1024];
            ssize_t s=recv(_rfds[pos].fd,buffer,sizeof(buffer)-1,0);//这里在进行读取的时候，不会被阻塞
            if(s>0)
            {
                buffer[s]=0;
                logMessage(NORMAL,"client# %s",buffer);
            }
            else if(s==0)
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(NORMAL,"client quit");
                return;
            }
            else
            {
                close(_rfds[pos].fd);
                ResetItem(pos);
                logMessage(ERROR,"client quit: %s",strerror(errno));
                return;
            }

            //2.处理request
            std::string response=_func(buffer);


            //3.返回
            //write
            write(_rfds[pos].fd,response.c_str(),response.size());

            logMessage(DEBUG,"out Recver");
        }

        //handler event rfds中，不仅仅是有一个fd是就绪的，可能存在多个
        void HandlerReadEvent()
        {
            for(int i=0;i<num;i++)
            {
                //过滤掉非法的fd
                if(_rfds[i].fd==defaultfd) continue;

                if(!_rfds[i].events & POLLIN) continue;
                //目前一定是listensock
                if(_rfds[i].fd==_listensock && (_rfds[i].revents & POLLIN)) 
                    Accepter(_listensock);
                else if(_rfds[i].revents & POLLIN)
                    Recver(i);
                else{}
            }

        }

        void start()
        {
            int timeout=1000;
            for(;;)
            {
                int n=poll(_rfds,num,timeout);
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
                    logMessage(NORMAL,"have event ready!");
                    HandlerReadEvent();
                    //HandlerWriteEvent(rfds);
                    break;
                }
            }
        }
        ~PollServer()
        {
            if(_listensock<0) 
                close(_listensock);
            if(_rfds) 
                delete []_rfds;
        }
    private:
        int _port;
        int _listensock;
        struct pollfd *_rfds;
        func_t _func;
    };
}