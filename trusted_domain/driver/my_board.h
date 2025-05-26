#ifndef MY_BOARD_H_
#define MY_BOARD_H_

#ifdef __ASSEMBLER__
#define CONS(NUM,TYPE)NUM
#else
#define CONS(NUM,TYPE)NUM##TYPE
#endif

#define PRIM_HART          7//定义CPU启动核
#define CLINT



#endif