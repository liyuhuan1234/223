#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>


void setNonBlock(int fd)
{
    int f1=fcntl(fd,F_GETFL);
    if(f1<0)
    {
        std::cerr<<"fcntl: "<<strerror(errno)<<std::endl;
        return;
    }
    fcntl(fd,F_SETFL,f1|O_NONBLOCK);
}

void prittLog()
{
    std::cout<<"this is a log"<<std::endl;
}

void download()
{
    std::cout<<"this is a download"<<std::endl;
}

void executSql()
{
    std::cout<<"this is a executSql"<<std::endl;
}