#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{	
	//close(0);
	//close(2);
	//close(1);
	umask(0);
	//int fd=open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
	int fd=open("log.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
	
	if(fd<0)
	{
		perror("open");
		return 1;
	}
	dup2(fd,1);
	printf("open fd:%d\n",fd);
	
	fflush(stdout);
	close(fd);

	return 0;
}
