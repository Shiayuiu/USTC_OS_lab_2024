/* 
 * 与 UART 相关的输出
 * 调用inb和outb函数，实现下面的uart的三个函数
 */
extern unsigned char inb(unsigned short int port_from);
extern void outb (unsigned short int port_to, unsigned char value);

#define uart_base 0x3F8

void uart_put_char(unsigned char c){
	//填写正确的内容
	outb(uart_base, c);
}

unsigned char uart_get_char(void){
	//填写正确的内容 
	unsigned char ch;
	while ((ch = inb(uart_base))== 0x00);
	return ch;
}

void uart_put_chars(char *str){ 
	//填写正确的内容
	int index=0;
	while (str[index]!='\0')
	{
		if(str[index]=='\n')
		{
			str[index] == '\r';
		}
		outb(uart_base,str[index]);
		index++;
	}
}