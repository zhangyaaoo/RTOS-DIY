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
void TaskInit(TASK_T *ptask, void *param, TCB *ptcb, Stack_t *pstack)
{
    //在执行PendSVC异常时，PSR、返回地址、LR、R12、R3、R2、R1、R0这些寄存器是自动保存的
    //在执行PendSVC异常服务进行任务切换时，不需要手动保存这些寄存器的值
    //在进行任务栈初始化时，这些寄存器在栈中的顺序与自动保存的顺序一样，自动保存的顺序就是：PSR..>R0
    //初始化栈时，我们将返回地址设置为任务函数的入口地址，这样，在切换到该任务首次执行时，就可以正确的从头开始执行任务。
    //任务函数的形参，我们是通过将参数放在R0寄存器中实现函数形参的传递。
    *(--pstack) = (unsigned int)(1 << 24);      //PSR
    *(--pstack) = (unsigned int)ptask;          //返回地址(赋值为任务函数的入口地址)
    *(--pstack) = (unsigned int)0x14141414u;    //LR
    *(--pstack) = (unsigned int)0x12121212u;    //R12
    *(--pstack) = (unsigned int)0x03030303u;    //R3
    *(--pstack) = (unsigned int)0x02020202u;    //R2
    *(--pstack) = (unsigned int)0x01010101u;    //R1
    *(--pstack) = (unsigned int)param;          //R0(赋值为任务函数的形参，将形参传递给任务函数)

    *(--pstack) = (unsigned int)0x11111111u;    //R11
    *(--pstack) = (unsigned int)0x10101010u;    //R10
    *(--pstack) = (unsigned int)0x09090909u;    //R9
    *(--pstack) = (unsigned int)0x08080808u;    //R8
    *(--pstack) = (unsigned int)0x07070707u;    //R7
    *(--pstack) = (unsigned int)0x06060606u;    //R6
    *(--pstack) = (unsigned int)0x05050505u;    //R5
    *(--pstack) = (unsigned int)0x04040404u;    //R4

    ptcb->StackPtr = pstack;
}

void TinyOSInit(void)
{
    CurrentTCBPtr = (void *)0;
    NextTCBPtr    = (void *)0;

    TaskOneTCBPtr = &TaskOneTCB;
    TaskTwoTCBPtr = &TaskTwoTCB;
    TaskIdleTCBPtr = &TaskIdleTCB;

    TaskTable[0]  = TaskOneTCBPtr;
    TaskTable[1]  = TaskTwoTCBPtr;
    SchedLockCount = 0;
}

void TinyOSStart(void)
{
    if (CurrentTCBPtr == (void *)0)
    {
        NextTCBPtr = TaskTable[0];
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
    uint32_t status = TaskEnterCritical();

    if (SchedLockCount > 0)
    {
        TaskExitCritical(status);
        return;
    }

    if (CurrentTCBPtr == TaskIdleTCBPtr)
    {
        if (TaskTable[0]->DelayTicks == 0)
        {
            NextTCBPtr = TaskTable[0];
        }
        else if (TaskTable[1]->DelayTicks == 0)
        {
            NextTCBPtr = TaskTable[1];
        } else
        {
            TaskExitCritical(status);
            return;
        }
    }
    else
    {
        if (CurrentTCBPtr == TaskTable[0])
        {
            if (TaskTable[1]->DelayTicks == 0)
            {
                NextTCBPtr = TaskTable[1];
            }
            else if (CurrentTCBPtr->DelayTicks != 0)
            {
                NextTCBPtr = TaskIdleTCBPtr;
            }
            else
            {
                TaskExitCritical(status);
                return;
            }
        }
        else if (CurrentTCBPtr == TaskTable[1])
        {
            if (TaskTable[0]->DelayTicks == 0)
            {
                NextTCBPtr = TaskTable[0];
            }
            else if (CurrentTCBPtr->DelayTicks != 0)
            {
                NextTCBPtr = TaskIdleTCBPtr;
            }
            else
            {
                TaskExitCritical(status);
                return;
            }
        }
    }
    TaskSwitch();
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
    unsigned int i;

    for (i=0; i<2; i++)
    {
        if (TaskTable[i]->DelayTicks > 0)
        {
            TaskTable[i]->DelayTicks --;
        }
    }
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

void TaskDealy(unsigned int DelayTicks)
{
    CurrentTCBPtr->DelayTicks = DelayTicks;
    TaskSched();
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
