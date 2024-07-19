#ifndef TASKARRV_H
#define TASKARRV_H
#include "myPrintk.h"
#include "wallClock.h"
#include "task.h"
extern int arrvLength;
//extern myTCB *arrvData[20];
struct arrvNode
{
    struct myTCB* TCB;
    struct arrvNode* next;
};

struct arrv
{
    struct arrvNode* head;
    struct arrvNode* tail;
};
extern struct arrv arrvData;
void addTask(int tid);
void arrvHook();
void arrvInit();
void arrvEnqueue(struct myTCB* task);
int arrvEmpty();
void arrvDequeue();
#endif
