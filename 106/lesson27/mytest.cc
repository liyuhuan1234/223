#include <iostream>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	while(true)
	{
		std::cout<<"我是一个正在运行的进程，pid: "<<getpid()<<std::endl;
		sleep(1);
	}
}
