.globl start

MAGIC_ITEM_NAME = 0x1BADB002
FLAGS_ITEM_NAME = 0x00000000
CHECKSUM_ITEM_NAME = 0x00000000 - FLAGS_ITEM_NAME - MAGIC_ITEM_NAME

.section ".multiboot_header"
.align 4
	.long MAGIC_ITEM_NAME    
	.long FLAGS_ITEM_NAME     
	.long CHECKSUM_ITEM_NAME  

.text # 进入代码段
.code32 # 32位代码

start:  # 这个跟第一行的声明要一致
	# initialize vga address
	movl $0, %eax

clean:
	movl $0xB8000, %ebx
	movl $0, (%ebx, %eax, 4)
	addl $1, %eax
	cmpl $0x1000, %eax
	jl clean

vgaout:
	movl $0x2f652f48, 0xB8000 #e H
	movl $0x2f6C2f6C, 0xB8004 #l l
	movl $0x2f202f6F, 0xB8008 #space o
	movl $0x2f6F2f77, 0xB800C #o w
	movl $0x2f6C2f72, 0xB8010 #l r
	movl $0x2f212f64, 0xB8014 #! d
	movl $0x2f502f20, 0xB8018 #P space
	movl $0x2f322f42, 0xB801C #2 B
	movl $0x2f302f32, 0xB8020 #0 2
	movl $0x2f302f30, 0xB8024 #0 0
	movl $0x2f372f30, 0xB8028 #7 0
	movl $0x2f5F2f39, 0xB802C #_ 9
	movl $0x2f752f57, 0xB8030 #u W
	movl $0x2f692f59, 0xB8034 #i Y
	movl $0x2f612f66, 0xB8038 #a f
	movl $0x2f2E2f6E, 0xB803C #. n

uartout:
	movw $0x3F8, %dx
	
	movb $0x48, %al
	outb %al, %dx
	movb $0x65, %al
	outb %al, %dx	
	movb $0x6C, %al
	outb %al, %dx
	movb $0x6C, %al
	outb %al, %dx
	movb $0x6F, %al
	outb %al, %dx
	movb $0x20, %al
	outb %al, %dx
	movb $0x77, %al
	outb %al, %dx
	movb $0x6F, %al
	outb %al, %dx
	movb $0x72, %al
	outb %al, %dx
	movb $0x6C, %al
	outb %al, %dx
	movb $0x64, %al
	outb %al, %dx
	movb $0x21, %al
	outb %al, %dx
	movb $0x20, %al
	outb %al, %dx
	movb $0x50, %al
	outb %al, %dx
	movb $0x42, %al
	outb %al, %dx
	movb $0x32, %al
	outb %al, %dx
	movb $0x32, %al
	outb %al, %dx
	movb $0x30, %al
	outb %al, %dx
	movb $0x30, %al
	outb %al, %dx
	movb $0x30, %al
	outb %al, %dx
	movb $0x30, %al
	outb %al, %dx
	movb $0x37, %al
	outb %al, %dx
	movb $0x39, %al
	outb %al, %dx
	movb $0x5F, %al
	outb %al, %dx
	movb $0x57, %al
	outb %al, %dx
	movb $0x75, %al
	outb %al, %dx
	movb $0x59, %al
	outb %al, %dx
	movb $0x69, %al
	outb %al, %dx
	movb $0x66, %al
	outb %al, %dx
	movb $0x61, %al
	outb %al, %dx
	movb $0x6E, %al
	outb %al, %dx
	movb $0x0A, %al
	outb %al, %dx

	hlt
