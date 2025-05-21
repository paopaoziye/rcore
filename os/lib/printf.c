#include "stdio.h"
//输出缓冲区
static char out_buf[1000]; 
/* 类似vprintk，使用sys_write输出 */
static int vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	_vsnprintf(out_buf, res + 1, s, vl);
	sys_write(stdout,out_buf,res + 1);
	return res;
}
/* 类似printk */
int printf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = vprintf(s, vl);
	va_end(vl);
	return res;
}
/* 从串口获取一个字符 */
char getchar(){
	char data[1];
	sys_read(stdin,data,1);
	return data[0];
}