#include "HttpServer.hpp"
#include <memory>

using namespace std;
using namespace server;

void Usage(std::string proc)
{
    cerr<<"Usage:\n\t"<<proc<<" port\r\n\r\n";
}

//1.服务器和网页分离
//2.url-> /:web根目录
bool Get(const HttpRequest &req,HttpResponse &resp)
{
    cout<<"------------------http start--------------"<<endl;
    cout << req.inbuffer<<std::endl;
    std::cout<<"method: "<<req.method<<std::endl;
    std::cout<<"url: "<<req.url<<std::endl;
    std::cout<<"httpversion: "<<req.httpversion<<std::endl;
    std::cout<<"path: "<<req.path<<std::endl;
    cout<<"-------------------http end---------------"<<endl;

    std::string respline="HTTP/1.1 200 OK\r\n";
    std ::string respheader="Content-Type:text/html\r\n";
    std::string respblank="\r\n";
    std::string body="<html lang=\"en\"><head><meta charset=\"UTF-8\"><title> for test</title><h1>hello world</h1></head><body><p>你是猪 你是猪 你是猪</p></body></html>";

    resp.outbuffer+=respline;
    resp.outbuffer+=respheader;
    resp.outbuffer+=respblank;
    resp.outbuffer+=body;
    return true;
}

// ./httpserver 8080
int main(int argc,char *argv[])
{
    if(argc !=2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port=atoi(argv[1]);
    unique_ptr<HttpServer> httpsvr(new HttpServer(Get,port));
    httpsvr->initServer();
    httpsvr->start();
    return 0;
}