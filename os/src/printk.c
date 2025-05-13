#include "stdio.h"
//输出缓冲区
static char out_buf[1000];
/* 逐步输出字符串 */
void uart_puts(char *s){
	while (*s) {
		sbi_console_putchar(*s++);
	}
}
/*计算格式化后s的长度，并通过_vsnprintf将字符串格式化到out_buf中，最后通过uart_puts输出 */
static int vprintk(const char* s, va_list vl){
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	uart_puts(out_buf);
	return res;
}
/* 将第一个参数s的实际值以及其指针传递给vprintk，并返回s的长度 */
int printk(const char* s, ...){
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = vprintk(s, vl);
	va_end(vl);
	return res;
}
/* 发出错误警告 */
void panic(char *s){
	printk("panic: ");
	printk(s);
	printk("\n");
	while(1){};
}