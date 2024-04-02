#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>


#define NAME_PIPE "/tmp/mypipe.106"

bool creatFifo(const std::string &path)
{
    int n=mkfifo(path.c_str(),0600);
    if(n==0) return true;
    else
    {
        std::cout<<"errno: "<<errno<<" err string "<<strerror(errno)<<std::endl;
        return false;
    }
}

void removeFifo(const std::string &path)
{
    int n=unlink(path.c_str());
    assert(n==0);//debug 方式有效，release版本无效
    (void)n;//防止release版本n被定义而被使用报warnning
}