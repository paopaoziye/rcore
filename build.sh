# 获取当前脚本文件所在的目录
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 如果没有output目录则创建
if [ ! -d "$SHELL_FOLDER/output" ]; then  
mkdir $SHELL_FOLDER/output
fi  
# 切换到qemu-8.0.2
cd qemu-8.0.2
# 如果没有output/qemu目录则运行config脚本进行配置，指定安装目录为$SHELL_FOLDER/output/qemu
if [ ! -d "$SHELL_FOLDER/output/qemu" ]; then  
./configure --prefix=$SHELL_FOLDER/output/qemu  --target-list=riscv64-softmmu --enable-gtk  --enable-virtfs --disable-gio
fi  
# 编译并安装qemu
make -j16$PROCESSORS
make install
cd ..

# 指定编译工具链前缀为riscv64-unknown-elf
CROSS_PREFIX=riscv64-unknown-elf
# 如果没有output/lowlevelboot目录则创建
if [ ! -d "$SHELL_FOLDER/output/lowlevelboot" ]; then  
mkdir $SHELL_FOLDER/output/lowlevelboot
fi  
# 切换到boot
cd boot

# 将start.s编译为start.o
$CROSS_PREFIX-gcc -x assembler-with-cpp -c start.s -o $SHELL_FOLDER/output/lowlevelboot/start.o
# 将start.o连接为lowlevel_fw.elf，指定链接脚本为boot.lds，并生成对应的map文件
$CROSS_PREFIX-gcc -nostartfiles -T./boot.lds -Wl,-Map=$SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.map -Wl,--gc-sections $SHELL_FOLDER/output/lowlevelboot/start.o -o $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf
# 使用gnu工具生成原始的程序bin文件
$CROSS_PREFIX-objcopy -O binary -S $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.bin
# 使用gnu工具生成反汇编文件，方便调试分析（当然我们这个代码太简单，不是很需要）
$CROSS_PREFIX-objdump --source --demangle --disassemble --reloc --wide $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.elf > $SHELL_FOLDER/output/lowlevelboot/lowlevel_fw.lst

# 切换到$SHELL_FOLDER/output/lowlevelboot
cd $SHELL_FOLDER/output/lowlevelboot
# 删除已经存在的fw.bin
rm -rf fw.bin
# 创建一个名为fw.bin，每次读写块大小为1kb，总大小为32kb的全零文件
dd of=fw.bin bs=1k count=32k if=/dev/zero
# 将lowlevel_fw.bin写入fw.bin
dd of=fw.bin bs=1k conv=notrunc seek=0 if=lowlevel_fw.bin