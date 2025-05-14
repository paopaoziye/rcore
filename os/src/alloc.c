#include "alloc.h"
#include "address.h"
StackFrameAllocator FrameAllocatorImpl;
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

/* 初始化可用内存 */
void frame_alloctor_init(){
    //初始化时 kernelend 需向上取整
    StackFrameAllocator_init(&FrameAllocatorImpl, \
            ceil_phys(phys_addr_from_size_t((uint64_t)kernelend)), \
            ceil_phys(phys_addr_from_size_t(PHYSTOP)));
    printk("Memoery start:%p\n",kernelend);
    printk("Memoery end:%p\n",PHYSTOP);
}