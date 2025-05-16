#ifndef OS_TASK_H__
#define OS_TASK_H__

#include "os.h"
#include "address.h"
//任务状态
typedef enum TaskState{
    UnInit,
    Ready,
    Running,
    Exited,
}TaskState;
//任务控制块
typedef struct TaskControlBlock{
    TaskState task_state;       //任务状态
    TaskContext task_context;   //任务上下文
    uint64_t trap_cx_ppn;       //Trap上下文所在物理地址
    uint64_t  base_size;        //应用数据大小
    uint64_t  kstack;           //应用内核栈的虚拟地址
    uint64_t  ustack;           //应用用户栈的虚拟地址
    uint64_t  entry;            //应用程序入口地址
    PageTable pagetable;        //应用页表所在物理页
}TaskControlBlock;
//函数声明
struct TaskContext tcx_init(reg_t kstack_ptr);
void task_create(void (*task_entry)(void));
void run_first_task();
void schedule();
void task_delay(volatile int count);
void proc_mapstacks(PageTable* kpgtbl);
void proc_trap(struct TaskControlBlock *p);
void proc_pagetable(struct TaskControlBlock *p);
TaskControlBlock* task_create_pt(size_t app_id);
uint64_t get_current_trap_cx();
uint64_t current_user_token();
void app_init(size_t app_id);
#endif