#include "../include/task.h"
#include "../include/myPrintk.h"
#include "../include/scheduler.h"

// unsigned long pMemHandler;

myTCB tcbPool[TASK_NUM];//进程池的大小设置

myTCB* idleTsk;                /* idle 任务 */
myTCB* currentTask;             /* 当前任务 */
myTCB* firstFreeTsk;           /* 下一个空闲的 TCB */

#define TSK_RDY 0        //表示当前进程已经进入就绪队列中
#define TSK_WAIT -1      //表示当前进程还未进入就绪队列中
#define TSK_RUNING 1     //表示当前进程正在运行
#define TSK_NONE 2       //表示进程池中的TCB为空未进行分配

rdyQueueFCFS rqFCFS;    // 就绪队列，本代码中通用

void rqFCFSInit() {
    // 初始化就绪队列
    rqFCFS.head=NULL;
    rqFCFS.tail=NULL;
}

int rqFCFSIsEmpty() {
    // 检查就绪队列是否为空
    if((rqFCFS.head==NULL)&&(rqFCFS.tail==NULL))
        return 1;
    return 0;
}


void taskEnqueue_FCFS(struct myTCB *task) {
    // 将任务按照 FCFS 调度算法，加入就绪队列
    if(rqFCFSIsEmpty()) {
        rqFCFS.head = task;
        rqFCFS.tail = task;
    }
    else {
        rqFCFS.tail->nextTCB = task;
        rqFCFS.tail = task;
    }
    task->TSK_State = TSK_RDY;
    rqFCFS.tail->nextTCB = NULL;
}

