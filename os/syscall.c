#include "os.h"
void __sys_write(size_t fd,const char* data,size_t len);
void __sys_yield(void);
/* 根据系统调用号调用对应的处理函数 */
void __SYSCALL(size_t syscall_id,reg_t arg1,reg_t arg2,reg_t arg3){
    switch(syscall_id){
        case __NR_write:
            __sys_write(arg1,(const char*)arg2,arg3);
            break;
        case __NR_sched_yield:
            __sys_yield();
            break;
        default:
            printf("Unsupported syscall id:%d\n",syscall_id);
            break;
    }
}
/* write系统调用的最终实现 */
void __sys_write(size_t fd,const char* data,size_t len){
    if(fd == 1){
        printf(data);
    }
    else{
        panic("Unsupported fd in sys_write!");
    }
}
/* yield系统调用的最终实现 */
void __sys_yield(){
    schedule();
}