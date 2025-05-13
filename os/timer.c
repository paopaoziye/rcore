#include "os.h"
//定义时钟频率和时钟中断频率
#define CLOCK_FREQ 10000000
#define TICKS_PER_SEC 500

/* 设置下次时钟中断的 cnt 值 */
void set_next_trigger(){
    sbi_set_timer(r_mtime() + CLOCK_FREQ/TICKS_PER_SEC);
}
/* 时钟中断初始化 */
void timer_init(){
    //使能S模式所有中断
    reg_t sstatus = r_sstatus();
    sstatus |= (1L << 1);
    w_sstatus(sstatus);
    //使能S模式时钟中断
    reg_t sie = r_sie();
    sie |= SIE_STIE;
    w_sie(sie);
    //设置下次时钟中断
    set_next_trigger();
}
/* 以2us为单位返回时间 */
uint64_t get_time_us(){
    reg_t time = r_mtime() / (CLOCK_FREQ/TICKS_PER_SEC);
    return time;
}
