#include "alloc.h"

/* 初始化内存分配器,其中[current,end)为可分配物理页号空间 */
void StackFrameAllocator_init(StackFrameAllocator* allocator,PhysPageNum l,PhysPageNum r){
    allocator->current = l.value;
    allocator->end = r.value;
    initStack(&allocator->recycled);
}
/* 物理页的分配 */
PhysPageNum StackFrameAllocator_alloc(StackFrameAllocator *allocator) {
    PhysPageNum ppn;
    //如果有被回收的物理页，则优先分配被回收的物理页，反之从空闲物理页中分配
    if (allocator->recycled.top >= 0) {
        ppn.value = pop(&(allocator->recycled));
    }else {
        if (allocator->current == allocator->end) {
            ppn.value = 0;
        } else {
            ppn.value = allocator->current++;
        }
    }
    //清空该物理页
    PhysAddr addr = phys_addr_from_phys_page_num(ppn);
    memset((void*)addr.value,0,PAGE_SIZE);
    return ppn;
}
/* 物理页回收 */
void StackFrameAllocator_dealloc(StackFrameAllocator *allocator, PhysPageNum ppn) {
    uint64_t ppnValue = ppn.value;
    //检查被回收的页面号是否小于current，否则是不合法的
    if (ppnValue >= allocator->current) {
        printk("Frame ppn=%lx has not been allocated!\n", ppnValue);
        return;
    }
    //检查被回收的页面号是否已经被回收过
    if(allocator->recycled.top>=0)
    {
        for (size_t i = 0; i <= allocator->recycled.top; i++)
        {
            if(ppnValue ==allocator->recycled.data[i] )
            return;
        }
    }
    //回收物理内存页号
    push(&(allocator->recycled), ppnValue);
}
static StackFrameAllocator FrameAllocatorImpl;
void frame_allocator_test(){
    StackFrameAllocator_init(&FrameAllocatorImpl, \
            floor_phys(phys_addr_from_size_t(MEMORY_START)), \
            ceil_phys(phys_addr_from_size_t(MEMORY_END)));
    printk("Memoery start:%d\n",floor_phys(phys_addr_from_size_t(MEMORY_START)));
    printk("Memoery end:%d\n",ceil_phys(phys_addr_from_size_t(MEMORY_END)));
    PhysPageNum frame[10];
    for (size_t i = 0; i < 5; i++)
    {
         frame[i] = StackFrameAllocator_alloc(&FrameAllocatorImpl);
         printk("frame id:%d\n",frame[i].value);
    }
    for (size_t i = 0; i < 5; i++)
    {
         StackFrameAllocator_dealloc(&FrameAllocatorImpl,frame[i]);
         printk("allocator->recycled.data.value:%d\n",FrameAllocatorImpl.recycled.data[i]);
         printk("frame id:%d\n",frame[i].value);
     }
     PhysPageNum frame_test[10];
     for (size_t i = 0; i < 5; i++)
     {
          frame[i] = StackFrameAllocator_alloc(&FrameAllocatorImpl);
         printk("frame id:%d\n",frame[i].value);
     }
}