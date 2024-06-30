/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * clear_screen和append2screen必须按照如下借口实现
 * 可以增加其他函数供clear_screen和append2screen调用
 */
#define VGA_SCREEN_WIDTH 	80
#define	VGA_SCREEN_HEIGHT	25
#define	CURSOR_line			0xE
#define CURSOR_col			0xF
#define CURSOR_index		0x3D4
#define CURSOR_data			0x3D5

extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000

short cur_line=0;
short cur_column=0;//当前光标位置
char * vga_init_p=(char *)0xB8000;
short offset=0;

void update_cursor(void){
	//通过当前行值cur_cline与列值cur_column回写光标
	cur_line = offset / VGA_SCREEN_WIDTH;
	cur_column = offset - cur_line * VGA_SCREEN_WIDTH;
	outb(CURSOR_index, CURSOR_line);
	outb(CURSOR_data, (offset >> 8)& 0xFF);
	outb(CURSOR_index, CURSOR_col);
	outb(CURSOR_data, offset & 0xFF);    
}

short get_cursor_position(void){
	//获得当前光标，计算出cur_line和cur_column的值
	outb(CURSOR_index, CURSOR_line);
	offset = inb(CURSOR_data);
	outb(CURSOR_index, CURSOR_col);
	offset = offset<<8 + inb(CURSOR_data);
	return offset;
}


void clear_screen(void) {
	//填写正确的内容  
	int *wp; //write pointer
	int i = 0;
	while (i < 0x1000)
	{
		wp = (int*)0xB8000 + i;
		*wp = 0x0F200F20;
		i += 4;
	}
	offset = 0;
	update_cursor();
}
void scroll_screen(void){
	int *rp,*wp;
	int i = 0;
	while (i < offset) 
	{
		wp =(int*)(0xB8000 + 2*i);
		if(i < offset - VGA_SCREEN_WIDTH){
			rp = (int*)(0xB8000 + 2*i + VGA_SCREEN_WIDTH*2);
			*wp = *rp;
		}
		else 
			*wp = 0x0F200F20;
		i = i + 2;
	}
	offset -= VGA_SCREEN_WIDTH;
	update_cursor();
}
void append_char(char str, int color){
	update_cursor();
	while (cur_line >= VGA_SCREEN_HEIGHT)
	{
		scroll_screen();
	}
	char *wp;
	wp = vga_init_p + offset*2;
	*wp = str;
	wp += 1;
	*wp = color;
	offset += 1;
	update_cursor();
}
void append2screen(char *str,int color){ 
	//填写正确的内容
	for (int i = 0; str[i] != '\0'; i++)
	{
		switch (str[i])
		{
		case '\n':
			offset = offset + VGA_SCREEN_WIDTH - offset % VGA_SCREEN_WIDTH;
			update_cursor();
			while (cur_line >= VGA_SCREEN_HEIGHT)
			{
				scroll_screen();
			}
			break;
		case '\f':
			clear_screen();
			break;
		case '\r':
			offset = offset - offset % VGA_SCREEN_WIDTH;
			update_cursor();
			break;
		case '\t':
			offset += 4;
			update_cursor();
			while (cur_line >= VGA_SCREEN_HEIGHT)
			{
				scroll_screen();
			}
			break;
		case '\v':
			offset += VGA_SCREEN_WIDTH;
			update_cursor();
			while (cur_line >= VGA_SCREEN_HEIGHT)
			{
				scroll_screen();
			}
			break;
		case '\b':
			if(offset % VGA_SCREEN_WIDTH != 0){
				offset -= 1;
				update_cursor();
			}
			break;
		default:
			append_char(str[i],color);
			break;
		}
	}
	
}
