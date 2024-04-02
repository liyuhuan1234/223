#include <iostream>
#include <signal.h>
#include <unistd.h>

using namespace std;

void handler(int signo)
{
	cout<<"get a signo: "<<signo<<endl;
}

int main()
{
	struct sigaction act,oact;
	act.sa_handler=handler;
	act.sa_flags=0;
	//当我们正在处理某一种信号时，系统会自动屏蔽同类型信号
	//当我们也想顺便屏蔽其它信号，就可以添加到sa_mask中
	sigaddset(&act.sa_mask,3);
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT,&act,&oact);

	while(true) sleep(1);
	
	return 0;
}
