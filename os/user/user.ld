OUTPUT_ARCH(riscv)
ENTRY(main)
BASE_ADDRESS = 0x10000;

SECTIONS
{
    /* 设置基址，填充代码段、只读数据段、可读写数据段和未初始化数据段 */
    . = BASE_ADDRESS;
    .text : {
        *(.text.entry)
        *(.text .text.*)
    }
    . = ALIGN(4K);
    .rodata : {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    }
    . = ALIGN(4K);
    .data : {
        *(.data .data.*)
        *(.sdata .sdata.*)
    }
    .bss : {
        *(.bss .bss.*)
        *(.sbss .sbss.*)
    }
    /* 丢弃调试信息和其他无用段 */
    /DISCARD/ : {
        *(.eh_frame)
        *(.debug*)
    }
}