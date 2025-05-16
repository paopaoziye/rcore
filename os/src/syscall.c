#include "os.h"
void __sys_write(size_t fd,const char* data,size_t len);
void __sys_yield(void);
uint64_t __sys_gettime();
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
    default:
        printk("Unsupported syscall id:%d\n",syscall_id);
        break;
    }
}
char * translated_byte_buffer(const char* data , size_t len)
{
    uint64_t  user_satp = current_user_token();  
    PageTable  pt ;
    pt.root_ppn.value = MAKE_PAGETABLE(user_satp);

    uint64_t start_va = (uint64_t)data;
    uint64_t end_va = start_va + len;
    VirtPageNum vpn = floor_virts(virt_addr_from_size_t(start_va));
    PageTableEntry* pte = find_pte(&pt , vpn);
    
    //拿到物理页地址
    int mask = ~( (1 << 10) -1 );
    uint64_t phyaddr = ( pte->bits & mask) << 2 ;
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

       char* str =  translated_byte_buffer(data,len);
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