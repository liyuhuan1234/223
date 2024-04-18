#include "util.hpp"
#include <cstdio>
#include <vector>
 #include <functional>

using func_t=std::function<void ()>;

#define INIT(v) do{\
            v.push_back(prittLog);\
        }while(0)

#define EXEC_OTHER(cbs) do {\
        for(auto const &cb:cbs) cb();\
    }while(0)


int main()
{
    std::vector<func_t> cbs;
    INIT(cbs);

    setNonBlock(0);
    char buffer[1024];
    while(true)
    {
        // printf(">>> ");
        // fflush(stdout);
        ssize_t s=read(0,buffer,sizeof(buffer)-1);
        if(s>0)
        {
            buffer[s-1]=0;
            std::cout<<"echo# "<<buffer<<std::endl;
        }
        else if(s==0)
        {
            std::cout<<"read end"<<std::endl;
            break;
        }
        else
        {
            //当我不输入的时候，底层没有数据
            //std::cout<<"EAGAIN: "<<EAGAIN<<" EWOULDBLOCK: "<<EWOULDBLOCK<<std::endl;

            if(errno==EAGAIN)
            {
                std::cout<<"我没错，只是没数据"<<std::endl;
                EXEC_OTHER(cbs);
            }

            else if(errno==EINTR)
            {
                continue;
            }

            else
            {
                std::cout<<"s: "<<s<<" errno: "<<strerror(errno)<<std::endl;
                break;
            }
        }
        sleep(1);
    }
}