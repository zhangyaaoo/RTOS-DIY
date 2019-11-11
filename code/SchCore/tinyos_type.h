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
typedef void CLEAN_FUNC_t(void *);      //定义清理函数类型

/* task control block */
typedef struct TaskCtrlBlock
{
    uint32_t    *StackPtr;      //Stack Pointer
    Node_t       LinkNode;      //TaskLinkNode
    uint32_t     DelayTicks;    //DelayTicks Count
    TaskPrio_t   Prio;          //TaskPrio
    Node_t       DelayNode;     //Task delay list's node
    uint32_t     TaskState;     //TaskState indicate
    uint32_t     Slice;         //Task run time slice
    uint32_t     SuspendCount;  //Task suspend count
    CLEAN_FUNC_t    *CleanFunc;     //when task is been deleted, this is the callback function.
    void            *CleanParam;    //CleanFunc's param.
    uint8_t          ReqDelFlag;
}TCB_t;

/* Task Information */
typedef struct _TaskInfo_t
{
    TaskPrio_t       Prio;          //TaskPrio
    uint32_t         TaskState;     //TaskState indicate
    uint32_t         DelayTicks;    //DelayTicks Count
    uint32_t         Slice;         //Task run time slice
    uint32_t         SuspendCount;  //Task suspend count
}TaskInfo_t;

#define TINYOS_TASK_STATE_RDY                   (0)
#define TINYOS_TASK_STATE_DELAYED               (1 << 1)
#define TINYOS_TASK_STATE_SUSPEND               (1 << 2)

#endif /* __OS_TYPE_H_ */
