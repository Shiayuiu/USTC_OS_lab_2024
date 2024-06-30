# 实验6 调度算法

## 原理说明

上面的图片来自于老师给出的实验PPT。可以看出，本次实验的主流程为：由Multiboot_header引导程序启动->osStart->初始化时钟并开中断->进入用户程序运行，实现各测试用tsk和Shell。

## 主要功能模块实现

我的实验6基于助教给出的框架代码做了一定修改完成。

**1.FCFS调度相关**

```c
void taskDequeue_FCFS() {
    // 实现出队操作，调度方式为FCFS 
    if(rqFCFSIsEmpty()){
        return;
    }
    if(rqFCFS.head == rqFCFS.tail){
        rqFCFS.head->TSK_State = TSK_WAIT;
        rqFCFS.head = rqFCFS.tail = NULL;
    }
    else {
        myTCB* tmp = rqFCFS.head;
        rqFCFS.head->TSK_State = TSK_WAIT;
        rqFCFS.head = tmp->nextTCB;
        kfree((unsigned long)tmp);
    }
}
struct myTCB* nextTask_FCFS() {
    // 获取下一个 FCFS 调度的任务
    if(rqFCFSIsEmpty()){
        return NULL;
    }
    else {
        return rqFCFS.head;
    } 
}
```

这两个函数的实现于上一个实验中已经说明过，这里不再赘述。

**2.RR调度相关**

```c
struct myTCB* nextTask_RR() {
    // 以 RR 调度算法，获取下一个任务
    if(rqFCFSIsEmpty()){
        return NULL;
    }
    else {
        return rqFCFS.head;
    } 
}
void schedule_RR(void){
    // RR调度算法
    while(1) {
        currentTask=NULL;
        struct myTCB* nextTask;
        nextTask=nextTask_FCFS();
        if(nextTask) {
            taskDequeue_FCFS();
            currentTask=nextTask;
            context_switch(&BspContext,currentTask->stackTop);
        }
    }
}
```

RR调度的nextTask()和schedule()和FCFS在逻辑上是一致的，最为核心的部分是RR_hook()函数。RR_hook()函数会维护currentTask的thisRuntime值并适时的进行入队出队的维护，同时提供抢占调度的算法。

**3.优先级调度相关**

```c
void taskEnqueue_PRIO(myTCB *task) {
    // 实现按照优先级调度算法，加入就绪队列
    unsigned int priority = task->taskPara->priority;
    if(rqFCFSIsEmpty()){
        rqFCFS.head = task;
        rqFCFS.tail = task;
        rqFCFS.tail->nextTCB = NULL;
    }
    else if(rqFCFS.head == rqFCFS.tail){
        if(priority < rqFCFS.head->taskPara->priority){
            task->nextTCB = rqFCFS.head;
            rqFCFS.head = task;
        }
        else {
            rqFCFS.tail->nextTCB = task;
            rqFCFS.tail = task;
            rqFCFS.tail->nextTCB = NULL;
        }
    }
    else {
        myTCB* prev = rqFCFS.head;
        myTCB* curr = rqFCFS.head;
        if(priority < rqFCFS.head->taskPara->priority){
            task->nextTCB = rqFCFS.head;
            rqFCFS.head = task;
            return;
        }
        while (curr->nextTCB)
        {
            if(priority > curr->taskPara->priority){
                prev = curr;
                curr = curr->nextTCB;
            }
            else {
                task->nextTCB = curr;
                prev->nextTCB = task;
            }
        }   
        rqFCFS.tail->nextTCB = task;
        rqFCFS.tail = task;
        rqFCFS.tail->nextTCB = NULL;
    }
}
```

实际上这个过程是向有序链表中插入有特定值的结点的过程，即链表的按序插入。需要考虑队列为空，只有一个元素，有多个元素的情况；对于有多个元素的情况，只需要按照最常见的双指针实现链表的插入排序即可实现。

4.任务参数相关

```c
void setTaskPara(unsigned int tag,unsigned int value,struct taskPara* buffer) {
    // 实现设置任务参数操作
    switch (tag) {
        case PRIO:
            buffer->priority = value;
            break;
        case ARRTIME:
            buffer->arrTime = value;
            break;
        case EXETIME:
            buffer->exetime = value;
            break;
    }
}
unsigned int getTaskPara(int tag,struct taskPara *para) {
    switch (tag) {
        case PRIO:
            return para->priority;
            break;
        case ARRTIME:
            return para->arrTime;
            break;
        case EXETIME:
            return para->exetime;
            break;
        default:
            return 0;
    }
}
void addRunTime() {
    // 增加当前任务的运行时间
    if((currentTask!=NULL)&&(getTickTime()%100==0))
        currentTask->runTime++;
}
```



## 运行结果及说明

在wsl的Ubuntu环境下运行，得到的结果如下：

1.FCFS调度

2.RR调度

3.优先级调度

经比对可知，运行结果是正确的。
