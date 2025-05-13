#include "os.h"
//输出缓冲区
static char out_buf[1000];
/* 逐步输出字符串 */
void uart_puts(char *s)
{
	while (*s) {
		sbi_console_putchar(*s++);
	}
}
/* 将格式化后的s输入到out中，并计算格式化后的s的长度 */
static int _vsnprintf(char * out, size_t n, const char* s, va_list vl)
{
	int format = 0;
	int longarg = 0;
	size_t pos = 0;
	//遍历字符s
	for (; *s; s++) {
		//进入格式化解析模式，根据s进入不同的switch分支
		//通过va_arg从可变参数列表中按指定类型提取参数，转换后填充到out中
		if (format) {
			switch(*s) {
			case 'l': {
				longarg = 1;
				break;
			}
			case 'p': {
				longarg = 1;
				if (out && pos < n) {
					out[pos] = '0';
				}
				pos++;
				if (out && pos < n) {
					out[pos] = 'x';
				}
				pos++;
			}
			case 'x': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				int hexdigits = 2*(longarg ? sizeof(long) : sizeof(int))-1;
				for(int i = hexdigits; i >= 0; i--) {
					int d = (num >> (4*i)) & 0xF;
					if (out && pos < n) {
						out[pos] = (d < 10 ? '0'+d : 'a'+d-10);
					}
					pos++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'd': {
				long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
				if (num < 0) {
					num = -num;
					if (out && pos < n) {
						out[pos] = '-';
					}
					pos++;
				}
				long digits = 1;
				for (long nn = num; nn /= 10; digits++);
				for (int i = digits-1; i >= 0; i--) {
					if (out && pos + i < n) {
						out[pos + i] = '0' + (num % 10);
					}
					num /= 10;
				}
				pos += digits;
				longarg = 0;
				format = 0;
				break;
			}
			case 's': {
				const char* s2 = va_arg(vl, const char*);
				while (*s2) {
					if (out && pos < n) {
						out[pos] = *s2;
					}
					pos++;
					s2++;
				}
				longarg = 0;
				format = 0;
				break;
			}
			case 'c': {
				if (out && pos < n) {
					out[pos] = (char)va_arg(vl,int);
				}
				pos++;
				longarg = 0;
				format = 0;
				break;
			}
			default:
				break;
			}
		} 
		//当s为%时，进入格式化解析
		else if (*s == '%') {
			format = 1;
		}
		//其余情况表示s为一个普通字符，直接将其存入out缓冲区
		else {
			if (out && pos < n) {
				out[pos] = *s;
			}
			pos++;
		}
    }
	//添加中止符
	if (out && pos < n) {
		out[pos] = 0;
	} else if (out && n) {
		out[n-1] = 0;
	}
	return pos;
}
/*计算格式化后s的长度，并通过_vsnprintf将字符串格式化到out_buf中，最后通过uart_puts输出 */
static int _vprintf(const char* s, va_list vl)
{
	int res = _vsnprintf(NULL, -1, s, vl);
	if (res+1 >= sizeof(out_buf)) {
		uart_puts("error: output string size overflow\n");
		while(1) {}
	}
	_vsnprintf(out_buf, res + 1, s, vl);
	uart_puts(out_buf);
	return res;
}
/* 将第一个参数s的实际值以及其指针传递给_vprintf，并返回s的长度 */
int printf(const char* s, ...)
{
	int res = 0;
	va_list vl;
	va_start(vl, s);
	res = _vprintf(s, vl);
	va_end(vl);
	return res;
}
/* 发出错误警告 */
void panic(char *s)
{
	printf("panic: ");
	printf(s);
	printf("\n");
	while(1){};
}