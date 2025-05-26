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
    DeviceState *plic[MY_BOARD_SOCKETS_MAX];
};
//硬件编号，为设计板子的硬件地图做准备
enum{
    MY_BOARD_MROM,
    MY_BOARD_SRAM,
    MY_BOARD_CLINT,
    MY_BOARD_PLIC,
    MY_BOARD_UART0,
    MY_BOARD_UART1,
    MY_BOARD_UART2,
    MY_BOARD_RTC,
    MY_BOARD_VIRTIO0,
    MY_BOARD_FLASH,
    MY_BOARD_DRAM,
};
//定义中断号
enum{
    MY_BOARD_VIRTIO0_IRQ = 1,
    MY_BOARD_UART0_IRQ = 10,
    MY_BOARD_UART1_IRQ = 11,
    MY_BOARD_UART2_IRQ = 12,
    MY_BOARD_RTC_IRQ   = 13,

};
//PLIC相关定义
#define MY_BOARD_PLIC_NUM_SOURCES    127          //支持的中断源的最大数量
#define MY_BOARD_PLIC_NUM_PRIORITIES 7            //支持的中断优先级的数量
#define MY_BOARD_PLIC_PRIORITY_BASE  0x04         //中断优先级寄存器的基址
#define MY_BOARD_PLIC_PENDING_BASE   0x1000       //中断挂起寄存器的基址
#define MY_BOARD_PLIC_ENABLE_BASE    0x2000       //中断使能寄存器的基址
#define MY_BOARD_PLIC_ENABLE_STRIDE  0x80         //中断使能寄存器之间的地址间隔
#define MY_BOARD_PLIC_CONTEXT_BASE   0x200000     //上下文保存寄存器的基址
#define MY_BOARD_PLIC_CONTEXT_STRIDE 0x1000       //上下文保存寄存器之间的地址间隔
#define MY_BOARD_PLIC_SIZE(__num_context) \
    (MY_BOARD_PLIC_CONTEXT_BASE + (__num_context) * MY_BOARD_PLIC_CONTEXT_STRIDE)

#endif