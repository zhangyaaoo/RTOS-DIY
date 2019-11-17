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

typedef enum
{
    ErrorCode_NoError = 0,
    ErrorCode_Timeout,
    ErrorCode_ResourceUnavaliable,
    ErrorCode_Deleted,
    ErrorCode_ResourceFull,
}ErrorCode_t;


/* Event type */
typedef enum
{
    EventTypeUnknown,
    EventTypeSem,
    EventTypeMbox,
}EventType_t;

typedef struct
{
    EventType_t      EventType;
    List_t           WaitedTaskList;
}Event_t;

/* task control block */
typedef struct TaskCtrlBlock
{
    uint32_t        *StackPtr;      //Stack Pointer
    Node_t           LinkNode;      //TaskLinkNode
    uint32_t         DelayTicks;    //DelayTicks Count
    TaskPrio_t       Prio;          //TaskPrio
    Node_t           DelayNode;     //Task delay list's node
    uint32_t         TaskState;     //TaskState indicate
    uint32_t         Slice;         //Task run time slice
    uint32_t         SuspendCount;  //Task suspend count
    CLEAN_FUNC_t    *CleanFunc;     //when task is been deleted, this is the callback function.
    void            *CleanParam;    //CleanFunc's param.
    uint8_t          ReqDelFlag;
    Event_t         *WaitEvent;
    void            *EventMsg;
    uint32_t         WaitEventRslt;
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

typedef struct _Sem_t
{
    Event_t             Event;
    uint32_t            Count;
    uint32_t            MaxCount;
}Sem_t;

typedef struct _SemInfo_t
{
    uint32_t Count;
    uint32_t MaxCount;
    uint32_t TaskCount;
}SemInfo_t;


/** 邮箱类型定义
  */
typedef struct _Mbox_t
{
    Event_t         Event;      //事件控制块，特意放在结构体中的第一个元素，以实现首地址相同
    uint32_t        Count;      //邮箱内当前的消息数量
    uint32_t        ReadIndex;  //邮箱消息缓冲区读索引
    uint32_t        WriteIndex; //邮箱消息缓冲区写索引
    uint32_t        MaxCount;   //邮箱内能够缓冲的最大消息数量
    void          **MsgBuf;     //消息缓冲区
}Mbox_t;

/** 邮箱信息类型定义
  */
typedef struct _MboxInfo_t
{
    uint32_t        Count;      //当前缓存的消息数量
    uint32_t        MaxCount;   //最大能缓存的消息数量
    uint32_t        TaskCount;  //当前等待的任务数量
}MboxInfo_t;

#define TINYOS_TASK_STATE_RDY                   (0u)
#define TINYOS_TASK_STATE_DELAYED               (1u << 1)
#define TINYOS_TASK_STATE_SUSPEND               (1u << 2)
#define TINYOS_TASK_WAIT_EVENT_MASK             (0xFFFFu << 16)

#endif /* __OS_TYPE_H_ */
