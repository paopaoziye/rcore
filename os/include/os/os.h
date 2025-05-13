#ifndef OS_OS_H__
#define OS_OS_H__

#include <stddef.h>
#include <stdarg.h>

#include "types.h"
#include "context.h"
#include "riscv.h"
#include "task.h"
#include "address.h"

#include "stdio.h"
#include "stack.h"
#include "string.h"

/* syscall.c以及系统调用号 */
extern uint64_t __SYSCALL(size_t syscall_id, reg_t arg1, reg_t arg2, reg_t arg3);
#define __NR_write 64
#define __NR_sched_yield 124
#define __NR_gettimeofday 169
/* kerneltrap.S*/
extern void __alltraps(void);
extern void __restore(pt_regs *next);
/* switch.S */
extern void __switch(TaskContext *current_task_cx_ptr,TaskContext* next_task_cx_ptr );
/* task.c */
extern void task_create(void (*task_entry)(void));
extern void task_init(void);
extern void run_first_task();
extern void schedule();
extern void task_delay(volatile int count);
/* trap.c */
extern void trap_init(void);
/* app.c */
extern size_t sys_write(size_t fd, const char* buf, size_t len);
extern size_t sys_yield();
extern uint64_t sys_gettime();
/* timer.c */
extern void timer_init(void);
extern void set_next_trigger(void);
extern uint64_t get_time_us(void);
/* sbi.c */
extern void sbi_set_timer(uint64_t stime_value);
extern void sbi_console_putchar(int ch);
#endif
