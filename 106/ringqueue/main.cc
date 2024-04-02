#include "RingQueue.hpp"
#include <pthread.h>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

std::string SelfName()
{
    char name[128];
    snprintf(name,sizeof(name),"thread[0x%x]",pthread_self());
    return name;
}

void *ProductorRoutine(void *rq)
{
    RingQueue<int> *ringqueue=static_cast<RingQueue<int> *>(rq);
    while(true)
    {
        //sleep(1);
        //version1
        int data=rand() % 10+1;
        ringqueue->Push(data);
        std::cout<<SelfName()<<",生产完成，生产的数据是："<<data<<std::endl;
        sleep(1);
    }
}

void *ConsumerRoutine(void *rq)
{
    RingQueue<int> *ringqueue=static_cast<RingQueue<int> *>(rq);
    while(true)
    {
        //version1
        int data;
        ringqueue->Pop(&data);
        std::cout<<SelfName()<<",消费完成，消费的数据是："<<data<<std::endl;
    }
}

int main()
{
    srand((unsigned int)time(nullptr) ^ getpid() ^ pthread_self() ^ 0x71727374);
    RingQueue<int> *rq=new RingQueue<int>();

    //单生产，单消费
    pthread_t p[4], c[8];
    for(int i=0;i<4;i++) pthread_create(p+i,nullptr,ProductorRoutine,rq);
    for(int i=0;i<8;i++) pthread_create(c+i,nullptr,ConsumerRoutine,rq);

    for(int i=0;i<4;i++) pthread_join(p[i],nullptr);
    for(int i=0;i<4;i++) pthread_join(c[i],nullptr);
    delete rq;

    return 0;
}