#include "os.h"
#include "alloc.h"
void os_main()
{
   printk("Os is running!\n");
   frame_allocator_test();
   while(1){
      
   }
   //trap_init();

   //task_init();
   
   //timer_init();

   //run_first_task();
   
}
