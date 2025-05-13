#include "address.h"

/* 从一个uint64_t中提取物理地址 */
PhysAddr phys_addr_from_size_t(uint64_t v){
    PhysAddr addr;
    //获取低 PA_WIDTH_SV39 位为1的掩码从而提取物理地址
    addr.value = v & ((1ULL << PA_WIDTH_SV39) - 1);
    return addr;
}
/* 从一个uint64_t中提取物理页号 */
PhysPageNum phys_page_num_from_size_t(uint64_t v){
    PhysPageNum pageNum;
    pageNum.value = v & ((1ULL << PPN_WIDTH_SV39) - 1);
    return pageNum;
}
/* 将一个物理地址转化为uint64_t */
uint64_t size_t_from_phys_addr(PhysAddr v){
    return v.value;
}
/* 将一个物理页号转化为uint64_t */
uint64_t size_t_from_phys_page_num(PhysPageNum v){
    return v.value;
}
/* 从物理页号转换为实际物理地址 */
PhysAddr phys_addr_from_phys_page_num(PhysPageNum ppn){
    PhysAddr addr;
    addr.value = ppn.value << PAGE_SIZE_BITS ;
    return addr;
}
/* 从一个uint64_t中提取虚拟地址 */
VirtAddr virt_addr_from_size_t(uint64_t v){
    VirtAddr addr;
    addr.value = v & ((1ULL << VA_WIDTH_SV39) - 1);
    return addr;
}
/* 从一个uint64_t中提取虚拟页号 */
VirtPageNum virt_page_num_from_size_t(uint64_t v){
    VirtPageNum pageNum;
    pageNum.value = v & ((1ULL << VPN_WIDTH_SV39) - 1);
    return pageNum;
}
/* 将一个虚拟地址转化为uint64_t */
uint64_t size_t_from_virt_addr(VirtAddr v){
    if (v.value >= (1ULL << (VA_WIDTH_SV39 - 1))) {
        return v.value | ~((1ULL << VA_WIDTH_SV39) - 1);
    } else {
        return v.value;
    }
}
/* 将一个虚拟页号转化为uint64_t */
uint64_t size_t_from_virt_page_num(VirtPageNum v){
    return v.value;
}
/* 把虚拟地址转换为虚拟页号 */
VirtPageNum virt_page_num_from_virt_addr(VirtAddr virt_addr){
    VirtPageNum vpn;
    vpn.value =  virt_addr.value / PAGE_SIZE;
    return vpn;
}
/* 物理地址向下取整 */
PhysPageNum floor_phys(PhysAddr phys_addr){
    PhysPageNum phys_page_num;
    phys_page_num.value = phys_addr.value / PAGE_SIZE;
    return phys_page_num;
}

/* 物理地址向上取整 */
PhysPageNum ceil_phys(PhysAddr phys_addr){
    PhysPageNum phys_page_num;
    phys_page_num.value = (phys_addr.value + PAGE_SIZE - 1) / PAGE_SIZE;
    return phys_page_num;
}

