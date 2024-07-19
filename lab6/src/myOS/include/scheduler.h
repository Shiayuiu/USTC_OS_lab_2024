#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include "task.h"
#define FCFS 0
#define RR 1
#define PRIOMODE 2


struct scheduler
{
    unsigned int type;
    struct myTCB *(*nextTask_func)(void);
    void (*enqueueTask_func)(struct myTCB *task);
    void (*dequeueTask_func)(void);
    void (*schedulerInit_func)(void);
    void (*scheduler)(void);
};
extern struct scheduler schedulerMode;
void destroyTask(int tid);

void schedule(void);
void setSysScheduler(unsigned int what);
void scheduleModeInit(void);
#endif
