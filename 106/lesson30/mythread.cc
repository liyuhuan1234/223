#include <iostream>
#include <pthread.h>
#include <cassert>
#include <unistd.h>

using namespace std;

void *thread_routine(void *args)
{
	//新线程
	while(true)
	{
		cout<<"我是新线程，我正在运行！"<<endl;
		sleep(1);
	}
}

int main()
{
	pthread_t tid;
	int n=pthread_create(&tid,nullptr,thread_routine,(void*)"thread one");
	assert(n==0);
	(void)n;

	//主线程
	while(true)
	{
		cout<<"我是主线程，我正在运行！"<<endl;
		sleep(1);
	}
	return 0;
}
