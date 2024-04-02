#include "myStdio.h"

int main()
{
	FILE_ *fp=fopen_("./log.txt","w");
	if(fp==NULL)
	{
		return 1;
	}	
	
	int cnt=0;
	const char *msg="hello bit";
	while(1)
	{
		cnt++;
		fwrite_(msg,strlen(msg),fp);
		sleep(1);
		if(cnt==10) break;
	}

	fclose_(fp);
	return 0;
}
