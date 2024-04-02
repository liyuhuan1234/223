#include <iostream>
#include <pthread.h>
#include <cassert>
#include <unistd.h>
#include <string>
#include <vector>

using namespace std;

class ThreadData
{
public:
	pthread_t tid;
	char namebuffer[64];
};

void *start_routine(void *args)
{
	//新线程
	ThreadData *td=static_cast<ThreadData *>(args);//安全的进行强制类型转换
	int cnt=10;
	while(cnt)
	{
		cout<<"new thread create success,name: "<<td->namebuffer<<" cnt: "<<cnt--<<endl;
		sleep(1);
	}
	delete td;
	return nullptr;
}

int main()
{
	//1.创建一批线程
	vector<ThreadData*> threads;
#define NUM 10
	for(int i=0;i<NUM;i++)
	{
		ThreadData *td=new ThreadData();

		snprintf(td->namebuffer,sizeof(td->namebuffer),"%s:%d","thread",i);
		//pthread_create(&tid,nullptr,start_routine,(void*)"thread one");
		pthread_create(&td->tid,nullptr,start_routine,td);
		threads.push_back(td);
		//sleep(1);
	}

	// pthread_t id;
	// int n=pthread_create(&id,nullptr,start_routine,(void*)"thread new");
	// assert(n==0);
	// (void)n;

	for(auto &iter :threads)
	{
		std::cout<<"create thread: "<<iter->namebuffer<<" : "<<iter->tid<<" success"<<endl;
	}

	//主线程
	while(true)
	{
		cout<<"new thread create success,name: main thread"<<endl;
		sleep(1);
	}

	return 0;
}
