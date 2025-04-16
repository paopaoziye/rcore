    .section .text.entry          //.text.entry表示该段为入口代码段
    .globl _start
_start:                           //设置栈指针并跳转到os_main
    la sp, boot_stack_top
    call os_main

    .section .bss.stack           //.bss.stack表示该段为自定义的未初始化数据段，专用于栈内存
    .globl boot_stack_lower_bound
boot_stack_lower_bound:           //标记栈底端位置
    .space 4096 * 16              //分配栈内存，大小为64KB
    .globl boot_stack_top
boot_stack_top:                   //标记栈顶端位置