#include "io.h"
#include "myPrintk.h"
#include "uart.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "wallClock.h"

int *h,*m,*s;
typedef struct myCommand {
    char name[80];
    char help_content[200];
    int (*func)(int argc, char (*argv)[8]);
}myCommand; 
int func_cmd(int argc, char(*argv)[8]);
int func_help(int argc, char(*argv)[8]);
myCommand cmd={"cmd\0","List all command\n\0",func_cmd};
myCommand help={"help\0","Usage: help [command]\n\0Display info about [command]\n\0",func_help};

int bufdiv(char* str,int len, char (*argv)[8]){
    int count = 0;
    int i = 0;
    char* p = str;
    while (*p)
    {  
        if((*p)==' '){
            while(*p++ == ' ');
            if(*--p){
                argv[count][i] = '\0';
                i = 0;
                count++;
            }
        }
        else{
            argv[count][i++] = *p++;
        }
        
    }
    argv[count++][i] = '\0';
    return count;
}

int func_cmd(int argc, char (*argv)[8]){
	if(argc == 1){
        myPrintk(0x7, "cmd\nhelp\n\0");
    }
    else {
        myPrintk(0x7, "Unknown Interrupt.\n\0");
    }
    return 0;
}

int func_help(int argc, char (*argv)[8]){
    if(argc == 1){
        myPrintk(0x7, help.help_content);
        return 0;
    }
    else if(argc == 2){
        if(!myStrcmp(argv[1],"cmd\0")){
            myPrintk(0x7,cmd.help_content);
            return 0;
        }
    }
    else {
        myPrintk(0x7,"Unknown Interrupt.\n\0");
        myPrintk(0x7, argc);
    }
    return 0;
}


void startShell(void){
//我们通过串口来实现数据的输入
char BUF[256]; //输入缓存区
int BUF_len=0;	//输入缓存区的长度
    
	int argc;
    char argv[8][8];

    do{
        int s0 = *s;
        getWallClock(h,m,s);
        if(*s != s0){
            setWallClock(*h, *m, *s);
        }

        BUF_len=0; 
        myPrintk(0x07,"Student>>\0");
        while((BUF[BUF_len]=uart_get_char())!='\r'){
            uart_put_char(BUF[BUF_len]);//将串口输入的数存入BUF数组中
            BUF_len++;  //BUF数组的长度加一
        }
        uart_put_chars(" -pseudo_terminal\0");
        uart_put_char('\n');
        myPrintk(0x7, BUF);
        myPrintk(0x7, "\n");
        argc = bufdiv(BUF,BUF_len,argv);
        if(!myStrcmp("cmd\0",argv[0])){
            func_cmd(argc,argv);
        }
        else if(!myStrcmp("help\0",argv[0])){
            func_help(argc,argv);
        }
        else{
            myPrintk(0x7,"Unknown Interrupt\n\0");
        }

    }while(1);

}
int myStrcmp(const char* str1, const char* str2){
    while (*str1)
    {
        while(*str1++ == *str2++);
    }
    return (int)(*str1 - *str2);
}