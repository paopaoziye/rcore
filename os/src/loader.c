#include "loader.h"
#include "os.h"
#include "alloc.h"

//link_app.S中定义的_num_app，为app各种元信息
extern uint64_t _num_app[];
extern char _app_names[];
static char* app_names[MAX_TASKS];
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

    metadata.id = app_id;

    assert(app_id <= num_app);

    return metadata;
}
AppMetadata  get_app_data_by_name(char* path){
    AppMetadata metadata;
    size_t num_app = get_num_app();
    for(size_t i = 0;i < num_app;i++){
        if(strcmp(path,app_names[i]) == 0){
            metadata = get_app_data(i+1);
            printk("find app:%s id:%d\n",path,metadata.id);
            return metadata;
        }
    }
    printk("not exit!!\n");
    return metadata;
}

static uint8_t flags_to_mmap_prot(uint8_t flags){
    return (flags & PF_R ? PTE_R : 0) | 
           (flags & PF_W ? PTE_W : 0) |
           (flags & PF_X ? PTE_X : 0);
}
/* 检查ELF文件 */
void elf_check(elf64_ehdr_t *ehdr){
    assert(*(uint32_t *)ehdr==ELFMAG);
    if (ehdr->e_machine != EM_RISCV || ehdr->e_ident[EI_CLASS] != ELFCLASS64){
        panic("only riscv64 elf file is supported");
    }
}
/* 加载ELF文件并映射 */
void load_segment(elf64_ehdr_t *ehdr,struct TaskControlBlock* proc){
    elf64_phdr_t *phdr;
    for (size_t i = 0; i < ehdr->e_phnum; i++)
    {
        //拿到当前程序头条目的指针
        phdr =(elf64_phdr_t *)(ehdr->e_phoff + ehdr->e_phentsize * i + (uint64_t)ehdr);
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
                memcpy((void*)paddr, (void*)((uint64_t)ehdr + phdr->p_offset + j), PAGE_SIZE);
                //内存逻辑段内存映射
                PageTable_map(&proc->pagetable,virt_addr_from_size_t(start_va + j), \
                                phys_addr_from_size_t(paddr), PAGE_SIZE , map_perm);
            }
        
        }
    }
    proc->ustack =  2 * PAGE_SIZE + PGROUNDUP(proc->ustack);
    proc->base_size = proc->ustack;
}
void proc_ustack(struct TaskControlBlock* proc){
    PhysPageNum ppn = kalloc();
    uint64_t paddr = phys_addr_from_phys_page_num(ppn).value;
    PageTable_map(&proc->pagetable,virt_addr_from_size_t(proc->ustack - PAGE_SIZE),phys_addr_from_size_t(paddr), \
                  PAGE_SIZE, PTE_R | PTE_W | PTE_U);
}
/* 加载app */
void load_app(size_t app_id){
    //读取ELF文件头
    AppMetadata  metadata = get_app_data(app_id+1);
    elf64_ehdr_t *ehdr = (elf64_ehdr_t *)metadata.start;
    //检查ELF文件
    elf_check(ehdr);
    //创建任务
    TaskControlBlock* proc = task_create_pt(app_id);
    //加载程序段
    load_segment(ehdr,proc);
    //设置人物的entry
    proc->entry = (uint64_t)ehdr->e_entry;
    //映射应用程序的用户栈
    proc_ustack(proc);

}
/* 获取app的名称 */
void get_app_names(){
    int app_num = get_num_app();
    printk("/**** APP LIST ****\n");
    for(size_t i = 0;i < app_num;i++){
        if(i == 0){
            size_t len = strlen(_app_names);
            app_names[0] = _app_names;
        }else{
            size_t len = strlen(app_names[i-1]);
            app_names[i] = (char*)((uint64_t)app_names[i-1] + len + 1);
        }
        printk("%s\n",app_names[i]);
    }
    printk("**** APP LIST ****/\n");
}