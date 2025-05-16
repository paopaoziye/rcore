#include "os.h"
/* 内核态的中断异常处理程序 */
void trap_from_kernel(){
	panic("a trap from kernel!\n");
}
/* 设置内核态的中断异常处理程序 */
void set_kernel_trap_entry(){
	w_stvec((reg_t)trap_from_kernel);
}
/* 设置用户态的中断异常处理程序 */
void set_user_trap_entry(){
	w_stvec((reg_t)TRAMPOLINE);  
}

void trap_handler(){
	//进入内核态后，修改 stvec 寄存器以禁止内核触发中断异常
	set_kernel_trap_entry();
	TrapContext* cx = (TrapContext*)get_current_trap_cx();

    reg_t scause = r_scause();
	reg_t cause_code = scause & 0xfff;
	if(scause & 0x8000000000000000)
	{
		switch (cause_code)
		{
		/* rtc 中断*/
		case 5:
			set_next_trigger();
			schedule();
			break;
		default:
			printk("undfined interrrupt scause:%x\n",scause);
			break;
		}
	}
	else
	{
		switch (cause_code)
		{
		/* U模式下的syscall */
		case 8:
			cx->a0 = __SYSCALL(cx->a7,cx->a0,cx->a1,cx->a2);
			cx->sepc += 8;
			break;
		default:
			printk("undfined exception scause:%x\n",scause);
			break;
		}
	}
	trap_return();
}

void trap_return(){  
    /* 把 stvec 设置为内核和应用地址空间共享的跳板页面的起始地址 */  
    set_user_trap_entry();
    /* Trap 上下文在应用地址空间中的虚拟地址 */  
    uint64_t trap_cx_ptr = TRAPFRAME;  
    /* 要继续执行的应用地址空间的 token */  
    uint64_t  user_satp = current_user_token();  
	/* 计算_restore函数的虚拟地址 */
    uint64_t  restore_va = (uint64_t)__restore - (uint64_t)__alltraps + TRAMPOLINE;  
	asm volatile (    
			"fence.i\n\t"    
			"mv a0, %0\n\t"  // 将trap_cx_ptr传递给a0寄存器  
			"mv a1, %1\n\t"  // 将user_satp传递给a1寄存器  
			"jr %2\n\t"      // 跳转到restore_va的位置执行代码  
			:    
			: "r" (trap_cx_ptr),    
			"r" (user_satp),
			"r" (restore_va)
			: "a0", "a1"
		);
  
}