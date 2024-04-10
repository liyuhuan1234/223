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
#include "protocol.hpp"

#define NUM 1024

class CalClient
{
public:
    CalClient(const std::string &serverip,const uint16_t &serverport)
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
            std::string line;
            std::string inbuffer;
            while(true)
            {
                std::cout<<"mycal>>> ";
                std::getline(std::cin,line);//1+1

                Request req=ParseLine(line);
                std::string content;
                req.serialize(&content);
                std::string send_string=enLength(content);
                send(_sock,send_string.c_str(),send_string.size(),0);

                std::string package,text;
                //"content_len"\r\n"exitcode result"\r\n
                if(!recvPackage(_sock,inbuffer,&package)) continue;
                if(!deLength(package,&text)) continue;
                //exitcode result
                Response resp;
                resp.deserialize(text);
                std::cout<<"exitCode: "<<resp.exitcode<<std::endl;
                std::cout<<"result: "<<resp.result<<std::endl;
            }
        }
    }

    Request ParseLine(const std::string &line)
    {
        //简易版本的状态机
        //"1+1"  "123*456"
        int status =0;//0:操作符之前 1：碰到了操作符 2：操作符之后
        int i=0;
        int cnt=line.size();
        std::string left,right;
        char op;
        while(i<cnt)
        {
            switch(status)
            {
            case 0:
            {
                if(!isdigit(line[i]))
                {
                    op=line[i];
                    status=1;
                }
                else left.push_back(line[i++]);
            }
                break;
            case 1:
                i++;
                status=2;
                break;
            case 2:
                right.push_back(line[i++]);
                break;
            }
        }
        std::cout<< std::stoi(left) <<" "<< std::stoi(right) <<" "<< op <<std::endl;
        return Request(std::stoi(left),std::stoi(right),op);
    }
    ~CalClient() 
    {
        if(_sock>=0) close(_sock);
    }
private:
    int _sock;
    std::string _serverip;
    uint16_t _serverport;
};