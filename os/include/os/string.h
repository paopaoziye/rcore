#ifndef OS_STRING_H__
#define OS_STRING_H__
#include "os.h"
//定义字符串的终止符
#define EOS '\0'
size_t strlen(const char* str);
void* memcpy(void* dest,const void *src,size_t count);
void* memset(void *dest,int ch,size_t count);
int strcmp(const char *lhs, const char *rhs);
void strncat(char *dest, const char *src, int n);
#endif