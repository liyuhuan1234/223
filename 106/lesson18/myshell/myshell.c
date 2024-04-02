#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define NUM 1024
#define OPT_NUM 64

#define NONE_REDIR    0
#define INPUT_REDIR   1
#define OUTPUT_REDIR  2
#define APPEND_REDIR  3

#define trimSpace(start) do{\
	while(isspace(*start)) start++;\
}while(0)

char lineCommand[NUM];
char *myargv[OPT_NUM];
int lastCode=0;
int lastSig=0;

int redirType=NONE_REDIR;
char *redirFile=NULL;

//"ls -a -l -i > myfile.txt" -> "ls -a -l -i" "myfile.txt"
void commandCheck(char *commands)
{
	assert(commands);
	char *start=commands;
	char *end=commands+strlen(commands);
	
	while(start<end)
	{
		if(*start=='>')
		{
			*start='\0';
			start++;
			if(*start=='>')
			{
				//"ls -a >> file.txt"
				redirType=APPEND_REDIR;
				start++;
			}
			else
			{
				//"ls -a > file.txt"
				redirType=OUTPUT_REDIR;
			}
			trimSpace(start);
			redirFile=start;
			break;
		}
		else if(*start=='<')
		{
			//"cat < file.txt"
			*start='\0';
			start++;
			//去掉空格
			trimSpace(start);
			//填写重定向信息
			redirType=INPUT_REDIR;
			redirFile=start;
			break;
		}
		else
		{
			start++;
		}
	}
}


int main()
{
	while(1)
	{
		//初始化
		redirType=NONE_REDIR;
		redirFile=NULL;
		errno=0;
		//输出提示符
		printf("用户名@主机名 当前路径#");
		fflush(stdout);

		//获取用户输入
		char *s=fgets(lineCommand,sizeof(lineCommand)-1,stdin);
		assert(s!=NULL);
		(void)s;
		//清除用户输入的\n
		lineCommand[strlen(lineCommand)-1]=0;
		//printf("test:%s\n",lineCommand);

		commandCheck(lineCommand);
	
		//字符串切割，如果没有子串 返回NULL
		myargv[0]=strtok(lineCommand," ");
		int i=1;
		if(myargv[0]!=NULL && strcmp(myargv[0],"ls")==0)
		{
			myargv[i++]=(char*)"--color=auto";
		}
		while(myargv[i++]=strtok(NULL," "));
		
		//如果是cd命令，不需要创建子进程，让shell自己执行对应的命令
		//像这种不需要让子进程执行，让shell自己执行的命令，称为内建或内置命令
		if(myargv[0]!=NULL && strcmp(myargv[0],"cd")==0)
		{
			if(myargv[1]!=NULL)
				chdir(myargv[1]);
			continue;
		}
		if(myargv[0]!=NULL && myargv[1]!=NULL && strcmp(myargv[0],"echo")==0 && strcmp(myargv[1],"$?")==0)
		{
			if(strcmp(myargv[1],"$?")==0)
			{
				printf("%d, %d\n",lastCode,lastSig);
			}
			else
			{
				printf("%s\n",myargv[1]);
			}
			continue;
		}

		//测试
#ifdef DEBUG
		for(i=0;myargv[i];i++)
		{
			printf("myargv[%d]:%s\n",i,myargv[i]);		
		}
#endif
	//执行命令
	pid_t id=fork();
	assert(id!=-1);

	if(id==0)
	{
		//真正重定向的工作是子进程完成的
		//由父进程给子进程提供信息
		switch(redirType)
		{
			case NONE_REDIR:
			//什么都不做
				break;
			case INPUT_REDIR:
				{
					int fd=open(redirFile,O_RDONLY);
					if(fd<0)
					{
						perror("open");
						exit(errno);
					}
					//重定向文件已经打开
					dup2(fd,0);
				}
				break;
			case OUTPUT_REDIR:
			case APPEND_REDIR:
				{	
					umask(0);
					int flags=O_WRONLY | O_CREAT;
					if(redirType==APPEND_REDIR) flags |= O_APPEND;
					else flags |= O_TRUNC;
					int fd=open(redirFile,flags,0666);
					if(fd<0)
					{
						perror("open");
						exit(errno);
					}
					dup2(fd,1);
				}
				break;
			default:
				printf("bug?\n");
				break;
			
		}		

		//执行程序替换的时候 不会影响曾经进程打开的重定向文件
		execvp(myargv[0],myargv);
		exit(1);
	}
	int status=0;
	pid_t ret=waitpid(id,&status,0);
	assert(ret>0);
	(void)ret;
	lastCode=((status>>8) & 0xFF);
	lastSig=status & 0x7f;
	}
}
