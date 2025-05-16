#include "os.h"
#include "alloc.h"

void os_main()
{
   printk("hello timer os!\n");
   frame_alloctor_init();
   kvminit();
   load_app(0);
   app_init(0);
   load_app(1);
   app_init(1);
   kvminithart();
   set_kernel_trap_entry();
   timer_init();
   run_first_task();

}