#include <stdio.h>

void Swap(int* a, int* b)
{
    int tmp=*a;
    *a=*b;
    *b=tmp;
}

int GetMid(int* arr,int left,int right)
{
    int mid=left+(right-left)/2;
    if(arr[left]<arr[right])
    {
        if(arr[mid]<arr[left])
            return left;
        else if(arr[mid]>arr[right])
            return right;
        else
            return mid;
    }
    else
    {
        if(arr[mid]>arr[left])
            return left;
        else if(arr[mid]<arr[right])
            return right;
        else
            return mid;
    }
}

//挖坑法
void quick_sort(int* arr,int left,int right)
{
    if(left>=right)
        return;
    
    int mid=GetMid(arr,left,right);
    Swap(&arr[mid],&arr[left]);
    int begin=left;
    int end=right;
    int pivot=begin;
    int key=arr[begin];
    while(begin<end)
    {
        while(begin<end && arr[end]>=key)
        {
            end--;
        }
        Swap(&arr[end],&arr[pivot]);
        pivot=end;

        while(begin<end && arr[begin]<=key)
        {
            begin++;
        }
        Swap(&arr[begin],&arr[pivot]);
        pivot=begin;
    }
    pivot=begin;
    Swap(&arr[pivot],&key);

    quick_sort(arr,left,pivot-1);
    quick_sort(arr,pivot+1,right);
}


int main()
{
    int arr[]={7,5,3,1,6,9,3,4,8,1,4,5,7,8,2};
    int i=0;
    int sz=sizeof(arr)/sizeof(arr[0]);
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

    return 0;
}
