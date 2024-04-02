#include <stdio.h>

//三数取中
int mid(int* a,int left,int right)
{
	int mid=left+(right-left)/2;
	if(a[left]>a[right])
	{
		if(a[mid]<a[right])
			return right;
		else if(a[left]<a[mid])
			return left;
		else
			return mid;
			
	}
	else
	{
		if(a[mid]<a[left])
			return left;
		else if(a[mid]>a[right])
			return right;
		else
			return mid;
	}
}


//挖坑法
void quick_sort(int* a,int left,int right)
{
	if(left>=right)
		return;

	int ret=mid(a,left,right);
	int change=a[left];
	a[left]=a[ret];
	a[ret]=change;

	int begin=left;
	int end=right;
	int pivot=begin;
	int key=a[begin];
	
	while(begin<end)
	{
		while(begin<end && a[end]>=key)
		{
			end--;
		}
		a[pivot]=a[end];
		pivot=end;
		
		while(begin<end && a[begin]<=key)
		{
			begin++;
		}
		a[pivot]=a[begin];
		pivot=begin;
	}
	pivot=begin;
	a[pivot]=key;

	quick_sort(a,0,pivot-1);
	quick_sort(a,pivot+1,right);
}


int main()
{
	int arr[]={3,4,7,9,3,6,5,10,9,8,6};
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
