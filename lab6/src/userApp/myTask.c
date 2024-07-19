#include "shell.h"
#include "memTestCase.h"
#include "../myOS/userInterface.h"
#include "myTask.h"
void myTask0()
{
    myPrintk(0x1,"******************************\n");
    myPrintk(0x1,"*          New Task 0        *\n");
    myPrintk(0x1,"*         Hello World!       *\n");
    myPrintk(0x1,"******************************\n");
}

void myTask1()
{
    myPrintk(0x2,"******************************\n");
    myPrintk(0x2,"*          New Task 1        *\n");
    myPrintk(0x2,"*         Hello World!       *\n");
    myPrintk(0x2,"******************************\n");
}

void myTask2()
{
    myPrintk(0x3,"******************************\n");
    myPrintk(0x3,"*          New Task 2        *\n");
    myPrintk(0x3,"*         Hello World!       *\n");
    myPrintk(0x3,"******************************\n");
}


void myTask3()
{
    myPrintk(0x5,"******************************\n");
    myPrintk(0x5,"*          Shell Task        *\n");
    myPrintk(0x5,"*       starting shell...    *\n");
    myPrintk(0x5,"******************************\n");

    initShell();
    memTestCaseInit();
    startShell();
}