#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <signal.h>
#include <sys/types.h>

static void Usage(const std::string &proc)
{
	std::cout<<"\nUsage: "<<proc<<" pid signo\n"<<std::endl;
}


int main(int argc,char *argv[])
{
	//一秒之后向进程发送信号,默认动作是终止进程
	//可以用于统计1秒之类系统可以执行多少次后面的代码
	alarm(1);

	int cnt=0;
	while(true)
	{
		std::cout<<"cnt: "<<cnt++<<std::endl;
	}
	//1.通过键盘发送信号
	//2.系统调用向目标进程发送信号
	//raise() 给自己发送任意信号 kill(getpid(),任意信号)
	//abort() 给自己发送SIGABRT信号 kill(getpid(),SIGABRT)
	//3.硬件异常产生信号
	//4.软件异常产生信号
	//while(true)
	//{
	//	std::cout<<"我在运行中..."<<std::endl;
	//	sleep(1);
	//	int a=10;
	//	//除0会终止进程 当前进程会收到来自操作系统的信号 SIGFPE
	//	a/=0;
	//}
	//if(argc!=3)
	//{
	//	Usage(argv[0]);
	//	exit(0);
	//}
	//pid_t id=atoi(argv[1]);
	//int signo=atoi(argv[2]);
	//int n=kill(id,signo);
	//if(n!=0)
	//{
	//	perror("kill");
	//}

	//while(true)
	//{
	//	std::cout<<"hello saturday"<<std::endl;
	//	sleep(1);
	//}
	return 0;
}
