#include "../myOS/userInterface.h"   //interface from kernel

#include "shell.h"
#include "myTask.h"
#include "../myOS/include/taskArrv.h"
#include "userApp.h"
#include "TestCase.h"

void wallClock_hook_main(void){
	int _h, _m, _s;
	char hhmmss[]="hh:mm:ss\0\0\0\0";

	getWallClock(&_h,&_m,&_s);
	sprintf(hhmmss,"%02d:%02d:%02d",_h,_m,_s);
	put_chars(hhmmss,0x7E,24,72);
}

void doSomeTestBefore(void){		
	setWallClock(18,59,59);		//set time 18:59:59
    setWallClockHook(&wallClock_hook_main);
}

void myMain(void)
{
    doSomeTestBefore();

    myPrintf(0x07, "********************************\n");
	myPrintf(0x07, "*         INIT   INIT !        *\n");
	myPrintf(0x07, "********************************\n");

    if(schedulerMode.type == 0 || schedulerMode.type == 1 || schedulerMode.type == 2)
        TEST();
    if(schedulerMode.type==3)
    {
        initShell();
        int shellTask = createTask(startShell);
        addTask(shellTask);
    }
}
