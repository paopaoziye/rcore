#ifndef OS_LOADER_H__
#define OS_LOADER_H__

#include "os.h"
#include "task.h"
//前向声明
struct TaskControlBlock;
//存储app二进制文件起始地址和大小
typedef struct {
    uint64_t start;
    uint64_t size;
    uint64_t id;
} AppMetadata;

//ELF文件解析相关宏定义
#define EI_NIDENT 16          //ELF文件头的e_ident数组大小，大小固定为16字节
#define ELFMAG 0x464C457FU    //ELF文件的魔数，即"\x7FELF"
#define EM_RISCV 0xF3         //标识ELF文件的目标机器架构是riscv
#define EI_CLASS     4        //e_ident数组中表示ELF文件类别的字段的索引
#define ELFCLASSNONE 0        //无效类别文件
#define ELFCLASS32   1        //32位ELF文件
#define ELFCLASS64   2        //64位ELF文件
#define ELFCLASSNUM  3        //类别数量
#define PT_LOAD 1             //表示可加载的段
#define PF_X 0x1              //段权限为可执行
#define PF_W 0x2              //段权限为可写
#define PF_R 0x4              //段权限为可读
//ELF64位文件头的抽象
typedef struct {
    uint8_t e_ident[EI_NIDENT];   //e_ident数组，包含魔数、文件类别、字节序、ELF版本等描述信息
    uint16_t e_type;              //文件类型，如可执行文件和共享库等
    uint16_t e_machine;           //目标机器架构
    uint32_t e_version;           //elf版本号
    uint64_t e_entry;             //程序入口地址
    uint64_t e_phoff;             //程序头表偏移量
    uint64_t e_shoff;             //节头表偏移量
    uint32_t e_flags;             //标志位
    uint16_t e_ehsize;            //ELF文件头的大小
    uint16_t e_phentsize;         //程序头表中每个条目的大小
    uint16_t e_phnum;             //程序头表中的条目数量
    uint16_t e_shentsize;         //节头表中每个条目的大小
    uint16_t e_shnum;             //节头表中的条目数量
    uint16_t e_shstrndx;          //节头表中字符串表的索引
} elf64_ehdr_t;
//ELF64位程序头条目的抽象，描述程序各个段的信息
typedef struct {
    uint32_t p_type;              //段类型
    uint32_t p_flags;             //段的权限标志
    uint64_t p_offset;            //段在文件中的偏移量
    uint64_t p_vaddr;             //段的虚拟地址
    uint64_t p_paddr;             //段的物理地址
    uint64_t p_filesz;            //段在文件中的大小
    uint64_t p_memsz;             //段在内存中的大小
    uint64_t p_align;             //段的对齐要求
} elf64_phdr_t;
//函数接口
size_t get_num_app();
AppMetadata  get_app_data(size_t app_id);
void load_app(size_t app_id);
void get_app_names();
AppMetadata  get_app_data_by_name(char* path);
void elf_check(elf64_ehdr_t *ehdr);
void load_segment(elf64_ehdr_t *ehdr,struct TaskControlBlock* proc);
void proc_ustack(struct TaskControlBlock* proc);
#endif