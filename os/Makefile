#指定交叉编译工具链前缀
CROSS_COMPILE = riscv64-unknown-elf-
#编译选项：不链接标准库、禁用GCC内置函数优化并使用medany代码模型
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany

#定义对应的编译工具
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

#汇编源文件列表
SRCS_ASM = \
	entry.S \
	kerneltrap.S \
	switch.S \
#c源文件列表
SRCS_C = \
	sbi.c \
	main.c \
	printk.c \
	trap.c \
	syscall.c \
	string.c \
	app.c \
	task.c \
	timer.c \
	printf.c \
	vsprintf.c \
	address.c \
	alloc.c \
	stack.c\

#相关路径
INCLUDE:=-I./include/os
SRC:=./src
LIB:=./lib
#将源文件列表转化为对应的目标文件列表，避免手动列出每个.o文件
OBJS = $(SRCS_ASM:.S=.o)
OBJS += $(SRCS_C:.c=.o)

#os.elf的依赖文件为OBJS，只有OBJS中的某个.o文件更新时，才会重新构建os.elf
#将.o文件编译为os.elf，并基于os.elf生成对应的os.bin
os.elf: ${OBJS}
	${CC} ${CFLAGS} -T os.ld -Wl,-Map=os.map -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin
#将所有的.c、.S文件编译为.o，指定头文件路径
%.o : $(SRC)/%.c
	${CC} ${CFLAGS} $(INCLUDE) -c -o $@ $<

%.o : $(LIB)/%.c
	${CC} ${CFLAGS} $(INCLUDE) -c -o $@ $<

%.o : $(SRC)/%.S
	${CC} ${CFLAGS} $(INCLUDE) -c -o $@ $<

#声明clean不是一个真实的目标文件，而是一个抽象的操作命令
#删除当前目录下所有.o、.bin和 .elf文件
.PHONY : clean
clean:
	rm -rf *.o *.bin *.elf
