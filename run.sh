# 获取当前脚本文件所在的目录
SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
# 用于设置qemu的分辨率
DEFAULT_VC="1280x720"
# 运行qemu-system-riscv64，指定机器类型为my-board，分配1GB内存，模拟8个CPU核心并禁用BIOS
# 将fw.bin加载到flash中，并模拟硬件启动流程
# 将my_board的三个串口和monitor绑定到虚拟控制台
$SHELL_FOLDER/output/qemu/bin/qemu-system-riscv64 \
-M my_board \
-m 1G \
-smp 8 \
-bios none \
-drive if=pflash,bus=0,unit=0,format=raw,file=$SHELL_FOLDER/output/fw/fw.bin \
-drive file=$SHELL_FOLDER/output/disk/disk.img,if=none,format=raw,id=x0 \
-device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0 \
-d in_asm -D qemu.log \
--serial vc:$DEFAULT_VC --serial vc:$DEFAULT_VC --serial vc:$DEFAULT_VC --monitor vc:$DEFAULT_VC --parallel none \
#-s \
#-S \