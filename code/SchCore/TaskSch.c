#define __TASKSCH_C_
#include "TaskSch.h"

void TaskRunFirst(void);
void TaskSwitch(void);

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

    CurrentTCBPtr = (void *)0;
    NextTCBPtr    = (void *)0;

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
}

void TinyOSStart(void)
{
    if (CurrentTCBPtr == (void *)0)
    {
        NextTCBPtr = GetHighReadyTask();;
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

    if (HighReadyTask != CurrentTCBPtr)
    {
        NextTCBPtr = HighReadyTask;
        TaskSwitch();
    }

    TaskExitCritical(status);
}


void TaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;      //触发PendSVC异常
}


/** @funcname  example1
  * @brief     None
  * @param     None
  * @retval    None
  */
__asm void PendSV_Handler(void)
{
    IMPORT  CurrentTCBPtr
    IMPORT  NextTCBPtr

    MRS     R0, PSP         //在TaskRunFirst函数中，设置了PSP为0，此处将PSP中的值读入寄存器R0中
    CBZ     R0, PendSVHandler_nosave    //如果R0中的值为0，则说明接下来运行的是第一个任务，所以不需要保存寄存器R4-R11中的内容
                                        //直接跳转，恢复第一个要运行的任务。
    STMDB   R0!, {R4-R11}   //将R4-R11压入任务堆栈
    LDR     R1, =CurrentTCBPtr
    LDR     R1, [R1]
    STR     R0, [R1]        //TCBPtr->StackPtr = R0

PendSVHandler_nosave
    LDR     R0, =CurrentTCBPtr
    LDR     R1, =NextTCBPtr
    LDR     R2, [R1]
    STR     R2, [R0]        // CurrentTCBPtr = NextTCBPtr

    LDR     R0, [R2]
    LDMIA   R0!, {R4-R11}
    MSR     PSP, R0         //将R4-R11弹出任务堆栈

    ORR     LR, LR, #0x04   //确保异常返回后使用的是进程栈

    BX      LR              //异常返回，并恢复自动保存的寄存器(PSR 返回地址 LR R12 R3 R2 R1 R0)中的内容
}


void SysTick_Handler(void)
{
    Node_t *Node;
    TCB_t  *ptcb;

    uint32_t status = TaskEnterCritical();

    for (Node = TaskDelayList.HeadNode.NextNode; Node != &(TaskDelayList.HeadNode); Node = Node->NextNode)
    {
        ptcb = CONTAINER_OF(Node, TCB_t, DelayNode);

        if (--(ptcb->DelayTicks) == 0)
        {
            TaskRmvFromDelayList(ptcb);

            TaskGetReady(ptcb);
        }
    }

    if (ListCount(&TaskTable[CurrentTCBPtr->Prio]) > 1)
    {
        if (--CurrentTCBPtr->Slice == 0)
        {
            ListRemoveHead(&TaskTable[CurrentTCBPtr->Prio]);
            ListInsertTail(&TaskTable[CurrentTCBPtr->Prio], &(CurrentTCBPtr->LinkNode));

            CurrentTCBPtr->Slice = TASK_RUN_TIMESLICE;
        }
    }

    TaskExitCritical(status);

    TaskSched();
}

__asm void IntDisable(void)
{
    CPSID   I
    BX      LR
}

__asm void IntEnable(void)
{
    CPSIE   I
    BX      LR
}

void TaskDelay(unsigned int DelayTicks)
{
    uint32_t status = TaskEnterCritical();

    TaskInsertToDelayList(CurrentTCBPtr, DelayTicks);

    TaskReliefReady(CurrentTCBPtr);

    TaskExitCritical(status);

    TaskSched();
}

void TaskInsertToDelayList(TCB_t *ptcb, uint32_t DelayTicks)
{
    ptcb->DelayTicks = DelayTicks;
    ListInsertHead(&TaskDelayList, &(ptcb->DelayNode));
    //ptcb->TaskState = TASK_STATE_DELAYED;
}

void TaskRmvFromDelayList(TCB_t *ptcb)
{
    ListRemoveNode(&TaskDelayList, &ptcb->DelayNode);
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

void TaskGetReady(TCB_t *ptcb)
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
