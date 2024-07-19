#include "../include/taskArrv.h"

int arrvLength;
struct arrv arrvData;
void arrvHook() {
    // 作用：检查是否有任务到达，如果有则将其加入就绪队列
    if(getTickTime()%100==0)
    {
        if(arrvEmpty())
        {
            return;
        }
        while((arrvData.head!=NULL)&&((arrvData.head->TCB->taskPara->arrTime)<=(getTickTime()/100)))
        {
            taskStart(arrvData.head->TCB->TSK_ID);
            arrvDequeue();
        }
    }

}
void arrvInit() {
    // 作用：初始化任务到达队列
    arrvData.head=NULL;
    arrvData.tail=NULL;
    append2HookList(arrvHook);
}

int arrvEmpty() {
    // 作用：检查任务到达队列是否为空
    if((arrvData.head==NULL)&&(arrvData.tail==NULL))
    {
        return 1;
    }
    return 0;
}


void arrvEnqueue(struct myTCB* task) {
    // 作用：将任务加入任务到达队列
    struct arrvNode* newNode=(struct arrvNode*)kmalloc(sizeof(struct arrvNode));
    if(!newNode)
    {
        return;
    }
    newNode->TCB=task;
    if(arrvEmpty())
    {
        arrvData.head=newNode;
        arrvData.tail=newNode;
    }
    else
    {
        if((newNode->TCB->taskPara->arrTime)<(arrvData.head->TCB->taskPara->arrTime))
        {
            newNode->next=arrvData.head;
            arrvData.head=newNode;
        }
        else
        {
            struct arrvNode* tmpNode=arrvData.head;
            while((tmpNode->next!=NULL)&&((newNode->TCB->taskPara->arrTime)>=(tmpNode->next->TCB->taskPara->arrTime)))
            {
                tmpNode=tmpNode->next;
            }
            newNode->next=tmpNode->next;
            tmpNode->next=newNode;
        }
    }
    while(arrvData.tail->next!=NULL)
    {
        arrvData.tail=arrvData.tail->next;
    }
}
void arrvDequeue() {
    // 作用：将任务从任务到达队列中取出
    if(arrvEmpty())
    {
        return;
    }
    else
    {
        struct arrvNode* tmpNode=arrvData.head;
        if((arrvData.head)==(arrvData.tail))
        {
            arrvData.head=NULL;
            arrvData.tail=NULL;
        }
        else
        {
            arrvData.head=arrvData.head->next;
        }
        kfree((unsigned long)tmpNode);
    }
}
void addTask(int tid) {
    //myPrintk(0x7,"here");//调试用
    if(tcbPool[tid].taskPara->arrTime==(getTickTime()/100))
    {
        taskStart(tid);
    }
    else
    {
        arrvEnqueue(& tcbPool[tid]);
    }
}