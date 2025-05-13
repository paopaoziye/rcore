#ifndef OS_STACK_H__
#define OS_STACK_H__

#include "os.h"
//栈的最大元素数
#define MAX_STACK_SIZE 10000
//栈的抽象
typedef struct {
    uint64_t data[MAX_STACK_SIZE];
    int top;    //栈顶序号，必须定义为有符号数
}Stack;
//函数接口
void initStack(Stack *stack);
bool isEmpty(Stack *stack);
bool isFull(Stack *stack);
void push(Stack *stack, uint64_t value);
uint64_t pop(Stack *stack);
uint64_t top(Stack *stack);
#endif
