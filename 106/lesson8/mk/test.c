#include<stdio.h>
#include<unistd.h>

int main()
{
	int cnt=10;
	while(cnt)
	{
		printf("剩余时间：%2d\r",cnt);
		fflush(stdout);
		cnt--;
		sleep(1);
	}
	return 0;
}
