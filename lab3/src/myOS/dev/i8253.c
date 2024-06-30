#include "io.h"
#define CONST_FREQ 1193182
#define CONST_FREQ_INTERRUPT 100
unsigned short CONST_FREQ_DIV = CONST_FREQ / CONST_FREQ_INTERRUPT;
void init8253(void){
	//TODO 你需要填写它
	//init
	outb(0x43, 0x34);
	outb(0x40, (unsigned char)CONST_FREQ_DIV);
	outb(0x40, (unsigned char)(CONST_FREQ_DIV >> 8));

	outb(0x21, inb(0x21) & 0xFE);
}
