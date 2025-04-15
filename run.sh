# 获取当前脚本文件所在的目录
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 运行qemu-system-riscv64，指定机器类型为my-board，分配1GB内存，模拟8个CPU核心并禁用BIOS
# 将fw.bin加载到flash中，并模拟硬件启动流程
# 并禁用qemu图形界面和并行端口，直接输出到控制台
$SHELL_FOLDER/output/qemu/bin/qemu-system-riscv64 \
-M my_board \
-m 1G \
-smp 8 \
-bios none \
-drive if=pflash,bus=0,unit=0,format=raw,file=$SHELL_FOLDER/output/fw/fw.bin \
-d in_asm -D qemu.log \
-nographic --parallel none \