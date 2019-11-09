#ifndef __TASKSCH_H_
#define __TASKSCH_H_

#ifdef __TASKSCH_C_
#define TASKSCH_EXT
#else
#define TASKSCH_EXT extern
#endif

#include "ARMCM3.h"

#define NVIC_INT_CTRL       0xE000ED04      //Interrupt control state register.
#define NVIC_SYSPRI14       0xE000ED22      //System priority register (priority 14).
#define NVIC_PENDSV_PRI     0xFF            //PendSV priority value (lowest).
#define NVIC_PENDSVSET      0x10000000      //Value to trigger PendSV exception.

#define MEM32(ADDR)         *((volatile unsigned int  *)ADDR)
#define MEM8(ADDR)          *((volatile unsigned char *)ADDR)

#define TASK_NUM_MAX        5u

typedef struct TaskCtrlBlock
{
    unsigned int *StackPtr;     //Stack Pointer
    unsigned int DelayTicks;    //DelayTicks Count
}TCB;

typedef unsigned int Stack_t;

typedef void TASK_T(void *);    //定义任务函数类型


TASKSCH_EXT TCB *CurrentTCBPtr;
TASKSCH_EXT TCB *NextTCBPtr;

TASKSCH_EXT TCB *TaskTable[TASK_NUM_MAX];

extern TCB *TaskOneTCBPtr;              //任务一控制块指针
extern TCB *TaskTwoTCBPtr;              //任务二控制块指针

extern TCB TaskOneTCB;                  //任务一控制块
extern TCB TaskTwoTCB;                  //任务二控制块

extern TCB *TaskIdleTCBPtr;
extern TCB TaskIdleTCB;

TASKSCH_EXT void TinyOSInit(void);
TASKSCH_EXT void TinyOSStart(void);
TASKSCH_EXT void TaskInit(TASK_T *ptask, void *param, TCB *ptcb, Stack_t *pstack);
TASKSCH_EXT void TaskSched(void);

TASKSCH_EXT void TaskDealy(unsigned int DelayTicks);

TASKSCH_EXT void IntDisable(void);
TASKSCH_EXT void IntEnable(void);

TASKSCH_EXT uint32_t TaskEnterCritical(void);
TASKSCH_EXT void TaskExitCritical(uint32_t status);

#endif /* __TASKSCH_H_ */
