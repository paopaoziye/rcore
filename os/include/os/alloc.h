#ifndef OS_ALLOC_H__
#define OS_ALLOC_H__

#include "os.h"

//栈式内存分配器
typedef struct 
{
    uint64_t current;   //空闲内存的起始物理页号
    uint64_t  end;      //空闲内存的结束物理页号
    Stack recycled;     //被回收的物理页号
}StackFrameAllocator;
//函数声明
void StackFrameAllocator_init(StackFrameAllocator* allocator,PhysPageNum l,PhysPageNum r);
PhysPageNum StackFrameAllocator_alloc(StackFrameAllocator *allocator);
void StackFrameAllocator_dealloc(StackFrameAllocator *allocator, PhysPageNum ppn);
void frame_alloctor_init();
PhysPageNum kalloc(void);
//内存分配器
extern StackFrameAllocator FrameAllocatorImpl;
#endif