#ifndef OS_TASK_H__
#define OS_TASK_H__

#include "os.h"
//任务状态
typedef enum TaskState{
    UnInit,
    Ready,
    Running,
    Exited,
}TaskState;
//任务控制块
typedef struct TaskControlBlock{
    TaskState task_state;
    TaskContext task_context;
}TaskControlBlock;

#endif