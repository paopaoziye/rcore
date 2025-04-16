OUTPUT_ARCH(riscv)
ENTRY(_start)
/* 定义一块名为ram的内存区域 */
MEMORY
{ 
	ram (rxai!w) : ORIGIN = 0x80200000, LENGTH = 128M
}
/* 定义程序各个段如何放入内存 */
SECTIONS
{
	.text : {
		*(.text .text.*)
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

}