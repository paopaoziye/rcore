#ifndef OS_ADDRESS_H
#define OS_ADDRESS_H
#include "types.h"

extern uint64_t kernel_satp;
#define PGROUNDUP(sz)  (((sz)+PAGE_SIZE-1) & ~(PAGE_SIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PAGE_SIZE-1))
//相关宏定义
#define PAGE_SIZE 0x1000      //页的大小即4KB  
#define PAGE_SIZE_BITS   0xc  //页内偏移地址长度即12位   

#define PA_WIDTH_SV39 56      //物理地址长度
#define VA_WIDTH_SV39 39      //虚拟地址长度
#define PPN_WIDTH_SV39 (PA_WIDTH_SV39 - PAGE_SIZE_BITS)  // 物理页号 44位 [55:12]
#define VPN_WIDTH_SV39 (VA_WIDTH_SV39 - PAGE_SIZE_BITS)  // 虚拟页号 27位 [38:12]

//内核地图
#define KERNBASE 0x80200000L                //内核起始地址
extern char etext[];                        //内核代码段尾部地址
extern char kernelend[];                    //内核数据段尾部地址
#define PHYSTOP (KERNBASE + 128*1024*1024)  //内核空余空间尾部地址
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))  //Sv39的最大地址空间 512G
#define TRAMPOLINE (MAXVA - PAGE_SIZE)      //跳板页开始位置
extern char trampoline[];                   //跳板页物理地址
#define TRAPFRAME (TRAMPOLINE - PAGE_SIZE)  //Trap页开始位置
//计算应用内核栈的地址，每个应用的内核栈下都有一个无效的守卫页
#define KSTACK(p) (TRAMPOLINE - ((p)+1)* 2*PAGE_SIZE)
#define MAKE_PAGETABLE(satp) ( satp & (SATP_SV39 - 1) )
#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PTE_FLAGS(pte) ((pte) & 0x3FF)
/* 物理地址 */
typedef struct {
    uint64_t value; 
} PhysAddr;

/* 虚拟地址 */
typedef struct {
    uint64_t value;
} VirtAddr;

/* 物理页号 */
typedef struct {
    uint64_t value;
} PhysPageNum;

/* 虚拟页号 */
typedef struct {
    uint64_t value;
} VirtPageNum;

/* 页表项 */
typedef struct  {
    uint64_t bits;
}PageTableEntry;

/* 页表 */
typedef struct {
    PhysPageNum root_ppn; //页表根节点物理页号
}PageTable;
/* 页表项标志位 */
#define PTE_V (1 << 0)   //有效位
#define PTE_R (1 << 1)   //可读属性
#define PTE_W (1 << 2)   //可写属性
#define PTE_X (1 << 3)   //可执行属性
#define PTE_U (1 << 4)   //用户访问模式
#define PTE_G (1 << 5)   //全局映射
#define PTE_A (1 << 6)   //访问标志位
#define PTE_D (1 << 7)   //脏位
/* 分页模式相关 */
#define SATP_SV39 (8UL << 60)
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64_t)pagetable)))

//函数接口
PhysAddr phys_addr_from_size_t(uint64_t v);
PhysPageNum phys_page_num_from_size_t(uint64_t v);
uint64_t size_t_from_phys_addr(PhysAddr v);
uint64_t size_t_from_phys_page_num(PhysPageNum v);
PhysAddr phys_addr_from_phys_page_num(PhysPageNum ppn);
VirtAddr virt_addr_from_size_t(uint64_t v);
VirtPageNum virt_page_num_from_size_t(uint64_t v);
uint64_t size_t_from_virt_addr(VirtAddr v);
uint64_t size_t_from_virt_page_num(VirtPageNum v);
VirtPageNum virt_page_num_from_virt_addr(VirtAddr virt_addr);
PhysPageNum floor_phys(PhysAddr phys_addr);
PhysPageNum ceil_phys(PhysAddr phys_addr);
VirtPageNum floor_virts(VirtAddr virt_addr);
PageTableEntry PageTableEntry_new(PhysPageNum ppn, uint8_t PTEFlags);
PageTableEntry PageTableEntry_empty();
PhysPageNum PageTableEntry_ppn(PageTableEntry *entry);
uint8_t PageTableEntry_flags(PageTableEntry *entry);
bool PageTableEntry_is_valid(PageTableEntry *entry);
uint8_t* get_bytes_array(PhysPageNum ppn);
PageTableEntry* get_pte_array(PhysPageNum ppn);
void indexes_from_vpn(VirtPageNum vpn,size_t* result);
PageTableEntry* find_pte_create(PageTable* pt,VirtPageNum vpn);
PageTableEntry* find_pte(PageTable* pt,VirtPageNum vpn);
void PageTable_map(PageTable* pt,VirtAddr va, PhysAddr pa, uint64_t size ,uint8_t pteflgs);
void PageTable_unmap(PageTable* pt, VirtPageNum vpn);
PageTable kvmmake(void);
void kvminit();
void kvminithart();
void proc_freepagetable(PageTable* pt,uint64_t sz);
#endif
