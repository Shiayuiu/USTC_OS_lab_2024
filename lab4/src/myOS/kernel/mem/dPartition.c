#include "../../include/myPrintk.h"


//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, belongs to user
	};	                           
} EMB;	//共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}


unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	
	*/
	if(EMB_size + dPartition_size >= totalSize){
		return 0;
	}

	dPartition *h = (dPartition*)start;
	h->size = totalSize;
	h->firstFreeStart = start + dPartition_size;

	EMB	*block_h = (EMB*)h->firstFreeStart;
	block_h->size = totalSize - dPartition_size;
	block_h->nextStart = 0;

	return start;
}


void dPartitionWalkByAddr(unsigned long dp){
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息

	*/
	dPartition *dp_h = (dPartition*)dp;
	showdPartition(dp_h);

	//遍历EMB
	unsigned long p = dp_h->firstFreeStart;
	EMB* block_h;
	while (p)
	{
		block_h = (EMB*)p;
		showEMB(block_h);
		p = block_h->nextStart;
	}

}


//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。

	*/
	//合法性检查
	if(size <= 0) {return 0;}
	if(size & 1) size += 1;
	if(size & 2) size += 2;
	if(size & 4) size += 4;

	dPartition *dp_h = (dPartition*)dp;
	if(!(dp_h->firstFreeStart)) {return 0;} //no free block

	unsigned long addr = dp_h->firstFreeStart;
	unsigned long addr_pre = 0;
	EMB *block;
	EMB *block_pre;

	while (addr)
	{
		block = (EMB*)addr;
		block_pre = (EMB*)addr_pre;

		if(block->size >= size + sizeof(unsigned long) && block->size <= size + sizeof(unsigned long) + EMB_size) {
			if(addr_pre == 0){
				dp_h->firstFreeStart = block->nextStart;
			}
			else {
				block_pre->nextStart = block->nextStart;
			}
			return addr + sizeof(unsigned long);
		}
		else if(block->size > size + sizeof(unsigned long)+ EMB_size) {
			unsigned long newnode = addr + sizeof(unsigned long) + size;
			EMB* newblock = (EMB*)newnode;
			newblock->size = block->size - size - sizeof(unsigned long);
			newblock->nextStart = block->nextStart;
			block->size -= newblock->size;

			if(!addr_pre){
				dp_h->firstFreeStart = newnode;
			}
			else {
				block_pre->nextStart = newnode;
			}
			return addr + sizeof(unsigned long);
		}
		addr_pre = addr;
		addr = block->nextStart;
	}
	
	return 0; //when failed
}


unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	
	*/
	start = start - sizeof(unsigned long);

	if(start < dp + dPartition_size){
		return 0;
	}
	dPartition *dp_h = (dPartition*)dp;
	if(start >= dp_h->size + dp){
		return 0;
	}

	unsigned long addr = dp_h->firstFreeStart;
	unsigned long addr_pre;
	unsigned long addr_nxt;
	EMB* block;

	while (addr)
	{
		block = (EMB*)addr;
		if(addr < start){
			addr_pre = addr;
		}
		else if(addr > start){
			addr_nxt = addr;
			break;
		}
		addr = block -> nextStart;
	}	
	
	block = (EMB*)start;
	if(addr_nxt) {
		if(addr_nxt == start + block->size) {
			EMB* block_nxt = (EMB*)addr_nxt;
			block->size += block_nxt->size;
			block->nextStart = block_nxt->nextStart;
		}
		else {
			block->nextStart = addr_nxt;
		}
	}
	else {
		block->nextStart = 0;
	}

	if(addr_pre) {
		EMB* block_pre = (EMB*)addr_pre;
		if(start == addr_pre + block_pre->size) {
			block_pre->size += block->size;
			block_pre->nextStart = block->nextStart;
		}
		else {
			block_pre->nextStart = start;
		}
	}
	else {
		dp_h->firstFreeStart = start;//已无空闲块
	}
	return 1;
}


// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
