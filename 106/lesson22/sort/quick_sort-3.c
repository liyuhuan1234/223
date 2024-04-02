#include <stdio.h>

//前后指针法
void quick_sort(int* a,int left,int right)
{
	if(left>=right)
		return;

	int prev=left;
	int cur=prev+1;
	int keyi=left;
	
	while(cur<=right)
	{
		if(a[cur]<=a[keyi])
		{	
			++prev;
			int tmp=a[prev];
			a[prev]=a[cur];
			a[cur]=tmp;
		}
		++cur;
	}
	int tmp=a[prev];
	a[prev]=a[keyi];
	a[keyi]=tmp;

	quick_sort(a,left,prev-1);
	quick_sort(a,prev+1,right);
}


int main()
{
	int arr[]={7,4,5,3,1,7,9,3,6,5,9,8,6,1,4,3};
	int sz=sizeof(arr)/sizeof(arr[0]);
	int i=0;
	for(i=0;i<sz;i++)
	{	
		printf("%d ",arr[i]);
	}
	printf("\n");
	
	quick_sort(arr,0,sz-1);
	
	for(i=0;i<sz;i++)
	{	
		printf("%d ",arr[i]);
	}	
	printf("\n");
}
