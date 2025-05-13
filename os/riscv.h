#ifndef __RISCV_H__
#define __RISCV_H__

#include "os.h"

/* 读取 sepc 寄存器的值，记录了trap发生之前的执行的最后一条指令地址 */
static inline reg_t r_sepc()
{
  reg_t x;
  asm volatile("csrr %0, sepc" : "=r" (x) ); //volatile禁止编译器优化该语句，确保其一定会被执行
  return x;
}

/* scause 记录了异常原因 */
static inline reg_t r_scause()
{
  reg_t x;
  asm volatile("csrr %0, scause" : "=r" (x) );
  return x;
}

/* stval 记录了trap发生时的地址 */ 
static inline reg_t r_stval()
{
  reg_t x;
  asm volatile("csrr %0, stval" : "=r" (x) );
  return x;
}

/* sstatus 记录S模式下处理器内核的运行状态 */
static inline reg_t r_sstatus()
{
  reg_t x;
  asm volatile("csrr %0, sstatus" : "=r" (x) );
  return x;
}


static inline void  w_sstatus(reg_t x)
{
  asm volatile("csrw sstatus, %0" : : "r" (x));
}

/* 读写 stvec 寄存器，存放了异常处理程序的地址 */
static inline void  w_stvec(reg_t x)
{
  asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline reg_t r_stvec()
{
  reg_t x;
  asm volatile("csrr %0, stvec" : "=r" (x) );
  return x;
}

#endif