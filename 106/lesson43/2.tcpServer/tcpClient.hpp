#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define NUM 1024

class TcpClient
{
public:
    TcpClient(const std::string &serverip,const uint16_t &serverport)
    :_sock(-1),_serverip(serverip),_serverport(serverport)
    {}
    void initClient()
    {
        //1.创建socket
        _sock=socket(AF_INET,SOCK_STREAM,0);
        if(_sock<0)
        {
            std::cerr<<"socket create error"<<std::endl;
            exit(2);
        }
        //2.tcp的client要bind，但是不用显示的bind，client port 要让OS自动指定
        //3.client不用listen
        //4.client不用accept
        //5.client要发起链接！

    }
    void start()
    {
        struct sockaddr_in server;
        memset(&server,0,sizeof(server));
        server.sin_family=AF_INET;
        server.sin_port=htons(_serverport);
        server.sin_addr.s_addr=inet_addr(_serverip.c_str());

        if(connect(_sock,(struct sockaddr*)&server,sizeof(server))!=0)
        {
            std::cerr<<"socket connect error"<<std::endl;
        }
        else
        {
            std::string msg;
            while(true)
            {
                std::cout<<"Enter# ";
                std::getline(std::cin,msg);
                write(_sock,msg.c_str(),msg.size());

                char buffer[NUM];
                int n=read(_sock,buffer,sizeof(buffer)-1);
                if(n>0)
                {
                    //目前我们把读到的数据当成字符串
                    buffer[n]=0;
                    std::cout<<"Server回显# "<<buffer<<std::endl;
                }
                else
                {
                    break;
                }
            }
        }
    }
    ~TcpClient()
    {
        if(_sock>=0) close(_sock);
    }
private:
    int _sock;
    std::string _serverip;
    uint16_t _serverport;
};