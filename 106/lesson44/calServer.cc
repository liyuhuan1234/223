#include "calServer.hpp"
#include <memory>


using namespace server;
using namespace std;

static void Usage(string proc)
{
    cout << "\nUsage:\n\t" << proc << " local_port\n\n";
}


//req:里面一定是处理好的完整的请求对象
//resp:根据req，进行业务处理，填充resp，不用管任何读取和写入
bool cal(const Request &req,Response &resp)
{
    //req已经有结构化完成的数据，可以直接使用
    resp.exitcode=OK;
    resp.result=OK;

    switch(req.op)
    {
    case '+':
        resp.result=req.x + req.y;
        break;
    case '-':
        resp.result=req.x - req.y;
        break;
    case '*':
        resp.result=req.x * req.y;
        break;
    case '/':
    {
        if(req.y==0) 
            resp.exitcode=DIV_ZERO;
        else 
            resp.result=req.x / req.y;
    }
        break;
    case '%':
    {
        if(req.y==0)
            resp.exitcode=MOD_ZERO;
        else   
            resp.result=req.x % req.y;
    }
        break;
    default:
        resp.exitcode=OP_ERROR;
        break;
    }
    return true;
}

//tcp 服务器，启动上和udp一模一样
//./tcpserver local_port
int main(int argc,char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    uint16_t port = atoi(argv[1]);

    unique_ptr<CalServer> tsvr(new CalServer());

    tsvr->initServer();
    tsvr->start(cal);

    return 0;
}