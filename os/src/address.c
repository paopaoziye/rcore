#include "address.h"
#include "alloc.h"

//内核页表
PageTable kernel_pagetable;
uint64_t kernel_satp;
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
/* 虚拟地址向下取整 */
VirtPageNum floor_virts(VirtAddr virt_addr){
    VirtPageNum virt_page_num;
    virt_page_num.value = virt_addr.value / PAGE_SIZE;
    return virt_page_num;
}
/* 新建一个页表项 */
PageTableEntry PageTableEntry_new(PhysPageNum ppn, uint8_t PTEFlags){
    PageTableEntry entry;
    entry.bits = (ppn.value << 10) | PTEFlags;
    return entry;
}
/* 创建空页表项 */
PageTableEntry PageTableEntry_empty(){
    PageTableEntry entry;
    entry.bits = 0;
    return entry;
}
/* 获取页表项的物理页号 */
PhysPageNum PageTableEntry_ppn(PageTableEntry *entry){
    PhysPageNum ppn;
    ppn.value = (entry->bits >> 10) & ((1ul << 44) - 1);
    return ppn;
}
/* 获取页表项的标志位 */
uint8_t PageTableEntry_flags(PageTableEntry *entry){
    return entry->bits & 0xFF;
}
/* 判断页表项是否为空 */
bool PageTableEntry_is_valid(PageTableEntry *entry){
    uint8_t entryFlags = PageTableEntry_flags(entry);
    return (entryFlags & PTE_V) != 0;
}

/* 将物理页号转换为以字节为单位的物理地址 */
uint8_t* get_bytes_array(PhysPageNum ppn){
    PhysAddr addr = phys_addr_from_phys_page_num(ppn);
    return (uint8_t*) addr.value;
}
/* 将物理页号转换为以页表项为单位的物理地址 */
PageTableEntry* get_pte_array(PhysPageNum ppn){
    PhysAddr addr = phys_addr_from_phys_page_num(ppn);
    return (PageTableEntry*) addr.value;
}

/* 从虚拟页号中提取三级页号索引，按照从高到低的顺序返回 */
void indexes_from_vpn(VirtPageNum vpn,size_t* result){
    size_t idx[3];
    for(int i = 2;i >= 0;i--){
        //提取vpn的低9位并将其右移9位
        idx[i] = vpn.value & 0x1ff;
        vpn.value >>= 9;
    }
    for (int i = 0; i < 3; i++) {
        result[i] = idx[i];
    }
}

