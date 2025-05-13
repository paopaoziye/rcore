#include "os.h"
/* riscv的系统调用封装，id为系统调用号，arg1-3为传递的参数 */
size_t syscall(size_t id, reg_t arg1, reg_t arg2, reg_t arg3) {
    long ret;
    //使用内联汇编，将id传递给寄存器a7，arg1-3传递给寄存器a0-a2
    //调用ecall触发系统调用后将a0寄存器的值即返回值传递给参数ret
    asm volatile (
        "mv a7, %1\n\t"   
        "mv a0, %2\n\t"   
        "mv a1, %3\n\t"   
        "mv a2, %4\n\t"   
        "ecall\n\t"       
        "mv %0, a0"    
        : "=r" (ret)
        : "r" (id), "r" (arg1), "r" (arg2), "r" (arg3)
        : "a7", "a0", "a1", "a2", "memory"
    );
    return ret;
}
/* 一系列系统调用接口 */
size_t sys_write(size_t fd, const char* buf, size_t len){
    syscall(__NR_write,(reg_t)fd,(reg_t)buf,(reg_t)len);
}
size_t sys_yield(){
    syscall(__NR_sched_yield,0,0,0);
}
uint64_t sys_gettime()
{
    return syscall(__NR_gettimeofday,0,0,0);
}


