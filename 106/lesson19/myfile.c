#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
	//C
	printf("hello printf\n");
	fprintf(stdout,"hello fprintf\n");
	fputs("hello fputs\n",stdout);
	
	//system call
	const char* msg="hello write\n";
	write(1,msg,strlen(msg));
	
	fork();

	return 0;
}
