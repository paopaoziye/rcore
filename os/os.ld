OUTPUT_ARCH(riscv)
ENTRY(_start)
BASE_ADDRESS = 0x80200000;
MEMORY
{ 
	ram (rxai!w) : ORIGIN = 0x80200000, LENGTH = 128M
}
SECTIONS
{
    skernel = .;   /* 定义内核起始内存地址 */
	.text : {
        *(.text.entry)
        . = ALIGN(4K);
        trampoline = .;
        *(.text.trampoline);
        . = ALIGN(4K);
		_strampoline = .;
		*(.text .text.*)
		. = ALIGN(4K);
		PROVIDE(etext = .);
	} >ram

	.rodata : {
		*(.rodata .rodata.*)
	} >ram

	.data : {
		. = ALIGN(4096);
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >ram

	.bss :{
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
	} >ram

	PROVIDE(kernelend = .);
}