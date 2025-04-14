#ifndef HW_RISCV_MY_BOARD_H
#define HW_RISCV_MY_BOARD_H

#include "hw/boards.h"
#include "hw/riscv/riscv_hart.h"
#include "hw/sysbus.h"
#include "hw/block/flash.h"

//最大CPU数量和最大socket节点数量
#define MY_BOARD_CPUS_MAX          8
#define MY_BOARD_SOCKETS_MAX       8
//
#define MY_BOARD_FLASH_SECTOR_SIZE (256 * KiB)
//定义板子名称，最后会被拼接为my-board-machine
#define TYPE_RISCV_MY_BOARD_MACHINE MACHINE_TYPE_NAME("my-board")
typedef struct MyBoardState MyBoardState;
//生成一个RISCV_MY_BOARD_MACHINE函数，用于将TYPE_RISCV_MY_BOARD_MACHINE转化为MyBoardState指针
DECLARE_INSTANCE_CHECKER(MyBoardState, RISCV_MY_BOARD_MACHINE,
                         TYPE_RISCV_MY_BOARD_MACHINE)
//描述板子的状态
struct MyBoardState{
    //每个板子都需要具备的基本板子状态，类似于继承
    MachineState parent;
    //板子具备的硬件
    RISCVHartArrayState soc[MY_BOARD_SOCKETS_MAX];
    PFlashCFI01 *flash;
    

};
//硬件编号，为设计板子的硬件地图做准备
enum{
    MY_BOARD_MROM,
    MY_BOARD_SRAM,
    MY_BOARD_UART0,
    MY_BOARD_FLASH,
    MY_BOARD_DRAM,
};
//定义中断号
enum{
    MY_BOARD_UART0_IRQ = 10,

};

#endif