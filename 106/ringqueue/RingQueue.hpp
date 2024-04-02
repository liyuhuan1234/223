#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <semaphore.h>
#include <pthread.h>

static const int gcap=5;

template<class T>
class RingQueue
{
private:
    void P(sem_t &sem)
    {
        int n=sem_wait(&sem);
        assert(n==0);
        (void)n;
    }
    void V(sem_t &sem)
    {
        int n=sem_post(&sem);
        assert(n==0);
        (void)n;
    }
public:
    RingQueue(const int &cap=gcap): _queue(cap), _cap(cap)
    {
        int n=sem_init(&_spaceSem,0,_cap);
        assert(n==0);
        n=sem_init(&_dataSem,0,0);
        assert(n==0);

        _productorStep=_consumerStep=0;
        pthread_mutex_init(&_pmutex,nullptr);
        pthread_mutex_init(&_cmutex,nullptr);
    }
    //生产者
    void Push(const T &in)
    {   
        P(_spaceSem);//申请到了空间信号量，意味着一定能进行正常的生产
        pthread_mutex_lock(&_pmutex);
        _queue[_productorStep++]=in;
        _productorStep %= _cap;
        pthread_mutex_unlock(&_pmutex);
        V(_dataSem);
    }
    //消费者
    void Pop(T *out)
    {
        pthread_mutex_lock(&_cmutex);
        P(_dataSem);
        *out=_queue[_consumerStep++];
        _consumerStep %= _cap;
        V(_spaceSem);
        pthread_mutex_unlock(&_cmutex);
    }

    ~RingQueue()
    {
        sem_destroy(&_spaceSem);
        sem_destroy(&_dataSem);

        pthread_mutex_destroy(&_pmutex);
        pthread_mutex_destroy(&_cmutex);
    }
private:
    std::vector<T> _queue;
    int _cap;
    sem_t _spaceSem; //生产者想生产，看重的是空间资源
    sem_t _dataSem; //消费者想消费，看重的是数据资源
    int _productorStep;
    int _consumerStep;
    pthread_mutex_t _pmutex;
    pthread_mutex_t _cmutex;

};