#include "../include/vsprintf.h"
#include "../include/vga.h"
#include "../include/kmalloc.h"

int hh=0,mm=0,ss=0,ms=0;

void setWallClock(int h, int m, int s){
        if ( (h<0) || (h>24) ) hh=0;
        else hh = h;
        if ( (m<0) || (h>60) ) mm=0;
        else mm = m;
        if ( (s<0) || (s>60) ) ss=0;
        else ss = s;
}

void getWallClock(int *h, int *m, int *s){
        *h = hh;
        *m = mm;
        *s = ss;
}

void (*wallClock_hook)(void) = 0;
void setWallClockHook(void (*func)(void)) {
	wallClock_hook = func;
}
void getTimeStamp(unsigned char *buffer){
    sprintf(buffer,"%02d:%02d:%02d",hh,mm,ss);
}
void oneTickUpdateWallClock(void){
	ms += 10;  // ?100HZ?
    if (ms>=1000) {ms=0;ss++;}
    if (ss>=60) {ss=0;mm++;}
    if (mm>=60) {mm=0;hh++;}
    if (hh>=24) hh=0;
    if(ms==0)
    {
        unsigned char * buffer=(unsigned char*) kmalloc(30);
        getTimeStamp(buffer);
        put_chars(buffer,0x2,24,70);
    }

	if (wallClock_hook) wallClock_hook();
}

//TODO: buffer的长度：至少10个字节

