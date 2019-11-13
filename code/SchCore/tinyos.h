#ifndef __TINYOS_H_
#define __TINYOS_H_

#ifdef   TINYOS_GLOBALS
#define  TINYOS_EXT
#else
#define  TINYOS_EXT  extern
#endif

#include "stm32f10x.h"
#include "tinyos_type.h"
#include "tinyos_cfg.h"

#include "dll.h"
#include "bitmap.h"

#define NVIC_INT_CTRL       0xE000ED04      //Interrupt control state register.
#define NVIC_SYSPRI14       0xE000ED22      //System priority register (priority 14).
#define NVIC_PENDSV_PRI     0xFF            //PendSV priority value (lowest).
#define NVIC_PENDSVSET      0x10000000      //Value to trigger PendSV exception.

#define MEM32(ADDR)         *((volatile unsigned int  *)ADDR)
#define MEM8(ADDR)          *((volatile unsigned char *)ADDR)

/*
===============================================================================
*                       TINYOS GLOBAL VARIABLES
===============================================================================
*/
//当前任务指针和下一个任务指针
TINYOS_EXT TCB_t *CurrentTask;
TINYOS_EXT TCB_t *NextTask;

//任务就绪表
TINYOS_EXT List_t TaskTable[PRIO_NUM_MAX];
//调度锁计数器
TINYOS_EXT uint32_t SchedLockCount;
//任务优先级位图
TINYOS_EXT BitMap_t TaskPrioBitMap;
//任务延时队列
TINYOS_EXT List_t TaskDelayList;


void TinyOSInit(void);
void TinyOSStart(void);

void TaskSuspend(TCB_t *pTcb);
void TaskunSuspend(TCB_t *pTcb);

void TaskRmFromDelayList(TCB_t *pTcb);
void TaskRmFromRdyTable(TCB_t *pTcb);

void TaskSetCleanCallFunc(TCB_t *pTcb, CLEAN_FUNC_t *CleanFunc, void *param);
void TaskDelForce(TCB_t *pTcb);

void TaskRmFromDelayList(TCB_t *pTcb);
void TaskRmFromRdyTable(TCB_t *pTcb);
void TaskReqDel(TCB_t *pTcb);
uint8_t TaskIsReqedDel(void);
void TaskDelSelf (void);

void TaskGetInfo(TCB_t *pTcb, TaskInfo_t *Info);


/*
===============================================================================
*                           TINYOS TASK SECTION
===============================================================================
*/
TCB_t *TaskInit(TASK_t *ptask, TaskPrio_t prio, void *param, TCB_t *ptcb, Stack_t *pstack);

void TaskSched(void);

void TaskSchedDisable(void);
void TaskSchedEnable(void);
TCB_t *GetHighReadyTask(void);

void TaskSetReady(TCB_t *ptcb);
void TaskReliefReady(TCB_t *ptcb);



/*
===============================================================================
*                           TINYOS TIME SECTION
===============================================================================
*/
void TaskDelay(unsigned int DelayTicks);
void TaskInsertToDelayList(TCB_t *ptcb, uint32_t DelayTicks);
void TaskRmvFromDelayList(TCB_t *ptcb);

/*
===============================================================================
*                           TINYOS EVENT SECTION
===============================================================================
*/
void        EventInit       (Event_t *pEvent, EventType_t EventType);
void        EventWait       (Event_t *Event, TCB_t *pTcb, void *Msg, uint32_t State, uint32_t TimeOut);
TCB_t      *EventWakeUp     (Event_t *Event, void *Msg, uint32_t Result);
void        EventRemoveTask (TCB_t *pTcb, void *Msg, uint32_t Result);
uint32_t    EventRemoveAll  (Event_t *Event, void *Msg, uint32_t Result);
uint32_t    EventWaitCount  (Event_t *Event);

/*
===============================================================================
*                            TINYOS SEM SECTION
===============================================================================
*/
void        SemInit         (Sem_t *Sem, uint32_t StartCount, uint32_t MaxCount);
uint32_t    SemWait         (Sem_t *Sem, uint32_t WaitTicks);
uint32_t    SemNoWaitGet    (Sem_t *Sem);
void        SemNotify       (Sem_t *Sem);
void        SemGetInfo      (Sem_t *Sem, SemInfo_t *Info);
uint32_t    SemDestroy      (Sem_t *Sem);

/*
===============================================================================
*                            TINYOS CPU SECTION
===============================================================================
*/
void IntDisable(void);
void IntEnable(void);
void SetSysTickPeriod(uint32_t ms);

uint32_t TaskEnterCritical(void);
void TaskExitCritical(uint32_t status);


#endif /* __TINYOS_H_ */
