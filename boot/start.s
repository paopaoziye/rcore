//定义宏loop，参数为cunt
.macro      loop,cunt                
    li		t1,	0xffff                    //将t1设置为0xffff
    li		t2,	\cunt                     //将t2设置为cunt
1:
	nop                                   //空操作
	addi    t1, t1, -1                    //t1 = t1-1
	bne		t1, x0, 1b                    //若t1 != 0，跳转到标签1
    li		t1,	0xffff                    //重置t1 = 0xffff
	addi    t2, t2, -1                    //t2 = t2-1
	bne		t2, x0, 1b                    //若t2 != 0，跳转到标签1
.endm                                     

//定义宏load_data，将_src_start处的数据逐字拷贝到_dst_start至_dst_end
.macro      load_data,_src_start,_dst_start,_dst_end
	bgeu	\_dst_start, \_dst_end, 2f    //如果_dst_start >= _dst_end，跳转到2结束
1:
	lw      t0, (\_src_start)             //从_src_start加载32位数据到t0
	sw      t0, (\_dst_start)             //将t0的值存储到_dst_start
	addi    \_src_start, \_src_start, 4   //_src_start指向下一个字
	addi    \_dst_start, \_dst_start, 4   //_dst_start指向下一个字
	bltu    \_dst_start, \_dst_end, 1b    //如果_dst_start < _dst_end，跳转到1继续拷贝
2:
.endm

	.section .text
	.globl _start
	.type _start,@function

_start:
	//将opensbi_fw.bin从0x20200000拷贝到0x80000000处
    li		a0,	0x202
	slli	a0,	a0, 20      //a0 = 0x20200000
    li		a1,	0x800
	slli	a1,	a1, 20      //a1 = 0x80000000
    li		a2,	0x802
	slli	a2,	a2, 20      //a2 = 0x80200000
	load_data a0,a1,a2

	//将qemu_sbi.dtb从0x20080000拷贝到0x82200000处
    li		a0,	0x2008
	slli	a0,	a0, 16       //a0 = 0x20080000
    li		a1,	0x822
	slli	a1,	a1, 20       //a1 = 0x82200000
    li		a2,	0x8228
	slli	a2,	a2, 16       //a2 = 0x82280000
	load_data a0,a1,a2

    csrr    a0, mhartid      //读取当前hart id
    li		t0,	0x0          
	beq		a0, t0, _no_wait //若为hart0，则跳转到_no_wait
	loop	0x1000           //其他hart循环等待
_no_wait:
    li		a1,	0x822
	slli	a1,	a1, 20       //a1 = 0x82200000，opensbi从a1中读取设备树地址
    li	    t0,	0x800
	slli	t0,	t0, 20       //t0 = 0x80000000
    jr      t0               //跳转到opensbi入口处运行

    .end