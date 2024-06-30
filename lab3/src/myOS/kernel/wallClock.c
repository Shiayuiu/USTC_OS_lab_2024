#include "vga.h"
#include "tick.h"
void setWallClock(int HH,int MM,int SS){
	//TODO 你需要填写它
	char DISPLAYED_CLOCK[12];

	DISPLAYED_CLOCK[0] = '0' + HH/10;
	DISPLAYED_CLOCK[1] = '0' + HH%10;
	DISPLAYED_CLOCK[2] = ' ';
	DISPLAYED_CLOCK[3] = ':';
	DISPLAYED_CLOCK[4] = ' ';
	DISPLAYED_CLOCK[5] = '0' + MM/10;
	DISPLAYED_CLOCK[6] = '0' + MM%10;
	DISPLAYED_CLOCK[7] = ' ';
	DISPLAYED_CLOCK[8] = ':';
	DISPLAYED_CLOCK[9] = ' ';
	DISPLAYED_CLOCK[10]= '0' + SS/10;
	DISPLAYED_CLOCK[11]= '0' + SS%10;

	put_chars(DISPLAYED_CLOCK, 0xf, VGA_COL - 1 , VGA_ROW -12);
}

void getWallClock(int *HH,int *MM,int *SS){
	//TODO 你需要填写它
	*HH = HH;
	*MM = MM;
	*SS = SS;
}