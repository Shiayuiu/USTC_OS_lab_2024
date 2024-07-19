#include "../include/scheduler.h"
#include "../include/myPrintk.h"
struct scheduler schedulerMode;
struct myTCB* firstFreeTCB;
unsigned long **prevTSK_StackPtrAddr;
unsigned long *nextTSK_StackPtr;
unsigned long *BspContextBase;
unsigned long *BspContext;

void setSysScheduler(unsigned int what) {
    schedulerMode.type=what;
}

void scheduleModeInit(void) {
    // 注意函数接口复用
    switch (schedulerMode.type) {
        case FCFS:
            schedulerMode.schedulerInit_func=rqFCFSInit;
            schedulerMode.nextTask_func=nextTask_FCFS;
            schedulerMode.enqueueTask_func=taskEnqueue_FCFS;
            schedulerMode.dequeueTask_func=taskDequeue_FCFS;
            schedulerMode.scheduler=schedule_FCFS;
            break;
        case RR:
            schedulerMode.schedulerInit_func=initT_RR;
            schedulerMode.nextTask_func=nextTask_RR;
            schedulerMode.enqueueTask_func=taskEnqueue_FCFS;
            schedulerMode.dequeueTask_func=taskDequeue_FCFS;
            schedulerMode.scheduler=schedule_RR;
            break;
        case PRIOMODE:
            schedulerMode.schedulerInit_func=rqFCFSInit;
            schedulerMode.nextTask_func=nextTask_FCFS;
            schedulerMode.enqueueTask_func=taskEnqueue_PRIO;
            schedulerMode.dequeueTask_func=taskDequeue_FCFS;
            schedulerMode.scheduler=schedule_FCFS;
            break;
        case 3:
            schedulerMode.schedulerInit_func=rqFCFSInit;
            schedulerMode.nextTask_func=nextTask_FCFS;
            schedulerMode.enqueueTask_func=taskEnqueue_FCFS;
            schedulerMode.dequeueTask_func=taskDequeue_FCFS;
            schedulerMode.scheduler=schedule_FCFS;
            break;
    }
}
