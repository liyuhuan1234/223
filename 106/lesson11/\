#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
	//创建子进程--fork是一个函数，函数执行前只有一个父进程--函数执行后:父进程+子进程
	fork(); 
	printf("我是一个进程，pid:%d,ppid:%d\n",getpid(),getppid());
	sleep(2);
/*	while(1)
	{
		printf("我是一个进程!,我的PID是:%d,父进程PPID是:%d\n",getpid(),getppid());
		sleep(1);
	}
	return 0;
*/
}
