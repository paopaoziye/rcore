#指定交叉编译工具链前缀
CROSS_COMPILE = riscv64-unknown-elf-
#编译选项：不链接标准库、禁用GCC内置函数优化、使用medany代码模型
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany
#定义对应的编译工具
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
#相关路径
INCLUDE:=-I../include/os
LIB = ../lib
#依赖文件为write.c和lib下所有.c文件，只有依赖文件发生变化时才会重新构建，链接脚本为user.ld
#编译链接依赖文件形成可执行文件bin/write和内存映射文件write.map
write: write.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld -Wl,-Map=write.map -o bin/write $^
#同上，形成可执行文件bin/time和内存映射文件time.map
time: time.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld -Wl,-Map=time.map -o bin/time $^
#生成time和write的反汇编文件time.txt和write.txt
objdump_time:
	${OBJDUMP} -d bin/time > time.txt
objdump_write:
	${OBJDUMP} -d bin/write > write.txt