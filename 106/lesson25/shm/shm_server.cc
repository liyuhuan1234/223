#include "comm.hpp"

int main()
{
    key_t k=getKey();
    printf("key:0x%x\n",k);
    int shmid=createShm(k);
    printf("shmid:%d\n",shmid);

    sleep(5);
    char *start=(char*)attchShm(shmid);
    printf("attach success,address start:%p\n",start);

    //使用
    while(true)
    {
        printf("client say: %s\n",start);
        sleep(1);
    }

    //去关联
    detachShm(start);

    //删除共享内存
    delShm(shmid);

    return 0;
}