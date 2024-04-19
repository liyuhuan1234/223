#pragma once

#include <iostream>
#include <sys/epoll.h>
#include "Err.hpp"
#include "Log.hpp"

const static int defaultepfd=-1;
const static int size=128;

class Epoller
{
public:
    Epoller():epfd_(-1)
    {}
    ~Epoller()
    {
        if(epfd_!=defaultepfd)
            close(defaultepfd);
    }
public:
    void Create()
    {
        epfd_=epoll_create(size);
        if(epfd_<0)
        {
            logMessage(FATAL,"epoll_create error,code: %d,errstring: %s",errno,strerror(errno));
            exit(EPOLL_CREATE_ERR);
        }
    }
    //用户->内核
    bool AddEvent(int sock,uint32_t events)
    {
        struct epoll_event ev;
        ev.events=events;
        ev.data.fd=sock;

        int n=epoll_ctl(epfd_,EPOLL_CTL_ADD,sock,&ev);
        return n==0;
    }
private:
    int epfd_;

};