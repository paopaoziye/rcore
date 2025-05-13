#include "os.h"

//用户栈、内核栈大小以及任务最大数量
#define USER_STACK_SIZE (4096 * 2)
#define KERNEL_STACK_SIZE (4096 * 2)
#define MAX_TASKS 10
//为内核栈和用户栈开辟空间
uint8_t KernelStack[MAX_TASKS][KERNEL_STACK_SIZE];
uint8_t UserStack[MAX_TASKS][USER_STACK_SIZE];
//任务队列，当前任务号和任务总数
struct TaskControlBlock tasks[MAX_TASKS];
static int _current = 0;
static int _top = 0;
/* 初始化任务上下文 */
struct TaskContext tcx_init(reg_t kstack_ptr) {
    struct TaskContext task_ctx;

    task_ctx.ra = (reg_t)__restore;
    task_ctx.sp = kstack_ptr;
    task_ctx.s0 = 0;
    task_ctx.s1 = 0;
    task_ctx.s2 = 0;
    task_ctx.s3 = 0;
    task_ctx.s4 = 0;
    task_ctx.s5 = 0;
    task_ctx.s6 = 0;
    task_ctx.s7 = 0;
    task_ctx.s8 = 0;
    task_ctx.s9 = 0;
    task_ctx.s10 = 0;
    task_ctx.s11 = 0;

    return task_ctx;
}
/* 创建任务，接收参数为一个函数指针即应用程序的地址 */
void task_create(void (*task_entry)(void))
{
    if(_top < MAX_TASKS)
    {
        //构造该任务的trap上下文，包括入口地址和用户栈指针，并将其压入到内核栈顶
        TrapContext* cx_ptr = (TrapContext *)(&KernelStack[_top] + KERNEL_STACK_SIZE - sizeof(TrapContext));
        //获取任务用户栈指针
        reg_t user_sp = (reg_t)(&UserStack[_top] + USER_STACK_SIZE);

        //设置 sstatus 寄存器第8位即SPP位为0 表示为U模式
        reg_t sstatus = r_sstatus();
        sstatus &= (0U << 8);
        w_sstatus(sstatus);

        //设置用户程序内核栈，填充用户栈指针
        cx_ptr->sepc = (reg_t)task_entry;
        cx_ptr->sstatus = sstatus; 
        cx_ptr->sp = user_sp;

        //构造每个任务任务控制块中的任务上下文，设置 ra 寄存器为 __restore 的入口地址
        tasks[_top].task_context = tcx_init((reg_t)cx_ptr);
        //初始化 TaskStatus 字段为Ready
        tasks[_top].task_state = Ready;
        //任务总数量递增
        _top++;

    }else{
        printf("Too many tasks!!!\n");   
    }
    
}
/* 任务调度器，使用轮转调度 */
void schedule(){
    //错误检查
	if (_top <= 0) {
		panic("Num of task should be greater than zero!\n");
		return;
	}
    //获取下一个任务
    int next = _current + 1;
    next = next % _top;
    //修改任务状态并进行上下文切换
    if(tasks[next].task_state == Ready)
    {
        struct TaskContext *current_task_cx_ptr = &(tasks[_current].task_context);
        struct TaskContext *next_task_cx_ptr = &(tasks[next].task_context);
        tasks[next].task_state = Running;
        tasks[_current].task_state = Ready;
        _current = next;
        __switch(current_task_cx_ptr,next_task_cx_ptr);
    }
    
}
/* 延迟函数的简单实现 */
void task_delay(volatile int count){
	count *= 50000;
	while (count--);
}
/* 启动系统的第一个任务 */
void run_first_task(){
    tasks[0].task_state = Running;
    struct TaskContext *next_task_cx_ptr = &(tasks[0].task_context);
    struct TaskContext _unused ;

    __switch(&_unused,next_task_cx_ptr);
    panic("unreachable in run_first_task!");
}
/* 创建一系列任务以测试轮转调度机制 */
void task1(){
    const char *message = "task1 is running!\n";
    int len = strlen(message);
    while (1){
        sys_write(1,message, len);
    }
}

void task2(){
    const char *message = "task2 is running!\n";
    int len = strlen(message);
    while (1){
        sys_write(1,message, len);
    }
}

void task3(){
    const char *message = "task3 is running!\n";
    int len = strlen(message);
    while (1){
        sys_write(1,message, len);
    }
}


void task_init(void){
	task_create(task1);
	task_create(task2);
    task_create(task3);
}


