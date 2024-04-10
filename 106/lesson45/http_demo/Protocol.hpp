#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "Util.hpp"

const std::string sep="\r\n";
const std::string default_root="./wwwroot";
const std::string home_page="index.html";


class HttpRequest
{
public:
    HttpRequest(){}
    ~HttpRequest(){}
    void parse()
    {
        //1.从inbuffer中拿到第一行，分隔符\r\n
        std::string line=Util::getOneLine(inbuffer,sep);
        if(line.empty()) return;

        //2.从请求行中提取三个字段
        std::cout<<"line: "<<line<<std::endl;
        std::stringstream ss(line);
        ss>>method>>url>>httpversion;

        //3.添加web默认路径
        path=default_root;
        path+=url;//./wwwroot/a/b/c.html
        if(path[path.size()-1]=='/') path+=home_page;
    }
public:
    std::string inbuffer;
    // std::string reqline;
    // std::vector<std::string> reqheader;
    // std::string body;
    
    std::string method;
    std::string url;
    std::string httpversion;
    std::string path;
};

class HttpResponse
{
public:
    std::string outbuffer;
};