#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <vector>

using namespace std;

//#define BLOCK_SIGNAL 2
#define MAX_SIGNUM 31

static vector<int> sigarr={2};

static void show_pending(const sigset_t &pending)
{
	for(int signo=MAX_SIGNUM;signo>=1;signo--)
	{
		if(sigismember(&pending,signo))
		{
			cout<<"1";
		}
		else
			cout<<"0";
	}
	cout<<"\n";
}

static void myhandler(int signo)
{
	cout<<signo<<" 号信号已经被递达！"<<endl;
}

int main()
{
	for(const auto &sig:sigarr) signal(sig,myhandler);
	//1.先尝试屏蔽指定的信号
	sigset_t block,oblock,pending;
	//1.1 初始化
	sigemptyset(&block);
	sigemptyset(&oblock);
	sigemptyset(&pending);
	
	//1.2 添加要屏蔽的信号
	for(const auto &sig:sigarr) sigaddset(&block,sig);
	
	//1.3 开始屏蔽,设置进内核
	sigprocmask(SIG_SETMASK,&block,&oblock);
	
	//2. 遍历打印pending信号集
	int cnt=10;
	while(true)
	{
		//2.1 初始化
		sigemptyset(&pending);
		//2.2 获取
		sigpending(&pending);
		//2.3 打印
		show_pending(pending);
		sleep(1);
		if(cnt--==0)
		{
			cout<<"恢复对信号的屏蔽，不屏蔽任何信号\n";
			sigprocmask(SIG_SETMASK,&oblock,&block);
		}
	}

}
