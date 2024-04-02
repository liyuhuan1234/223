#include <stdio.h>

void Swap(int *a,int *b)
{
    int tmp=*a;
    *a=*b;
    *b=tmp;
}

//三数取中
int GetMid(int *arr,int left,int right)
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
void quick_sort(int *arr,int left,int right)
{
    if(left>=right)
        return;
    int mid=GetMid(arr,left,right);
    Swap(&arr[mid],&arr[left]);
    int prev=left;
    int cur=prev+1;
    int keyi=left;

    while(cur<right)
    {
        if(arr[cur]<arr[keyi])
        {
            ++prev;
            Swap(&arr[prev],&arr[cur]);
        }
        ++cur;
    }
    Swap(&arr[prev],&arr[keyi]);

    quick_sort(arr,left,prev-1);
    quick_sort(arr,prev+1,right);
}


int main()
{
    int arr[]={6,5,3,1,0,6,3,7,7,4,3,4,5};
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