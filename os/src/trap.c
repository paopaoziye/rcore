#include "os.h"
#include "context.h"
#include "riscv.h"

/* 异常/中断处理函数，根据 scause 寄存器的值进行不同的处理 */
TrapContext* trap_handler(TrapContext* cx)
{
	//读取 scause 寄存器的值，并提取其低12位
    reg_t scause = r_scause();
	reg_t cause_code = scause & 0xfff;
	//根据 scause 寄存器的最高位判断是中断还是异常
	if(scause & 0x8000000000000000)
	{
		//根据 cause_code 判断中断类型
		switch (cause_code)
		{
		//时钟中断处理，设置下一次时钟中断并出发任务调度
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
		//根据 cause_code 判断异常类型
		switch (cause_code)
		{
		//触发系统调用
		case 8:
			cx->a0 = __SYSCALL(cx->a7,cx->a0,cx->a1,cx->a2);
			cx->sepc += 8;
			break;
		default:
			printk("undfined exception scause:%x\n",scause);
			break;
		}
	}
	return cx;
}

/* 将stvec即发生异常时处理函数地址设置为__alltraps */
void trap_init()
{
	w_stvec((reg_t)__alltraps);
}