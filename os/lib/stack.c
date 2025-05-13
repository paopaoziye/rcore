#include "stack.h"
/* 栈的初始化 */
void initStack(Stack *stack){
    stack->top = -1;
}
/* 判断栈是否为空 */
bool isEmpty(Stack *stack){
    return stack->top == -1;
}
/* 判断栈是否为满 */
bool isFull(Stack *stack){
    return stack->top == MAX_STACK_SIZE -1;
}
/* 入栈操作 */
void push(Stack *stack,uint64_t value){
    if(isFull(stack)){
        printk("Stack overflow\n");
        return;
    }
    stack->data[++stack->top] = value;
}
/* 出栈操作 */
uint64_t pop(Stack *stack){
    if(isEmpty(stack)){
        printk("Stack underflow\n");
        return -1;       
    }
    return stack->data[stack->top--];
}
/* 获取栈顶元素 */
uint64_t top(Stack *stack){
    if(isEmpty(stack)){
        printk("Stack is empty\n");
        return -1;       
    }
    return stack->data[stack->top];
}



