# 获取当前脚本文件所在的目录
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 运行qemu-system-riscv64，指定机器类型为my-board，分配1GB内存，模拟8个CPU核心并禁用BIOS
$SHELL_FOLDER/output/qemu/bin/qemu-system-riscv64 \
-M my_board \
-m 1G \
-smp 8 \
-bios none \
-monitor stdio \