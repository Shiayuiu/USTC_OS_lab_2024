#include "TestCase.h"
char *message1 = "********************************\n";

void myTskFCFS1(void)
{
    taskExecute();
}

void myTskFCFS2(void)
{
    taskExecute();
}

void myTskFCFS3(void)
{
    taskExecute();
}

void myTskFCFS4(void)
{
    taskExecute();
}

void TEST(void)
{

    int newTskTid1 = createTask(myTskFCFS1); // its tid will be 1
    setTaskPara(PRIO, 3, tcbPool[newTskTid1].taskPara);
    setTaskPara(ARRTIME, 0, tcbPool[newTskTid1].taskPara);
    setTaskPara(EXETIME, 3, tcbPool[newTskTid1].taskPara);

    int newTskTid2 = createTask(myTskFCFS2); // its tid will be 2
    setTaskPara(PRIO, 1, tcbPool[newTskTid2].taskPara);
    setTaskPara(ARRTIME, 1, tcbPool[newTskTid2].taskPara);
    setTaskPara(EXETIME, 2, tcbPool[newTskTid2].taskPara);

    int newTskTid3 = createTask(myTskFCFS3); // its tid will be 3
    setTaskPara(PRIO, 0, tcbPool[newTskTid3].taskPara);
    setTaskPara(ARRTIME, 3, tcbPool[newTskTid3].taskPara);
    setTaskPara(EXETIME, 3, tcbPool[newTskTid3].taskPara);

    int newTskTid4 = createTask(myTskFCFS4); // its tid will be 4
    setTaskPara(PRIO, 4, tcbPool[newTskTid4].taskPara);
    setTaskPara(ARRTIME, 2, tcbPool[newTskTid4].taskPara);
    setTaskPara(EXETIME, 3, tcbPool[newTskTid4].taskPara);

    addTask(newTskTid1);
    addTask(newTskTid2);
    addTask(newTskTid3);
    addTask(newTskTid4);
}
