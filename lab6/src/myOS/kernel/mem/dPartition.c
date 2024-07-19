#include "../../include/myPrintk.h"

//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
//	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
    myPrintk(0x5,"dPartition(start=%u, size=%u, firstFreeStart=%u)\n", dp, dp->size,dp->firstFreeStart);

}


// EMB每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	unsigned long nextStart;    // if free: pointer to next block
} EMB;	//共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB * emb){
//	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
    myPrintk(0x3,"EMB(start=%u, size=%u, nextStart=%u)\n", emb, emb->size, emb->nextStart);
}

unsigned long align8(unsigned long a)
{
	return ((a-1)/8+1)*8;
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	//本函数需要实现！！！
	/*功能：初始化内存。
	1.在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2.然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3.返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	
	*/
	if(totalSize<=(sizeof(dPartition)+sizeof(EMB)))
	{
		return 0;
	}

	dPartition* handler=(dPartition*)start;
	handler->size=totalSize-sizeof(dPartition);
	handler->firstFreeStart=start+sizeof(dPartition);

	EMB* block=(EMB*)(handler->firstFreeStart);
	block->size=(handler->size)-sizeof(EMB);
	block->nextStart=0;

	return start;
}

void dPartitionWalkByAddr(unsigned long dp){
	//本函数需要实现！！！
	/*功能：本函数遍历输出EMB 方便调试
	1.先打印dP的信息，可调用上面的showdPartition。
	2.然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息

	*/
	
	dPartition* handler=(dPartition*)dp;
	showdPartition(handler);

	unsigned long addr=handler->firstFreeStart;
	EMB* block;
	while(addr)
	{
		block=(EMB*)addr;
		showEMB(block);
		addr=block->nextStart;
	}
}

//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	//本函数需要实现！！！
	/*功能：分配一个空间
	1.使用firstfit的算法分配空间，当然也可以使用其他fit，不限制。
	2.成功分配返回首地址，不成功返回0
	3.从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。

	*/
	// myPrintk(0x07,"%d \n",size);
	if (size<=0)
	{
		return 0;
	}
	
	dPartition* handler=(dPartition*)dp;
	// dPartitionWalkByAddr(dp);
	// myPrintk(0x07,"%d\n",handler->firstFreeStart);
	if(!(handler->firstFreeStart))
	{
		return 0;
	}

	unsigned long sizeAlign=align8(size);
	unsigned long addr=handler->firstFreeStart;
	unsigned long preAddr=0;
	EMB* block;
	EMB* preBlock;
	while(addr)
	{
		block=(EMB*)addr;
		preBlock=(EMB*)preAddr;
		if(((block->size)>=sizeAlign)&&((block->size)<=(sizeAlign+sizeof(EMB))))
		{
			if(preAddr==0)
			{
				handler->firstFreeStart=block->nextStart;
			}
			else
			{
				preBlock->nextStart=block->nextStart;
			}
			return addr+sizeof(EMB);
		}
		if ((block->size)>(sizeAlign+sizeof(EMB)))
		{
			unsigned long newAddr=addr+sizeof(EMB)+sizeAlign;
			EMB* newBlock=(EMB*)newAddr;
			newBlock->size=block->size-sizeAlign-sizeof(EMB);
			newBlock->nextStart=block->nextStart;
			block->size=block->size-newBlock->size- sizeof(EMB);
			if (preAddr==0)
			{
				handler->firstFreeStart=newAddr;
			}
			else
			{
				preBlock->nextStart=newAddr;
			}
			return addr+sizeof(EMB);
		}
		preAddr=addr;
		addr=block->nextStart;
	}
	return 0;
}

/*
 *r
 */
unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	//本函数需要实现！！！
	/*功能：释放一个空间
	1.按照对应的fit的算法释放空间
	2.注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3.需要考虑两个空闲且相邻的内存块的合并
	*/
	// dPartitionWalkByAddr(dp);

	start=start-sizeof(EMB);
	// start=start-sizeof(EMB);

	if(start<dp+sizeof(dPartition))
	{
		return 0;
	}
	dPartition* handler=(dPartition*)dp;
//	showdPartition(handler);

	if(start>=dp+handler->size)
	{
		return 0;
	}

	unsigned long addr=handler->firstFreeStart;
	unsigned long preAddr=0;
	EMB* block;
	while(addr)
	{
		block=(EMB*)addr;
		if(addr<start)
		{
			preAddr=addr;
		}
		else
		{
			break;
		}
		addr=block->nextStart;
	}

	block=(EMB*)start;
	if(addr)
	{
		if(addr==(start+block->size+sizeof(EMB)))
		{
			EMB* nextBlock=(EMB*)addr;
			block->size=block->size+nextBlock->size+sizeof(EMB);
			block->nextStart=nextBlock->nextStart;
		}
		else
		{
			block->nextStart=addr;
		}
	}
	else
	{
		block->nextStart=0;
	}

	if(preAddr)
	{
		EMB* preBlock=(EMB*)preAddr;
		if(start==preAddr+preBlock->size+sizeof(EMB))
		{
			preBlock->size=preBlock->size+block->size+sizeof(EMB);
			preBlock->nextStart=block->nextStart;
		}
		else
		{
			block->nextStart=start;
		}
	}
	else
	{
		handler->firstFreeStart=start;
	}
	return 1;
}

//wrap: we select firstFit, you can select another one
//user need not know this
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
