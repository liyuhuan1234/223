#include <stdio.h>

void Swap(int* a,int* b)
{
    int tmp=*a;
    *a=*b;
    *b=tmp;
}

//三数取中
int GetMid(int arr[],int right,int left)
{
    int mid=right+(left-right)/2;
    if(arr[right]<arr[left])
    {
        if(arr[mid]<arr[right])
            return right;
        else if(arr[mid]>arr[left])
            return left;
        else
            return mid;
    }
    else
    {
        if(arr[mid]>arr[right])
            return right;
        else if(arr[mid]<arr[left])
            return left;
        else
            return mid;
    }
}

//挖坑法
void quick_sort(int arr[],int right,int left)
{
    if(right>left)
        return;
    int Mid=GetMid(arr,right,left);
    Swap(&arr[Mid],&arr[right]);
    int begin=right;
    int end=left;
    int pivot=begin;
    int key=arr[pivot];
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

    quick_sort(arr,right,pivot-1);
    quick_sort(arr,pivot+1,left);
}

int main()
{
    int arr[]={5,6,1,3,9,8,5,1,7,4,7,3,0,2};
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
    return 0;
}
