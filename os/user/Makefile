CROSS_COMPILE = riscv64-unknown-elf-
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany

CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
INCLUDE:=-I../include/os

LIB = ../lib
BIN_DIR = bin

all: $(BIN_DIR) init_proc exc user_shell time

# 创建bin目录的规则
$(BIN_DIR):
	mkdir -p $@

write: write.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld $^ -o bin/write 

time: time.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld $^ -o bin/time 

user_shell:user_shell.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld  $^ -o bin/user_shell 

exc: exc.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld  $^ -o bin/xec 

init_proc: init_proc.c $(LIB)/*.c
	${CC} ${CFLAGS} $(INCLUDE) -T user.ld  $^ -o bin/init_proc

debug: objdump_time objdump_write objdump_exc

objdump_time:
	${OBJDUMP} -d bin/time > time.txt
objdump_write:
	${OBJDUMP} -d bin/write > write.txt
objdump_exc:
	${OBJDUMP} -d bin/exc > exc.txt