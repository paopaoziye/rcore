#include "os.h"
#include "alloc.h"

void os_main()
{
   printk("hello timer os!\n");

   // 内存分配器初始化
   frame_alloctor_init();

   kvminit();

   kvminithart();

   while(1){

   }
}