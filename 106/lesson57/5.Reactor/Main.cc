#include "TcpServer.hpp"
#include <memory>

using namespace tcpserver;

static void usage(std::string proc)
{
    std::cerr<<"Usage:\n\t"<<proc<<" port"<<std::endl;
}

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

void calculate(Connection *conn)
{
    std::string onePackage;
    while(ParseOnePackage(conn->inbuffer_,&onePackage))
    {
        std::string reqStr;
        if(!deLength(onePackage,&reqStr)) 
            return;
        std::cout<<"去掉报头的正文:\n"<<reqStr<<std::endl;

        Request req;
        if(!req.deserialize(reqStr)) 
            return;

        Response resp;
        cal(req,resp);

        std::string respStr;
        resp.serialize(&respStr);

        conn->outbuffer_+=enLength(respStr);

        std::cout<<"-------------result: "<<conn->outbuffer_<<std::endl;
    }
    //发回去
    if(conn->sender_) 
        conn->sender_(conn);
    //如果没有发送完毕，需要对对应的sock开启对写事件的关心，如果发完了，我们要关闭对写事件的关心
    // if(!conn->outbuffer_.empty())
    //     conn->tsp_->EnableReadWrite(conn,true,true);
    // else
    //     conn->tsp_->EnableReadWrite(conn,true,false);
}

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port=atoi(argv[1]);

    std::unique_ptr<TcpServer> tsvr(new TcpServer(calculate,port));

    tsvr->InitServer();
    tsvr->Dispatcher();

    return 0;
}