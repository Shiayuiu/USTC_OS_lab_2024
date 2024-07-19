#ifndef __TASK_H__
#define __TASK_H__

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#include "myPrintk.h"
#include "kmalloc.h"
#include "wallClock.h"
#include "scheduler.h"
#endif

#define NULL 0
#define PRIO 0
#define ARRTIME 1
#define EXETIME 2

#define STACK_SIZE 0x2000
#define TASK_NUM 10

typedef struct taskPara
{
    unsigned int priority;
    unsigned int arrTime;
    unsigned int exetime;
} taskPara;

typedef struct myTCB
{
    unsigned long *stackTop;
    unsigned long TSK_ID;
    unsigned long TSK_State;
    unsigned long runTime;
    unsigned long thisRunTime;
    unsigned long *stackMax;
    void (*task_entrance)(void);
    taskPara* taskPara;
    struct myTCB *nextTCB;
} myTCB;

typedef struct rdyQueueFCFS
{
    myTCB* head;
    myTCB* tail;
    myTCB * idleTask;
} rdyQueueFCFS;


extern myTCB tcbPool[TASK_NUM];//进程池的大小设置

extern myTCB* idleTsk;                /* idle 任务 */
extern myTCB* currentTask;             /* 当前任务 */
extern myTCB* firstFreeTsk;           /* 下一个空闲的 TCB */
extern rdyQueueFCFS rqFCFS;

void initTaskPara(struct taskPara*buffer);
void setTaskPara(unsigned int tag,unsigned int value,struct taskPara* buffer);
unsigned int getTaskPara(int tag,struct taskPara *para);
void rqFCFSInit();
int rqFCFSIsEmpty();
void taskEnqueue_FCFS(struct myTCB *task);
void taskEnqueue_PRIO(struct myTCB *task);
void taskDequeue_FCFS();

void myMain(void);
extern unsigned int getTickTime();
extern struct myTCB* firstFreeTCB;
void destroyTask(int TSK_ID);
void arrvInit();

void TaskManagerInit();
int createTask(void(*taskBody)(void));
void taskStart(int TSK_ID);
void addRunTime();
void taskExecute();
void printCurrentTask();

extern myTCB* firstFreeTCB;
extern unsigned long **prevTSK_StackPtrAddr;
extern unsigned long *nextTSK_StackPtr;
extern unsigned long *BspContextBase;
extern unsigned long *BspContext;
void CTX_SW();
void context_switch(unsigned long **prevTskStkAddr, unsigned long *nextTskStk);
void schedule_FCFS(void);
struct myTCB* nextTask_FCFS();
struct myTCB* nextTask_RR();
void destroyTask(int tid);
int createTask(void(*taskBody)(void));
void RR_hook();
void idleTaskBody();
void initT_RR();

void schedule_RR(void);

#endif
