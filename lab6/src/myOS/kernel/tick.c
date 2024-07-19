#include "../include/wallClock.h"
extern void oneTickUpdateWallClock(void);       //TODO: to be generalized

//#ifndef __TICK_HOOK__
//#define __TICK_HOOK__
void (*hook_list[30])(void); //{};
//#endif

int tick_number = 0;
int hook_func_num=0;
void append2HookList(void (*func)(void))
{
    hook_list[hook_func_num++]=func;
}
void tick(void){
	int temp_col, temp_row;	
	tick_number++;	
//	myPrintk(0x4,"tick::%d\n",tick_number);

	oneTickUpdateWallClock();

	/* call hooks 
	scheduler_tick();  // RR
	tick_hook_arr();  // arriving	

	if(watchdogTimer_func) watchdogTimer_func(); 
    */
//	if(tick_hook) tick_hook();  //user defined
    for(int i=0;i<hook_func_num;i++)
    {
        hook_list[i]();
    }
}
unsigned int getTickTime()
{
    return tick_number;
}