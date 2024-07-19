#include "include/i8253.h"
#include "include/i8259.h"
#include "include/irq.h"
#include "include/uart.h"
#include "include/vga.h"
#include "include/mem.h"
#include "include/myPrintk.h"
#include "include/task.h"
#include "include/wallClock.h"

extern void myMain(void);		//TODO: to be generalized

void pressAnyKeyToStart(void){
	myPrintk(0x07,"Press any key to continue\n");
    uart_get_char();
    myPrintk(0x07,"press 0,1,2,3 to choose FCFS,RR,PRIO to run the os or start the shell\n");
    setSysScheduler((unsigned int)uart_get_char()-'0');
    myPrintk(0x07,"%d",schedulerMode.type);
    uart_get_char();
    myPrintk(0x07,"\n");
}

void osStart(void){
	clear_screen();
	pressAnyKeyToStart(); // prepare for uart device
	init8259A();
	init8253();
	enable_interrupt();

	clear_screen();

	pMemInit();  

	{
		unsigned long tmp = dPartitionAlloc(pMemHandler,100);
		dPartitionWalkByAddr(pMemHandler);
		dPartitionFree(pMemHandler,tmp);
		dPartitionWalkByAddr(pMemHandler);
	}	

	// tick_number=0;
	TaskManagerInit();

	while(1);
}
