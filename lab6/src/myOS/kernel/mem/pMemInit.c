#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;//可用的内存的起始地址
unsigned long pMemSize;//可用的大小
unsigned long pMemHandler;

void memTest(unsigned long start, unsigned long grainSize){
	/*本函数需要实现！！！*/
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从某一个地址开始的内存是否可用，具体算法就可以用ppt上说的写了读看是否一致。
	注意点两个：
	1、开始的地址要大于1M，需要做一个if判断。
	2、grainsize不能太小，也要做一个if判断
	*/
    if(start<0x100000)
    {
        start=0x100000;
    }
    if(grainSize<2)
    {
        grainSize=2;
    }
    unsigned long addr=start;
    unsigned short data;
    unsigned short *addrHead,*addrTail;
    unsigned short testData1=0xAA55;
    unsigned short testData2=0x55AA;
	int flag=1;
	pMemSize=0;
	pMemStart=start;
	// int i=0;
	while(flag)
	{
		// myPrintk(0x07,"test%d finished!  ",i);
		// i++;
		addrHead=(unsigned short*)addr;
		addrTail=(unsigned short*)(addr+grainSize-2);
		
		data=*addrHead;
		*addrHead=testData1;
		// myPrintk(0x07,"%x %x    ",(*addrHead),testData1);
		if((*addrHead)!=testData1)
		{
			flag=0;
		}
		*addrHead=testData2;
		// myPrintk(0x07,"%x %x\n",(*addrHead),testData2);
		if((*addrHead)!=testData2)
		{
			flag=0;
		}
		*addrHead=data;

		data=*addrTail;
		*addrTail=testData1;
		// myPrintk(0x07,"%x %x    ",(*addrTail),testData1);

		if((*addrTail)!=testData1)
		{
			flag=0;
		}
		*addrTail=testData2;
		// myPrintk(0x07,"%x %x\n",(*addrTail),testData2);
		if((*addrTail)!=testData2)
		{
			flag=0;
		}
		*addrTail=data;

		if(flag)
		{
			addr+=grainSize;
			pMemSize+=grainSize;
		}
	}
	myPrintk(0x7,"MemStart: %x  \n",pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n",pMemSize);
	
}

extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	// myPrintk(0x7,"start init memory\n");
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}

	pMemHandler = dPartitionInit(pMemStart,pMemSize);	
}
