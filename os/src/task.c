#include "task.h"
#include "alloc.h"
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

    task_ctx.ra = (reg_t)trap_return;
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
void task_create(void (*task_entry)(void)){
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
        printk("Too many tasks!!!\n");   
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
/* 为每个应用程序映射内核栈 */
void proc_mapstacks(PageTable* kpgtbl){
    struct TaskControlBlock *p;
    //遍历任务列表
    for(p = tasks;p < &tasks[MAX_TASKS];p++){
        //为内核栈申请物理内存
        char *pa = (char*)phys_addr_from_phys_page_num(kalloc()).value;
        if(pa == 0)
            panic("kalloc");
        //计算对应内核栈所在虚拟地址
        uint64_t va = KSTACK((int)(p-tasks));
        PageTable_map(kpgtbl,virt_addr_from_size_t(va+PAGE_SIZE),phys_addr_from_size_t((uint64_t)pa), \
                      PAGE_SIZE,PTE_R | PTE_W);
        printk("finish task%d stack map : %p!\n",(int)(p-tasks),va); 
        p->kstack = va +  2 * PAGE_SIZE;
    }
}
/* 为每个应用程序分配一页内存用与存放trap，同时初始化任务上下文 */
void proc_trap(struct TaskControlBlock *p){
  // 为每个程序分配一页trap物理内存
  p->trap_cx_ppn = phys_addr_from_phys_page_num(kalloc()).value;
  printk("trap value:%p\n",p->trap_cx_ppn);
  // 初始化任务上下文全部为0
  memset(&p->task_context, 0 ,sizeof(p->task_context));
}
/* 为用户程序创建页表，映射跳板页和trap上下文页*/
void proc_pagetable(struct TaskControlBlock *p){
  // 创建一个空的用户的页表，分配一页内存
  PageTable pagetable;
  pagetable.root_ppn = kalloc();
  
  //映射跳板页
  PageTable_map(&pagetable,virt_addr_from_size_t(TRAMPOLINE),phys_addr_from_size_t((uint64_t)trampoline),\
                PAGE_SIZE , PTE_R | PTE_X);
  //映射用户程序的trap页
  PageTable_map(&pagetable,virt_addr_from_size_t(TRAPFRAME),phys_addr_from_size_t(p->trap_cx_ppn), \
                PAGE_SIZE, PTE_R | PTE_W );
  p->pagetable = pagetable;
  printk("p->pagetable:%p\n",p->pagetable.root_ppn.value);
}
/* 创建app页表 */
TaskControlBlock* task_create_pt(size_t app_id){
  if(_top < MAX_TASKS)
  {
    //为app分配一页内存存放trap
    proc_trap(&tasks[app_id]);
    //为app创建页表，映射跳板页和trap上下文页
    proc_pagetable(&tasks[app_id]); 
    _top++;
  }
  
  return &tasks[app_id];
}
/* 获取当前app的trap上下文地址 */
uint64_t get_current_trap_cx(){
  return tasks[_current].trap_cx_ppn;
}
/* 返回当前执行的应用程序的satp token*/
uint64_t current_user_token(){
   return MAKE_SATP(tasks[_current].pagetable.root_ppn.value);
}
void app_init(size_t app_id){
    //获取app的trap上下文
    TrapContext* cx_ptr = (TrapContext*)tasks[app_id].trap_cx_ppn;
    //设置 sstatus 寄存器SPP位为0，即U模式
    reg_t sstatus = r_sstatus();
    sstatus &= (0U << 8);
    w_sstatus(sstatus);
    // 设置程序入口地址
    cx_ptr->sepc = tasks[app_id].entry;
    // 保存 sstatus 寄存器内容
    cx_ptr->sstatus = sstatus; 
    // 设置用户栈虚拟地址
    cx_ptr->sp = tasks[app_id].ustack;
    // 设置内核页表token
    cx_ptr->kernel_satp = kernel_satp;
    // 设置内核栈虚拟地址
    cx_ptr->kernel_sp = tasks[app_id].kstack;
    // 设置内核trap_handler的地址
    cx_ptr->trap_handler = (uint64_t)trap_handler;

    /* 构造每个任务任务控制块中的任务上下文，设置 ra 寄存器为 trap_return 的入口地址*/
    tasks[app_id].task_context = tcx_init((reg_t)cx_ptr);
    // 初始化 TaskStatus 字段为 Ready
    tasks[app_id].task_state = Ready;
}
void run_first_task(){
    tasks[0].task_state = Running;
    struct TaskContext *next_task_cx_ptr = &(tasks[0].task_context);
    struct TaskContext _unused ;
    printk("task is running!\n");
    __switch(&_unused,next_task_cx_ptr);
    panic("unreachable in run_first_task!");
}