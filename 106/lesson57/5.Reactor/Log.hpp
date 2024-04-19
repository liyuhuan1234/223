#pragma once

#include <iostream>
#include <cstdarg>
#include <ctime>
#include <unistd.h>

#define DEBUG 0
#define NORMAL 1
#define WARNING 2
#define ERROR 3
#define FATAL 4

const char * to_levelstr(int level)
{
    switch(level)
    {
        case DEBUG:return  "DEBUG";
        case NORMAL:return "NORMAL";
        case WARNING:return "WARNING";
        case ERROR:return "ERROR";
        case FATAL:return "FATAL";
        default: return nullptr;
    }
}

void logMessage(int level,const char *format,...)
{
    //[日志等级] [时间戳/时间][pid][message]
    //[WARNING][2024-04-08 9:35:18] [123] [创建socket失败]
#define NUM 1024
    char logprefix[NUM];
    snprintf(logprefix,sizeof(logprefix),"[%s][%ld][pid:%d]",to_levelstr(level),(long int)time(nullptr),getpid());
    
    char logcontent[NUM];
    va_list arg;
    va_start(arg,format);
    vsnprintf(logcontent,sizeof(logcontent),format,arg);

    std::cout<<logprefix<<logcontent<<std::endl;
}