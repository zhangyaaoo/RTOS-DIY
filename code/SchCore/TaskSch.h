#ifndef __TASKSCH_H_
#define __TASKSCH_H_

#ifdef __TASKSCH_C_
#define TASKSCH_EXT
#else
#define TASKSCH_EXT extern
#endif

#include "ARMCM3.h"
#include "os_type.h"
#include "os_cfg.h"
#include "bitmap.h"
#include "dll.h"

#define NVIC_INT_CTRL       0xE000ED04      //Interrupt control state register.
#define NVIC_SYSPRI14       0xE000ED22      //System priority register (priority 14).
#define NVIC_PENDSV_PRI     0xFF            //PendSV priority value (lowest).
#define NVIC_PENDSVSET      0x10000000      //Value to trigger PendSV exception.

#define MEM32(ADDR)         *((volatile unsigned int  *)ADDR)
#define MEM8(ADDR)          *((volatile unsigned char *)ADDR)



typedef struct TaskCtrlBlock
{
    unsigned int *StackPtr;     //Stack Pointer
    unsigned int DelayTicks;    //DelayTicks Count
    TaskPrio_t   Prio;          //TaskPrio
    Node_t       DelayNode;
    TaskState_t  TaskState;
}TCB_t;

typedef unsigned int Stack_t;

typedef void TASK_t(void *);    //定义任务函数类型


TASKSCH_EXT TCB_t *CurrentTCBPtr;
TASKSCH_EXT TCB_t *NextTCBPtr;

TASKSCH_EXT TCB_t *TaskTable[PRIO_NUM_MAX];

//调度锁计数器
TASKSCH_EXT uint32_t SchedLockCount;

TASKSCH_EXT BitMap_t TaskPrioBitMap;

TASKSCH_EXT List_t   TaskDelayList;
extern TCB_t *TaskOneTCBPtr;              //任务一控制块指针
extern TCB_t *TaskTwoTCBPtr;              //任务二控制块指针

extern TCB_t TaskOneTCB;                  //任务一控制块
extern TCB_t TaskTwoTCB;                  //任务二控制块

extern TCB_t *TaskIdleTCBPtr;
extern TCB_t TaskIdleTCB;

TASKSCH_EXT void TinyOSInit(void);
TASKSCH_EXT void TinyOSStart(void);

TASKSCH_EXT TCB_t *TaskInit(TASK_t *ptask, TaskPrio_t prio, void *param, TCB_t *ptcb, Stack_t *pstack);
TASKSCH_EXT void TaskSched(void);

TASKSCH_EXT void TaskDealy(unsigned int DelayTicks);

TASKSCH_EXT void IntDisable(void);
TASKSCH_EXT void IntEnable(void);

TASKSCH_EXT uint32_t TaskEnterCritical(void);
TASKSCH_EXT void TaskExitCritical(uint32_t status);

TASKSCH_EXT void TaskSchedDisable(void);
TASKSCH_EXT void TaskSchedEnable(void);
TASKSCH_EXT TCB_t *GetHighReadyTask(void);

TASKSCH_EXT void TaskInsertToDelayList(TCB_t *ptcb, uint32_t DelayTicks);
TASKSCH_EXT void TaskRmvFromDelayList(TCB_t *ptcb);
TASKSCH_EXT void TaskGetReady(TCB_t *ptcb);
TASKSCH_EXT void TaskReliefReady(TCB_t *ptcb);

#endif /* __TASKSCH_H_ */
