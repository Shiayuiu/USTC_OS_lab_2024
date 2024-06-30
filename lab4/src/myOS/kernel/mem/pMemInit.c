#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;  // 可用的内存的起始地址
unsigned long pMemSize;  // 可用的大小
unsigned long pMemHandler;
void memTest(unsigned long start, unsigned long grainSize){
	// TODO
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从start开始有多大的内存可用，具体算法参照ppt检测算法的文字描述
	注意点三个：
	1、覆盖写入和读出就是往指针指向的位置写和读，不要想复杂。
	  (至于为什么这种检测内存的方法可行大家要自己想一下)
	2、开始的地址要大于1M，需要做一个if判断。
	3、grainsize不能太小，也要做一个if判断
	
	*/

	//initialize
	if(start < 0x100000){
		start = 0x100000;	//最小起始地址
	}
	if(grainSize < 2){
		grainSize = 2;		//最小步长
	}
	pMemStart = start;
	pMemSize  = 0;
	unsigned long  test_addr = start;	//标识当前检测的内存地址
	unsigned short *test_h, *test_t;	//标识检测区域的头尾各两个字节
	unsigned short data_buf;			//缓存内存中预先可能存放的数据
	unsigned short const1 = 0xAA55;
	unsigned short const2 = 0x55AA;
	while (1)
	{
		
		test_h = (unsigned short*)test_addr;
		test_t = (unsigned short*)(test_addr +  grainSize - 2);
		
		data_buf = *test_h;
		*test_h = const1;
		if(*test_h != const1){
			break;
		}
		*test_h = const2;
		if(*test_h != const2){
			break;
		}
		*test_h = data_buf; 

		data_buf = *test_t;
		*test_t = const1;
		if(*test_t != const1){
			break;
		}
		*test_t = const2;
		if(*test_t != const2){
			break;
		}
		*test_t = data_buf;

		test_addr = test_addr + grainSize;
		pMemSize = pMemSize + grainSize;
	}


	//需要返回可用内存块的起始地址和大小
	myPrintk(0x7,"MemStart: %x  \n", pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n", pMemSize );
	
}

extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	
	// 此处选择不同的内存管理算法
	pMemHandler = dPartitionInit(pMemStart,pMemSize);	
}
