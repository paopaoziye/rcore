#include "loader.h"
#include "os.h"
#include "alloc.h"

//link_app.S中定义的_num_app，为app各种元信息
extern uint64_t _num_app[];

/* 获取加载的app数量 */
size_t get_num_app(){
    return _num_app[0];
}
/* 获取app的元信息 */
AppMetadata  get_app_data(size_t app_id)
{
    AppMetadata metadata;

    size_t num_app = get_num_app();

    metadata.start = _num_app[app_id];  

    metadata.size = _num_app[app_id+1] - _num_app[app_id];

    assert(app_id <= num_app);

    return metadata;
}
static uint8_t flags_to_mmap_prot(uint8_t flags){
    return (flags & PF_R ? PTE_R : 0) | 
           (flags & PF_W ? PTE_W : 0) |
           (flags & PF_X ? PTE_X : 0);
}
/*  */
void load_app(size_t app_id){
    printk("loading app\n");
    AppMetadata  metadata = get_app_data(app_id+1);
    elf64_ehdr_t *ehdr = (elf64_ehdr_t *)metadata.start;
    assert(*(uint32_t *)ehdr==ELFMAG);
    printk("magic : %p!\n",*(uint32_t *)ehdr);
    if (ehdr->e_machine != EM_RISCV || ehdr->e_ident[EI_CLASS] != ELFCLASS64){
        panic("only riscv64 elf file is supported");
    }
    uint64_t entry = (uint64_t)ehdr->e_entry;
    printk("entry : %p!\n",ehdr->e_entry);
    printk("init task%d\n",app_id);
    TaskControlBlock* proc = task_create_pt(app_id);
    proc->entry = entry;
    elf64_phdr_t *phdr;
    for (size_t i = 0; i < ehdr->e_phnum; i++)
    {
        //拿到当前程序头条目的指针
        phdr =(elf64_phdr_t *)(ehdr->e_phoff + ehdr->e_phentsize * i + metadata.start);
        printk("phdr%d:%p\n",i,phdr);
        if(phdr->p_type == PT_LOAD)
        {
            // 获取映射内存段开始位置
            uint64_t start_va = phdr->p_vaddr;
            // 获取映射内存段结束位置
            proc->ustack = start_va + phdr->p_memsz;
            //  转换elf的可读，可写，可执行的 flags
            uint8_t map_perm = PTE_U | flags_to_mmap_prot(phdr->p_flags);
            // 获取映射内存大小,需要向上对齐
            uint64_t map_size = PGROUNDUP(phdr->p_memsz);
            for (size_t j = 0; j < map_size; j+= PAGE_SIZE)
            {
                // 分配物理内存，加载程序段，然后映射
                PhysPageNum ppn = kalloc();
                //获取到分配的物理内存的地址
                uint64_t paddr = phys_addr_from_phys_page_num(ppn).value;
                memcpy((void*)paddr, (void*)(metadata.start + phdr->p_offset + j), PAGE_SIZE);
                //内存逻辑段内存映射
                PageTable_map(&proc->pagetable,virt_addr_from_size_t(start_va + j), \
                                phys_addr_from_size_t(paddr), PAGE_SIZE , map_perm);
            }
        
        }
    }
    // 映射应用程序用户栈开始地址
    proc->ustack =  2 * PAGE_SIZE + PGROUNDUP(proc->ustack);
    PhysPageNum ppn = kalloc();
    uint64_t paddr = phys_addr_from_phys_page_num(ppn).value;
    PageTable_map(&proc->pagetable,virt_addr_from_size_t(proc->ustack - PAGE_SIZE),phys_addr_from_size_t(paddr), \
                  PAGE_SIZE, PTE_R | PTE_W | PTE_U);
}
