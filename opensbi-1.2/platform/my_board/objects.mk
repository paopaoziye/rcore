# 编译选项，设置为空
platform-cppflags-y =
platform-cflags-y =
platform-asflags-y =
platform-ldflags-y =

# 需要编译的目标文件
platform-objs-y += platform.o

# 表示固件为JUMP类型，固件的代码段起始地址以及跳转的目标地址
FW_JUMP=y
FW_TEXT_START=0x80000000
FW_JUMP_ADDR=0x0
