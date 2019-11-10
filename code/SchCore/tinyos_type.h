#ifndef __OS_TYPE_H_
#define __OS_TYPE_H_

#ifdef __OS_TYPE_C_
#define OS_TYPE_EXT
#else
#define OS_TYPE_EXT extern
#endif

#include <stdint.h>
#include "dll.h"

typedef unsigned int TaskPrio_t;
typedef unsigned int Stack_t;
typedef void TASK_t(void *);    //定义任务函数类型


/* task state */
typedef enum
{
    TASK_STATE_RUN,
    TASK_STATE_READY,
    TASK_STATE_DELAYED,
}TaskState_t;


/* task control block */
typedef struct TaskCtrlBlock
{
    uint32_t    *StackPtr;      //Stack Pointer
    Node_t       LinkNode;      //TaskLinkNode
    uint32_t     DelayTicks;    //DelayTicks Count
    TaskPrio_t   Prio;          //TaskPrio
    Node_t       DelayNode;     //Task delay list's node
    TaskState_t  TaskState;     //TaskState indicate
    uint32_t     Slice;         //Task run time slice
}TCB_t;

#endif /* __OS_TYPE_H_ */
