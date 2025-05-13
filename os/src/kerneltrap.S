#异常(trap)的入口
.globl __alltraps
.align 4   
__alltraps:
    # 交换sp和sscratch的值，这样sp指向内核栈，sscratch存储用户栈的地址
    csrrw sp, sscratch, sp
    # 在内核栈为TrapContext分配空间，大小为34个8字节
    addi sp, sp, -34*8
    # 保存通用寄存器，其中sp(x2)不需要保存，因为已经保存在sscratch中
    sd x1, 1*8(sp)
    sd x3, 3*8(sp)
    sd x4, 4*8(sp)
    sd x5, 5*8(sp)
    sd x6, 6*8(sp)
    sd x7, 7*8(sp)
    sd x8, 8*8(sp)
    sd x9, 9*8(sp)
    sd x10,10*8(sp)
    sd x11, 11*8(sp)
    sd x12, 12*8(sp)
    sd x13, 13*8(sp)
    sd x14, 14*8(sp)
    sd x15, 15*8(sp)
    sd x16, 16*8(sp)
    sd x17, 17*8(sp)
    sd x18, 18*8(sp)
    sd x19, 19*8(sp)
    sd x20, 20*8(sp)
    sd x21, 21*8(sp)
    sd x22, 22*8(sp)
    sd x23, 23*8(sp)
    sd x24, 24*8(sp)
    sd x25, 25*8(sp)
    sd x26, 26*8(sp)
    sd x27, 27*8(sp)
    sd x28, 28*8(sp)
    sd x29, 29*8(sp)
    sd x30, 30*8(sp)
    sd x31, 31*8(sp)

    # 读取 sstatus 和 sepc 并保存
    csrr t0, sstatus
    csrr t1, sepc
    sd t0, 32*8(sp)
    sd t1, 33*8(sp)
    # 读取 sscratch 并保存
    csrr t2, sscratch
    sd t2, 2*8(sp)
    # 将 sp 的值传递给 a0，从而trap_handler可以访问用户程序通过内核栈传递的数据
    mv a0, sp
    call trap_handler

# 异常(trap)返回
.globl __restore
.align 4
__restore:
    #将sp指向a0，即TrapContext处，此处需要注释掉
    #mv sp, a0
    #从内核栈中加载并还原sstatus、sepc和sscratch
    ld t0, 32*8(sp)
    ld t1, 33*8(sp)
    ld t2, 2*8(sp)
    csrw sstatus, t0
    csrw sepc, t1
    csrw sscratch, t2
    #还原通用寄存器
    ld x1, 1*8(sp)
    ld x3, 3*8(sp)
    ld x4, 4*8(sp)
    ld x5, 5*8(sp)
    ld x6, 6*8(sp)
    ld x7, 7*8(sp)
    ld x8, 8*8(sp)
    ld x9, 9*8(sp)
    ld x10,10*8(sp)
    ld x11, 11*8(sp)
    ld x12, 12*8(sp)
    ld x13, 13*8(sp)
    ld x14, 14*8(sp)
    ld x15, 15*8(sp)
    ld x16, 16*8(sp)
    ld x17, 17*8(sp)
    ld x18, 18*8(sp)
    ld x19, 19*8(sp)
    ld x20, 20*8(sp)
    ld x21, 21*8(sp)
    ld x22, 22*8(sp)
    ld x23, 23*8(sp)
    ld x24, 24*8(sp)
    ld x25, 25*8(sp)
    ld x26, 26*8(sp)
    ld x27, 27*8(sp)
    ld x28, 28*8(sp)
    ld x29, 29*8(sp)
    ld x30, 30*8(sp)
    ld x31, 31*8(sp)

    #将TrapContext从内核栈中释放
    addi sp, sp, 34*8
    #交换sp和sscratch的值，此时sp指向用户栈，sscratch存储内核栈的地址
    csrrw sp, sscratch, sp
    #将CPU特权级从S降为U，并跳转到sepc指向的地址
    sret

