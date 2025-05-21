#include "os.h"
void __sys_write(size_t fd,const char* data,size_t len);
void __sys_yield(void);
uint64_t __sys_gettime();
int __sys_fork();
uint64_t __sys_exec(const char* name);
void __sys_read(size_t fd, const char* data, size_t len);
void __sys_exit(uint64_t exit_code);
int __sys_wait();
/* 根据系统调用号调用对应的处理函数 */
uint64_t __SYSCALL(size_t syscall_id, reg_t arg1, reg_t arg2, reg_t arg3) {
    switch (syscall_id)
    {
    case __NR_write:
        __sys_write(arg1,(const char*)arg2, arg3);
        break;
    case __NR_sched_yield:
        __sys_yield();
        break;
    case __NR_gettimeofday:
        return __sys_gettime();
        break;
    case __NR_read:
        __sys_read(arg1,(const char*)arg2, arg3);
        break;
    case __NR_clone:
        return __sys_fork();
        break;
    case __NR_execve:
        return __sys_exec((const char*)arg2);
        break;
    case __NR_exit:
        __sys_exit((uint64_t)arg1);
        break;
    case __NR_waitpid:
        __sys_wait();
        break;
    default:
        printk("Unsupported syscall id:%d\n",syscall_id);
        break;
    }
}
char * translated_byte_buffer(const char* data)
{
    uint64_t  user_satp = current_user_token();  
    PageTable  pt ;
    pt.root_ppn.value = MAKE_PAGETABLE(user_satp);

    uint64_t start_va = (uint64_t)data;
    VirtPageNum vpn = floor_virts(virt_addr_from_size_t(start_va));
    PageTableEntry* pte = find_pte(&pt , vpn);
    
    //拿到物理页地址
    // int mask = ~( (1 << 10) -1 );
    // uint64_t phyaddr = ( pte->bits & mask) << 2 ;
    uint64_t phyaddr = PTE2PA(pte->bits);
    //拿到偏移地址
    uint64_t page_offset = start_va & 0xFFF;
    uint64_t data_d = phyaddr + page_offset;
    return (char*) data_d;
}
/* write系统调用的最终实现 */
void __sys_write(size_t fd, const char* data, size_t len)
{

    if(fd == stdout || fd == stderr)
    {

       char* str =  translated_byte_buffer(data);
       printk("%s",str);
    }
    else
    {
        panic("Unsupported fd in sys_write!");
    }
}
/* yield系统调用的最终实现 */
void __sys_yield(){
    schedule();
}
/* gettime系统调用的最终实现 */
uint64_t __sys_gettime()
{
    return get_time_us();
}
/* read系统调用的最终实现 */
void __sys_read(size_t fd, const char* data, size_t len)
{
    if(fd == stdin){
        int c;
        assert(len == 1);
        while(1){
            c = sbi_console_getchar();
            if( c != -1)
                break;
            schedule();
            continue;
        }
        char* str = translated_byte_buffer(data);
        str[0] = c;
    }
}
/* fork系统调用的最终实现 */
int __sys_fork(){
    struct TaskControlBlock* np;
    struct TaskControlBlock* p = current_proc();
    //分配进程
    if((np = alloc_proc()) ==0){
        return -1;
    }
    //拷贝父进程的内存数据和trap页
    uvmcopy(&p->pagetable,&np->pagetable,p->base_size);
    memcpy((void*)np->trap_cx_ppn,(void*)p->trap_cx_ppn,PAGE_SIZE);
    //子进程返回值设置为0
    TrapContext* cx_ptr = (TrapContext*)np->trap_cx_ppn;
    cx_ptr->a0 = 0;
    cx_ptr->kernel_sp = np->kstack;
    //复制TCB的信息
    np->entry = p->entry;
    np->base_size = p->base_size;
    np->parent = p;
    np->ustack = p->ustack;
    //设置子进程返回地址和内核栈
    np->task_context = tcx_init((reg_t)np->kstack);

    _top++;
    return np->pid;
}
/* exec系统调用的最终实现 */
uint64_t __sys_exec(const char* name){
    char* app_name = translated_byte_buffer(name);
    printk("exec app_name:%s\n",app_name);
    exec(app_name);
    return 0;
}
/* exit系统调用的最终实现 */
void __sys_exit(uint64_t exit_code){
    exit_curproc_and_run_next(exit_code);
}
int __sys_wait(){
    return wait();
}
void exec(char* name){
    //读取ELF文件头
    AppMetadata metadata = get_app_data_by_name(name);
    elf64_ehdr_t *ehdr = (elf64_ehdr_t *)metadata.start;
    //检查ELF文件
    elf_check(ehdr);
    TaskControlBlock* proc = current_proc();
    PageTable old_pt = proc->pagetable;
    uint64_t old_size = proc->base_size;
    //重新分配页表
    proc_pagetable(proc);
    load_segment(ehdr,proc);
    proc_ustack(proc);

    TrapContext* cx_ptr = (TrapContext*)proc->trap_cx_ppn;
    cx_ptr->sepc = (uint64_t)ehdr->e_entry;
    cx_ptr->sp = proc->ustack;
    reg_t sstatus = r_sstatus();
    sstatus &= (0U << 8);
    w_sstatus(sstatus);
    cx_ptr->sstatus = sstatus;
    cx_ptr->kernel_satp = kernel_satp;
    cx_ptr->kernel_sp = proc->kstack;
    cx_ptr->trap_handler = (uint64_t)trap_handler;

    proc_freepagetable(&old_pt,old_size);
}
