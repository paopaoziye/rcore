#ifndef OS_RISCV_H__
#define OS_RISCV_H__

#include "os.h"

// sie 寄存器S模式中断相关设置位
#define SIE_SEIE (1L << 9) //外部中断
#define SIE_STIE (1L << 5) //时钟中断
#define SIE_SSIE (1L << 1) //软件中断

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable
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
/* 读写 sie 寄存器，记录了S/M模式下的中断设置 */
static inline reg_t r_sie()
{
  reg_t x;
  asm volatile("csrr %0, sie" : "=r" (x) );
  return x;
}

static inline void w_sie(reg_t x)
{
  asm volatile("csrw sie, %0" : : "r" (x));
}
/* 读取 mtime 寄存器，当 mtime 寄存器的值超过mtimecmp会触发时钟中断 */
static inline reg_t r_mtime()
{
  reg_t x;
  asm volatile("rdtime %0" : "=r"(x));
  return x;
}
/* 读写satp寄存器 */
static inline void w_satp(reg_t x)
{
  asm volatile("csrw satp, %0" : : "r" (x));
}

static inline reg_t r_satp()
{
  reg_t x;
  asm volatile("csrr %0, satp" : "=r" (x) );
  return x;
}

/* 刷新 TLB */
static inline void sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm volatile("sfence.vma zero, zero");
}
// 关闭中断
static inline void intr_off()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// 打开中断
static inline void intr_on()
{
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int
intr_get()
{
  reg_t x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}
#endif