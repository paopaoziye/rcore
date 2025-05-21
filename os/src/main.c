#include "os.h"
#include "alloc.h"

void os_main()
{
   printk("hello timer os!\n");
   frame_alloctor_init();
   kvminit();
   proc_init();
   load_app(0);
   app_init(0);
   kvminithart();
   set_kernel_trap_entry();
   get_app_names();
   timer_init();
   run_first_task();
}