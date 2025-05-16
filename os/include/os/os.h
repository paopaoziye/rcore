#ifndef OS_OS_H__
#define OS_OS_H__

#include <stddef.h>
#include <stdarg.h>

#include "types.h"
#include "context.h"
#include "riscv.h"
#include "task.h"
#include "address.h"
#include "assert.h"
#include "loader.h"

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
/* trap.c */
extern void trap_init(void);
extern void trap_from_kernel(void);
void set_kernel_trap_entry(void);
void set_user_trap_entry(void);
void trap_handler(void);
void trap_return(void);
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
