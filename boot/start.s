.section .text
.global  _start
.type    _start,@function

_start:
    csrr a0,mhartid            //从csr寄存器中读取内核的hart id并存储到a0寄存器中
    li   t0,0x0                //将立即数0加载到t0
    beq  a0,t0,_core0          //若a0 = t0，则跳转到_core0
_loop:
    j    _loop                 //若不跳转，则进入无限循环
_core0:
    li   t0,0x100              //将t0设置为100
    slli t0,t0,20              //将t0左移20位，即t0 = 0x10000000
    li   t1,'H'                //将t1设置为'H'
    sb   t1,0(t0)              //将t0的值写入t1对应的地址，即uart0的数据发送寄存器
    li   t1,'i'
    sb   t1,0(t0)
    li   t1,'!'
    sb   t1,0(t0)
    j    _loop                 //防止程序跑飞
.end