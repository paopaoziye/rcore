#ifndef OS_ADDRESS_H
#define OS_ADDRESS_H
#include "os.h"


//相关宏定义
#define PAGE_SIZE 0x1000      //页的大小即4KB  
#define PAGE_SIZE_BITS   0xc  //页内偏移地址长度即12位   

#define PA_WIDTH_SV39 56      //物理地址长度
#define VA_WIDTH_SV39 39      //虚拟地址长度
#define PPN_WIDTH_SV39 (PA_WIDTH_SV39 - PAGE_SIZE_BITS)  // 物理页号 44位 [55:12]
#define VPN_WIDTH_SV39 (VA_WIDTH_SV39 - PAGE_SIZE_BITS)  // 虚拟页号 27位 [38:12]

#define MEMORY_END 0x80800000    //空闲空间起始地址
#define MEMORY_START 0x80400000  //空闲空间末尾地址

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
#endif