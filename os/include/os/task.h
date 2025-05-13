#ifndef OS_TASK_H__
#define OS_TASK_H__

#include "os.h"

typedef enum TaskState{
    UnInit,
    Ready,
    Running,
    Exited,
}TaskState;

typedef struct TaskControlBlock{
    TaskState task_state;
    TaskContext task_context;
}TaskControlBlock;

#endif