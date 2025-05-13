#include "os.h"

void os_main()
{
   printf("Os is running!\n");

   trap_init();

   task_init();

   timer_init();

   run_first_task();
   
}
