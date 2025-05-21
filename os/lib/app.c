#include "os.h"
#include "stdio.h"
/* riscv的系统调用封装，id为系统调用号，arg1-3为传递的参数 */
uint64_t syscall(size_t id, reg_t arg1, reg_t arg2, reg_t arg3) {
    register uintptr_t a0 asm ("a0") = (uintptr_t)(arg1);
    register uintptr_t a1 asm ("a1") = (uintptr_t)(arg2);
    register uintptr_t a2 asm ("a2") = (uintptr_t)(arg3);
    register uintptr_t a7 asm ("a7") = (uintptr_t)(id);
    
    asm volatile ("ecall"
		      : "+r" (a0)
		      : "r" (a1), "r" (a2), "r" (a7)
		      : "memory");
    return a0;
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
int sys_read(size_t fd, const char* buf, size_t len){
    return syscall(__NR_read,(reg_t)fd,(reg_t)buf,(reg_t)len);
}
int sys_fork(){
    return syscall(__NR_clone,0,0,0);
}
int sys_exec(char* name){
    return syscall(__NR_execve,0,(reg_t)name,0);
}
int sys_exit(uint64_t exit_code){
    return syscall(__NR_exit,exit_code,0,0);
}
int sys_waitpid(){
    return syscall(__NR_waitpid,0,0,0);
}


