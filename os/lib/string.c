#include "string.h"
/* 计算字符串的长度 */
size_t strlen(const char* str){
    char *ptr = (char *)str;
    while(*ptr != EOS){
        ptr++;
    }
    return ptr-str;
}
/* 将src的n个字节复制到dest */
void* memcpy(void* dest,const void *src,size_t count){
    char *ptr = dest;
    while(count--){
        *ptr++ = *((char*)(src++));
    }
    return dest;
}
/* 将count个字符ch填充到dest起始处 */
void* memset(void *dest,int ch,size_t count){
    char *ptr = dest;
    while(count--){
        *ptr++ = ch;
    }
    return dest;
}
//把 lhs1 所指向的字符串和 rhs2 所指向的字符串进行比较
int strcmp(const char *lhs, const char *rhs)
{
    while (*lhs == *rhs && *lhs!= EOS && *rhs != EOS)
    {
        lhs++;
        rhs++;
    }
    return *lhs < *rhs ? -1 : *lhs > *rhs;
}
void strncat(char *dest, const char *src, int n) {
    while (*dest) {
        dest++;
    }
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
}
