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

//前后指针法
void quick_sort(int* arr,int left,int right)
{
    if(left>=right)
        return;
    
    int mid=GetMid(arr,left,right);
    Swap(&arr[mid],&arr[left]);
    int begin=left;
    int end=right;
    int keyi=begin;
    int prev=begin;
    int cur=prev+1;

    while(cur<=right)
    {
        if(arr[cur]<arr[keyi])
        {
            ++prev;
            Swap(&arr[prev],&arr[cur]);
        }
        ++cur;
    }
    Swap(&arr[keyi],&arr[prev]);

    quick_sort(arr,left,prev-1);
    quick_sort(arr,prev+1,right);
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