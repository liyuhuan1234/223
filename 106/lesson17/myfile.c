#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<assert.h>

#define FILE_NAME "log.txt"

//每一个宏对应的数值只有一个比特位是1，彼此位置不重叠
//#define ONE (1<<0)
//#define TWO (1<<1)
//#define THREE (1<<2)
//#define FOUR (1<<3)
//
//void show(int flags)
//{
//	if(flags & ONE) printf("ONE\n");
//	if(flags & TWO) printf("TWO\n");
//	if(flags & THREE) printf("THREE\n");
//	if(flags & FOUR) printf("FOUR\n");
//}

int main()
{	
	umask(0);
	//"w" -> O_WRONLY | O_CREAT | O_TRUNC 
	//"a" -> O_WRONLY | O_CREAT | O_APPEND
	//int fd=open(FILE_NAME,O_WRONLY|O_CREAT|O_TRUNC,0666);
	int fd=open(FILE_NAME,O_RDONLY);
	assert(fd!=-1);
	(void)fd;
	
	char buffer[1024];
	ssize_t num=read(fd,buffer,sizeof(buffer)-1);
	if(num>0)
	{
		buffer[num]=0;
	}
	printf("%s",buffer);

	//int cnt=5;
	//char outBuffer[64];
	//while(cnt)
	//{
	//	sprintf(outBuffer,"%s:%d\n","hellobit",cnt--);
	//	//库函数向文件中写入时不考虑\0，因此不用+1
	//	write(fd,outBuffer,strlen(outBuffer));
	//}
	
	//printf("fd:%d\n",fd);
	close(fd);






	//show(ONE);
	//printf("----------------------\n");
	//show(TWO);
	//printf("----------------------\n");
	//show(ONE|TWO);
	//printf("----------------------\n");
	//show(ONE|TWO|THREE);
	//printf("----------------------\n");
	//show(ONE|TWO|THREE|FOUR);	
	//printf("----------------------\n");


	//r w r+(读写，不存在出错) w+(读写，不存在自动创建) a(追加) a+()
	//以w方式打开文件，会自动清空内部数据
	// 0666 & ~umask = 664
	//FILE *fp=fopen(FILE_NAME,"w");
	//FILE *fp=fopen(FILE_NAME,"r");
	//FILE *fp=fopen(FILE_NAME,"a");
	//if(NULL==fp)
	//{
	//	perror("fopen");
	//	return 1;
	//}

	//char buffer[64];
	//while(fgets(buffer,sizeof(buffer)-1, fp)!=NULL)
	//{	
	//	buffer[strlen(buffer)-1]=0;
	//	puts(buffer);
	//}

	//int cnt=5;
	//while(cnt)
	//{
	//	fprintf(fp,"%s:%d\n","hello bit",cnt--);
	//}

	//fclose(fp);
}
