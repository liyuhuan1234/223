#include "my_add.h"
#include "my_sub.h"

int main()
{
	int a=10;
	int b=20;

	int res=Sub(a,b);
	printf("result:%d\n",res);
	res=Add(a,b);
	printf("result:%d\n",res);

	return 0;	
}