void taskEnqueue_PRIO(myTCB *task) {
    // TODO: 实现按照优先级调度算法，加入就绪队列
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
void taskDequeue_FCFS() {
    // TODO:实现出队操作，调度方式为FCFS 
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

void initTaskPara(struct taskPara *buffer) {
    // 实现初始化任务参数操作
    buffer->priority=0;
    buffer->arrTime=0;
    buffer->exetime=0;
}

void setTaskPara(unsigned int tag,unsigned int value,struct taskPara* buffer) {
    // TODO:实现设置任务参数操作
    switch (tag) {
        case PRIO:
            // TODO buffer->? = ?
            buffer->priority = value;
            break;
        case ARRTIME:
            // TODO
            buffer->arrTime = value;
            break;
        case EXETIME:
            // TODO
            buffer->exetime = value;
            break;
    }
}
unsigned int getTaskPara(int tag,struct taskPara *para) {
    // TODO:实现获取任务参数操作
    switch (tag) {
        case PRIO:
            // TODO
            return para->priority;
            break;
        case ARRTIME:
            // TODO
            return para->arrTime;
            break;
        case EXETIME:
            // TODO
            return para->exetime;
            break;
        default:
            return 0;
    }
}

void taskExecute() {
    // 执行当前任务直到完成
    while (currentTask->runTime< getTaskPara(EXETIME,currentTask->taskPara));
};

void taskEnd() {
    // 结束当前任务并进行上下文切换
    context_switch(&currentTask->stackTop,BspContext);
}


void stack_init(unsigned long **stk, void (*task)(void)) {
    // 初始化任务栈
    *(*stk)-- = (unsigned long)taskEnd; // eip
    *(*stk)-- = (unsigned long)task; // eip
    *(*stk)-- = (unsigned long)0x0202; // flag registers
    *(*stk)-- = (unsigned long)0xAAAAAAAA; // eax
    *(*stk)-- = (unsigned long)0xCCCCCCCC; // ecx
    *(*stk)-- = (unsigned long)0xDDDDDDDD; // edx
    *(*stk)-- = (unsigned long)0xBBBBBBBB; // ebx
    *(*stk)-- = (unsigned long)0x44444444; // esp
    *(*stk)-- = (unsigned long)0x55555555; // ebp
    *(*stk)-- = (unsigned long)0x66666666; // esi
    *(*stk) = (unsigned long)0x77777777; // edi

}

int createTask(void(*taskBody)(void)) {
    // 以 tskBody 创建一个新任务
    if(!firstFreeTsk)
        return -1;

    struct myTCB *newTask=firstFreeTsk;
    firstFreeTsk=firstFreeTsk->nextTCB;
    newTask->stackMax=(unsigned long *) kmalloc(STACK_SIZE);

    if(!(newTask->stackMax))
        return -1;

    newTask->stackTop=STACK_SIZE+newTask->stackMax-1;
    initTaskPara(newTask->taskPara);
    stack_init(&newTask->stackTop,taskBody);
    return newTask->TSK_ID;
}

void taskStart(int tid) {
    // 启动指定 ID 的任务
    tcbPool[tid].TSK_State=TSK_RDY;
    schedulerMode.enqueueTask_func(& tcbPool[tid]);
}

void destroyTask(int tid) {
    // 销毁指定 ID 的任务
    kfree((unsigned long)tcbPool[tid].stackMax);
    kfree((unsigned long)tcbPool[tid].taskPara);
    tcbPool[tid].task_entrance=NULL;
    tcbPool[tid].TSK_State=TSK_NONE;
    tcbPool[tid].runTime=0;
    tcbPool[tid].thisRunTime=0;
    tcbPool[tid].stackMax=NULL;
    tcbPool[tid].stackTop=NULL;
    tcbPool[tid].nextTCB = firstFreeTsk;
    firstFreeTsk = & tcbPool[tid];
}

void startMultiTask() {
    // 启动多任务调度
    BspContextBase=(unsigned long *) kmalloc(10*STACK_SIZE);
    BspContext=BspContextBase+STACK_SIZE-1;
    currentTask=NULL;
    schedulerMode.scheduler();
}

void idleTaskBody() {
    // 空闲函数，输出示意
    myPrintk(0x1,"******************************\n");
    myPrintk(0x1,"*          Idle Task         *\n");
    myPrintk(0x1,"*  Waiting for scheduling... *\n");
    myPrintk(0x1,"******************************\n");
}

void TaskManagerInit() {
    // 多任务管理初始化
    struct myTCB* prevTCB=NULL;
    for(int i=0;i<TASK_NUM;i++)
    {
        tcbPool[i].taskPara=(struct taskPara*) kmalloc(sizeof (struct taskPara));
        tcbPool[i].TSK_ID=i;
        tcbPool[i].runTime=0;
        tcbPool[i].thisRunTime=0;
        tcbPool[i].task_entrance=NULL;
        tcbPool[i].TSK_State=TSK_NONE;
        tcbPool[i].stackMax=NULL;
        tcbPool[i].stackTop=NULL;
        tcbPool[i].nextTCB=NULL;
        if(prevTCB)
        {
            prevTCB->nextTCB = & tcbPool[i];
        }
        prevTCB = &tcbPool[i];
    }
    firstFreeTsk= & tcbPool[0];
    scheduleModeInit(); // 初始化调度模式
    arrvInit(); // 初始化任务到达队列
    append2HookList(addRunTime); // 添加任务运行时间
    append2HookList(printCurrentTask); // 添加打印当前任务信息
    schedulerMode.schedulerInit_func(); // 初始化调度器
    int initTid= createTask(myMain); // 创建主任务
    taskStart(initTid); // 启动主任务
    startMultiTask(); // 启动多任务
}

void addRunTime() {
    // TODO：增加当前任务的运行时间
    if((currentTask!=NULL)&&(getTickTime()%100==0))
        currentTask->runTime++;
}

void printCurrentTask() {
    // 打印当前任务信息
    if((getTickTime()%100==0)&&(currentTask!=NULL)&&(schedulerMode.type!=3)) {
        myPrintk(0x07,"***********************************\n");
        myPrintk(0x07,"* current task tid : %-2d         *\n", currentTask->TSK_ID);
        myPrintk(0x07,"* priority         : %-2d         *\n", getTaskPara(PRIO,currentTask->taskPara));
        myPrintk(0x07,"* arrive time      : %-2d         *\n", getTaskPara(ARRTIME,currentTask->taskPara));
        myPrintk(0x07,"* execute time     : %-2d / %2d    *\n",currentTask->runTime, getTaskPara(EXETIME,currentTask->taskPara));
        myPrintk(0x07,"* thisRuntime      : %-2d         *\n", currentTask->thisRunTime+1);
        myPrintk(0x07,"* Runtime          : %-2d         *\n", currentTask->runTime);
        myPrintk(0x07,"***********************************\n");
    }
}

void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk) {
    // 进行上下文切换，
    prevTSK_StackPtrAddr = prevTskStkAddr;
    nextTSK_StackPtr = nextTskStk;
    CTX_SW();
}


struct myTCB* nextTask_FCFS() {
    // TODO: 获取下一个 FCFS 调度的任务
    if(rqFCFSIsEmpty()){
        return NULL;
    }
    else {
        return rqFCFS.head;
    } 
}

void schedule_FCFS(void){
    // FCFS 调度算法
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

struct myTCB* nextTask_RR() {
    // TODO: 以 RR 调度算法，获取下一个任务
    if(rqFCFSIsEmpty()){
        return NULL;
    }
    else {
        return rqFCFS.head;
    } 
}

void RR_hook() {
    // TODO：RR 调度的 HOOK 函数
    if((currentTask!=NULL)&&(getTickTime()%100 == 0)){
        currentTask->thisRunTime++;
    }
    if((currentTask->thisRunTime >=2)&&(getTickTime()%100 == 0)){
        currentTask->thisRunTime = 0;
        taskEnqueue_FCFS(currentTask);
        taskEnd();
    }
}

void initT_RR() {
    // 初始化RR调度所需的资源
    rqFCFSInit();
    append2HookList(RR_hook);
}

void schedule_RR(void){
    // TODO: RR调度算法
    while(1) {
        currentTask=NULL;
        struct myTCB* nextTask;
        nextTask=nextTask_RR();
        if(nextTask) {
            taskDequeue_FCFS();
            currentTask=nextTask;
            context_switch(&BspContext,currentTask->stackTop);
        }
    }
}