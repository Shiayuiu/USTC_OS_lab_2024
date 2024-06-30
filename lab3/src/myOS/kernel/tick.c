#include "wallClock.h"
int system_ticks=0;
int HH=12,MM=34,SS=56;

void tick(void){
	system_ticks++;
	//update wallclock
	if(system_ticks == 100){
		system_ticks = 0;
		SS = ((SS+1)==60)?0:SS+1;
		if(!SS) MM = ((MM+1)==60)?0:MM+1;
		if(!SS & !MM) HH = ((HH+1)==24)?0:HH+1;
	}
	setWallClock(HH, MM, SS);
	return;
}

