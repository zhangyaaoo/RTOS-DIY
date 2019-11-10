#define __TASKSCH_C_
#include "tinyos.h"

TCB_t *TaskIdleTCBPtr;                           //空闲任务控制块指针
TCB_t TaskIdleTCB;                               //空闲任务控制块
Stack_t TaskIdleStackBuf[IDLETASK_STACK_SIZE];   //空闲任务堆栈

void TaskRunFirst(void);
void TaskSwitch(void);
void TaskIdle(void *param);

/** @funcname  TaskInit
  * @brief     任务初始化
  * @param
  *   ptask : 任务函数的指针
  *   param : 任务函数的传参
  *   ptcb  : 任务控制块的指针
  *   pstack: 任务堆栈的栈顶地址
  * @retval    None
  */
TCB_t *TaskInit(TASK_t *pTask, TaskPrio_t Prio, void *param, TCB_t *pTcb, Stack_t *pStack)
{
    //在执行PendSVC异常时，PSR、返回地址、LR、R12、R3、R2、R1、R0这些寄存器是自动保存的
    //在执行PendSVC异常服务进行任务切换时，不需要手动保存这些寄存器的值
    //在进行任务栈初始化时，这些寄存器在栈中的顺序与自动保存的顺序一样，自动保存的顺序就是：PSR..>R0
    //初始化栈时，我们将返回地址设置为任务函数的入口地址，这样，在切换到该任务首次执行时，就可以正确的从头开始执行任务。
    //任务函数的形参，我们是通过将参数放在R0寄存器中实现函数形参的传递。
    *(--pStack) = (unsigned int)(1 << 24);      //PSR
    *(--pStack) = (unsigned int)pTask;          //返回地址(赋值为任务函数的入口地址)
    *(--pStack) = (unsigned int)0x14141414u;    //LR
    *(--pStack) = (unsigned int)0x12121212u;    //R12
    *(--pStack) = (unsigned int)0x03030303u;    //R3
    *(--pStack) = (unsigned int)0x02020202u;    //R2
    *(--pStack) = (unsigned int)0x01010101u;    //R1
    *(--pStack) = (unsigned int)param;          //R0(赋值为任务函数的形参，将形参传递给任务函数)

    *(--pStack) = (unsigned int)0x11111111u;    //R11
    *(--pStack) = (unsigned int)0x10101010u;    //R10
    *(--pStack) = (unsigned int)0x09090909u;    //R9
    *(--pStack) = (unsigned int)0x08080808u;    //R8
    *(--pStack) = (unsigned int)0x07070707u;    //R7
    *(--pStack) = (unsigned int)0x06060606u;    //R6
    *(--pStack) = (unsigned int)0x05050505u;    //R5
    *(--pStack) = (unsigned int)0x04040404u;    //R4

    //初始化任务栈指针
    pTcb->StackPtr      = pStack;

    //初始化任务延时节点
    NodeInit(&pTcb->DelayNode);

    //初始化任务链表节点
    NodeInit(&pTcb->LinkNode);

    //初始化任务优先级  任务就绪位图  任务检索表
    pTcb->Prio          = Prio;
    ListInsertHead(&TaskTable[Prio], &pTcb->LinkNode);
    BitmapSet(&TaskPrioBitMap, Prio);

    //初始化任务状态
    pTcb->TaskState = TASK_STATE_READY;

    //初始化任务运行时间片的大小
    pTcb->Slice =  TASK_RUN_TIMESLICE;

    return pTcb;
}

void TinyOSInit(void)
{
    uint32_t i;

    CurrentTask = (void *)0;
    NextTask    = (void *)0;

    //初始化调度锁
    SchedLockCount = 0;

    //初始化任务优先级位图
    BitmapInit(&TaskPrioBitMap);

    //初始化任务延时列表
    ListInit(&TaskDelayList);

    //初始化任务就绪表
    for (i = 0; i < PRIO_NUM_MAX; i++)
    {
        ListInit(&TaskTable[i]);
    }

    TaskIdleTCBPtr  = TaskInit(TaskIdle, (PRIO_NUM_MAX - 1), (void *)0x0, &TaskIdleTCB, &TaskIdleStackBuf[1024]);
}

void TinyOSStart(void)
{
    if (CurrentTask == (void *)0)
    {
        NextTask = GetHighReadyTask();;
    }

    TaskRunFirst();
}


void TaskRunFirst(void)
{
    __set_PSP(0);       //Assigns the given value(0) to the Process Stack Pointer (PSP).

    MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;      //设置PendSVC异常的优先级为最低
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;      //触发PendSVC异常
}

void TaskSched(void)
{
    TCB_t *HighReadyTask;

    uint32_t status = TaskEnterCritical();

    if (SchedLockCount > 0)
    {
        TaskExitCritical(status);
        return;
    }

    HighReadyTask = GetHighReadyTask();

    if (HighReadyTask != CurrentTask)
    {
        NextTask = HighReadyTask;
        TaskSwitch();
    }

    TaskExitCritical(status);
}


void TaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;      //触发PendSVC异常
}

void TaskReliefReady(TCB_t *ptcb)
{
    ListRemoveNode(&TaskTable[ptcb->Prio], &(ptcb->LinkNode));

    //队列中可能存在多个任务，只有队列中没有任务时，才清除位图
    if (ListCount(&TaskTable[ptcb->Prio]) == 0)
    {
        BitmapClear(&TaskPrioBitMap, ptcb->Prio);
    }
}

void TaskSetReady(TCB_t *ptcb)
{
    ListInsertHead(&TaskTable[ptcb->Prio], &ptcb->LinkNode);
    BitmapSet(&TaskPrioBitMap, ptcb->Prio);
}

uint32_t TaskEnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();        // CPSID I
    return primask;
}

void TaskExitCritical(uint32_t status)
{
    __set_PRIMASK(status);
}

void TaskSchedDisable(void)
{
    uint32_t status = TaskEnterCritical();

    if (SchedLockCount < 255)
    {
        SchedLockCount++;
    }

    TaskExitCritical(status);
}

void TaskSchedEnable(void)
{
    uint32_t status = TaskEnterCritical();

    if (SchedLockCount > 0)
    {
        if (--SchedLockCount == 0)
        {
            TaskSched();
        }
    }

    TaskExitCritical(status);
}

TCB_t *GetHighReadyTask(void)
{
    uint32_t HighestPrio;
    Node_t *Node;

    HighestPrio = BitmapGetFirstSet(&TaskPrioBitMap);
    Node = ListFirst(&(TaskTable[HighestPrio]));

    return CONTAINER_OF(Node, TCB_t, LinkNode);
}

void TaskIdle(void *param)
{
    while (1)
    {
        ;
    }
}

