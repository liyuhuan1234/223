#include <stdio.h>

//左右指针法
void quick_sort(int* a,int left,int right)
{
	if(left>=right)
		return;

	int begin=left;
	int end=right;
	int keyi=begin;
	
	while(begin<end)
	{
		while(begin<end && a[end]>=a[keyi])
		{
			end--;
		}
		
		while(begin<end && a[begin]<=a[keyi])
		{
			begin++;
		}
		int tmp=a[end];
		a[end]=a[begin];
		a[begin]=tmp;
	}
	int tmp= a[begin];
	a[begin]=a[keyi];
	a[keyi]=tmp;

	quick_sort(a,left,begin-1);
	quick_sort(a,begin+1,right);
}


int main()
{
	int arr[]={6,4,7,9,3,6,5,10,9,8,6};
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
