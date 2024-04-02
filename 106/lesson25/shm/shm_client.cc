#include "comm.hpp"


int main()
{
    key_t k=getKey();
    printf("key:0x%x\n",k);
    int shmid=getShm(k);
    printf("shmid:%d\n",shmid);

    sleep(5);
    char *start=(char*)attchShm(shmid);
    printf("attach success,address start:%p\n",start);

    const char* msg="hello server,我是另一个进程，正在和你通信";
    pid_t id=getpid();
    int cnt=1;
    while(true)
    {
        sleep(1);
        snprintf(start,MAX_SIZE,"%s[pid:%d][消息编号：%d]",msg,id,cnt++);
    }
    detachShm(start);

    return 0;
}