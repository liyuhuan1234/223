#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <ctime>
#include <sys/wait.h>
#include <sys/types.h>

#define MakeRandSeed() srand((unsigned long)time(nullptr)^getpid()^0x171237^rand()%1234)
#define PROCSS_NUM 5

//////////////////////////子进程要完成的某种任务--模拟//////////////////////////
//函数指针类型
typedef void(*func_t)();

void downLoadtask()
{
	std::cout<<getpid()<<":下载任务\n"<<std::endl;
	sleep(1);
}

void ioTask()
{
	std::cout<<getpid()<<":IO任务\n"<<std::endl;
	sleep(1);
}

void flushTask()
{
	std::cout<<getpid()<<":刷新任务\n"<<std::endl;
	sleep(1);
}

void loadTaskFunc(std::vector<func_t> *out)
{
	assert(out);
	out->push_back(downLoadtask);
	out->push_back(ioTask);
	out->push_back(flushTask);
}

//////////////////////////下面的代码是一个多进程程序//////////////////////////
class subEp
{
public:
	subEp(pid_t subId,int writeFd)
	:subId_(subId),writeFd_(writeFd)
	{
		char nameBuffer[1024];
		snprintf(nameBuffer,sizeof(nameBuffer),"process-%d[pif(%d)-fd(%d)]",num++,subId_,writeFd_);
		name_=nameBuffer;
	}
public:
	static int num;
	std::string name_;
	pid_t subId_;
	int writeFd_;
};
int subEp::num=0;

int recvTask(int readFd)
{
	int code=0;
	ssize_t s=read(readFd,&code,sizeof code);
	if(s==4) return code;
	else if(s<=0) return -1;
	else return 0;
}

void sendTask(const subEp &process,int taskNum)
{
	std::cout<<"send task num:"<<taskNum<<" send to " <<process.name_<<std::endl;
	int n=write(process.writeFd_,&taskNum,sizeof(taskNum));
	assert(n==sizeof(int));
	(void)n;
}

void creatSubProcess(std::vector<subEp> *subs,std::vector<func_t> &funcMap)
{
	std::vector<int> deleteFd;
	for(int i=0;i<PROCSS_NUM;i++)
	{
		int fds[2];
		int n=pipe(fds);
		assert(n==0);
		(void)n;
		//bugs
		pid_t id=fork();
		if(id==0)
		{
			for(int i=0;i<deleteFd.size();i++)
				close(deleteFd[i]);
			//子进程,处理任务
			close(fds[1]);
			while(true)
			{
				//1.获取命令码，如果没有发送，我们子进程应该阻塞
				int commandCode=recvTask(fds[0]);
				//2.完成任务
				if(commandCode>=0&&commandCode<funcMap.size()) 
					funcMap[commandCode]();
				else if(commandCode==-1) 
					break;
			}
			exit(0);
		}
		close(fds[0]);
		subEp sub(id,fds[1]);
		//subs.push_back(std::move(sub));
		subs->push_back(sub);
		deleteFd.push_back(fds[1]);
	}
}

void loadBlanceContrl(const std::vector<subEp> &subs, const std::vector<func_t> &funcMap,int count)
{
	int processnum=subs.size();
	int tasknum=funcMap.size();
	bool forever=(count==0?true:false);
	while(true)
	{
		//1.选择一个子进程-->std::vector<subEp>-->index -随机数
		int subIdx=rand()%processnum;
		//2.选择一个任务-->std::vector<func_t> -->index
		int taskIdx=rand()%tasknum;
		//3.任务发送给选择的进程
		sendTask(subs[subIdx],taskIdx);
		sleep(1);
		if(!forever)
		{
			count--;
			if(count==0) break;
		}
	}
	//write quit -> read 0
	for(int i=0;i<processnum;i++) close(subs[i].writeFd_);
}


void waitProcess(std::vector<subEp> processes)
{
	int processnum=processes.size();
	for(int i=0;i<processnum;i++)
	{
		waitpid(processes[i].subId_,nullptr,0);
		std::cout<<"wait sub process success ..." <<processes[i].subId_<<std::endl;
	}
}


int main()
{
	MakeRandSeed();
	//1.建立子进程并建立和子进程通信的信道
	//1.1 加载方法表
	//[子进程id,wfd]
	std::vector<func_t> funcMap;
	loadTaskFunc(&funcMap);
	//1.2 创建子进程，并且维护好父子通信信道
	std::vector<subEp> subs;
	creatSubProcess(&subs,funcMap);
	//2.父进程，控制子进程，负载均衡的向子进程发送任务
	int taskCnt=3;//0:永远进行
	loadBlanceContrl(subs, funcMap,taskCnt);

	//3.回收子进程信息
	waitProcess(subs);

	return 0;
}