/* 查找并填充页表项 */
PageTableEntry* find_pte_create(PageTable* pt,VirtPageNum vpn){
    //虚拟页号三级索引
    size_t idx[3];
    indexes_from_vpn(vpn,idx);
    //提取页表根节点
    PhysPageNum ppn = pt->root_ppn;
    //提取各级页表项，如果没有pte，则分配一页内存并创建
    for(int i = 0;i < 3;i++){
        //提取页表项
        PageTableEntry* pte = &get_pte_array(ppn)[idx[i]];
        if(i == 2){
            return pte;
        }
        //如果页表项为空，则分配一个新的物理页并新建一个页表项
        if(!PageTableEntry_is_valid(pte)){
            PhysPageNum frame = StackFrameAllocator_alloc(&FrameAllocatorImpl);
            *pte = PageTableEntry_new(frame,PTE_V);
        }
        //更新页表为下一级页表
        ppn = PageTableEntry_ppn(pte);
    }
}
/* 查找页表项 */
PageTableEntry* find_pte(PageTable* pt,VirtPageNum vpn){
    //虚拟页号三级索引
    size_t idx[3];
    indexes_from_vpn(vpn,idx);
    //提取页表根节点
    PhysPageNum ppn = pt->root_ppn;
    //提取各级页表项，如果没有pte，则分配一页内存并创建
    for(int i = 0;i < 3;i++){
        //提取页表项
        PageTableEntry* pte = &get_pte_array(ppn)[idx[i]];
        //如果页表项为空，则分配一个新的物理页并新建一个页表项
        if(!PageTableEntry_is_valid(pte)){
            return NULL;
        }
        if(i == 2){
            return pte;
        }
        //更新页表为下一级页表
        ppn = PageTableEntry_ppn(pte);
    }
}
/* 建立虚拟地址和物理地址的映射 */
void PageTable_map(PageTable* pt,VirtAddr va, PhysAddr pa, uint64_t size ,uint8_t pteflgs){
    if(size == 0)
        panic("mappages:size");
    //向下取整需要映射虚拟地址和物理地址，获取其页号
    PhysPageNum ppn = floor_phys(pa);
    VirtPageNum vpn = floor_virts(va);  
    //获取标志映射结束的虚拟页号
    uint64_t last =  (va.value + size - 1) / PAGE_SIZE;
    for(;;){
        //找到va对应的页表项，并确保其三级页表项为空，从而创建三级页表项完成映射
        PageTableEntry* pte = find_pte_create(pt,vpn);
        assert(!PageTableEntry_is_valid(pte));
        *pte = PageTableEntry_new(ppn,PTE_V | pteflgs);

        if( vpn.value == last )
            break;
        
        vpn.value += 1;
        ppn.value += 1;
    }
}
/* 取消映射 */
void PageTable_unmap(PageTable* pt, VirtPageNum vpn){
    //清空对应三级页表项
    PageTableEntry* pte = find_pte(pt,vpn);
    assert(!PageTableEntry_is_valid(pte));
    *pte = PageTableEntry_empty();
}
/* 内核虚拟内存映射，采用恒等内存映射 */
PageTable kvmmake(void){
    //创建页表根节点并打印其物理地址
    PageTable pt;
    PhysPageNum root_ppn =  StackFrameAllocator_alloc(&FrameAllocatorImpl);
    pt.root_ppn = root_ppn;
    //映射内核代码段并打印其结束地址，其权限为可执行和只读
    PageTable_map(&pt,virt_addr_from_size_t(KERNBASE),phys_addr_from_size_t(KERNBASE), \
                    (uint64_t)etext-KERNBASE , PTE_R | PTE_X ) ;
    //映射内核数据段和RAM，其权限为可读写
    PageTable_map(&pt,virt_addr_from_size_t((uint64_t)etext),phys_addr_from_size_t((uint64_t)etext ), \
                    PHYSTOP - (uint64_t)etext , PTE_R | PTE_W ) ;
    //映射跳板页
    PageTable_map(&pt, virt_addr_from_size_t(TRAMPOLINE), phys_addr_from_size_t((uint64_t)trampoline), \
                    PAGE_SIZE, PTE_R | PTE_X );     
    proc_mapstacks(&pt);
    return pt;
}
/* 创建内核页表 */
void kvminit(){
  kernel_pagetable = kvmmake();
  kernel_satp = MAKE_SATP(kernel_pagetable.root_ppn.value);
}
/* 开启分页模式并将内核页表写入satp寄存器 */
void kvminithart(){
  //构造riscv的 satp 寄存器值
  sfence_vma();
  //将上述构造的 satp 寄存器值写入 satp 寄存器
  w_satp(MAKE_SATP(kernel_pagetable.root_ppn.value));
  //刷新TLB
  sfence_vma();
  //打印当前satp寄存器的值
  reg_t satp = r_satp();
}
/* 取消映射 */
void uvmunmap(PageTable* pt,VirtPageNum vpn,uint64_t npages,int do_free){
    PageTableEntry* pte;
    uint64_t i;
    for(i = vpn.value;i < vpn.value + npages;i++){
        pte = find_pte(pt,virt_page_num_from_size_t(i));
        if(pte != 0){
            if(do_free){
                uint64_t phyaddr = PTE2PA(pte->bits);
                PhysPageNum ppn = floor_phys(phys_addr_from_size_t(phyaddr));
                kfree(ppn);
            }
            *pte = PageTableEntry_empty();
        }
    }

}
/* 解除页表映射关系并释放内存 */
void freewalk(PhysPageNum ppn){
    //遍历页表项
    for(int i = 0;i < 512;i++){
        PageTableEntry* pte = &get_pte_array(ppn)[i];
        if((pte->bits & PTE_V) && (pte->bits & (PTE_R | PTE_W | PTE_X)) == 0){
            PhysPageNum child_ppn = PageTableEntry_ppn(pte);
            freewalk(child_ppn);
            *pte = PageTableEntry_empty();
        }else if(pte->bits & PTE_V){
            panic("freewalk:leaf!");
        }
    }
    kfree(ppn);
}
void uvmfree(PageTable* pt,uint64_t sz){
    if(sz > 0){
        uvmunmap(pt,floor_virts(virt_addr_from_size_t(0)),sz/PAGE_SIZE,1);
    }
    freewalk(pt->root_ppn);
}
/* 销毁进程地址空间 */
void proc_freepagetable(PageTable* pt,uint64_t sz){
    uvmunmap(pt,floor_virts(virt_addr_from_size_t(TRAMPOLINE)),1,0);
    uvmunmap(pt,floor_virts(virt_addr_from_size_t(TRAPFRAME)),1,0);
    uvmfree(pt,sz);
}