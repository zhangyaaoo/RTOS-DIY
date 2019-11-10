#define __TINYOS_CPU_C_
#include "ARMCM3.h"
#include "tinyos.h"

/** @funcname  example1
  * @brief     None
  * @param     None
  * @retval    None
  */
__asm void PendSV_Handler(void)
{
    IMPORT  CurrentTask
    IMPORT  NextTask

    MRS     R0, PSP         //在TaskRunFirst函数中，设置了PSP为0，此处将PSP中的值读入寄存器R0中
    CBZ     R0, PendSVHandler_nosave    //如果R0中的值为0，则说明接下来运行的是第一个任务，所以不需要保存寄存器R4-R11中的内容
                                        //直接跳转，恢复第一个要运行的任务。
    STMDB   R0!, {R4-R11}   //将R4-R11压入任务堆栈
    LDR     R1, =CurrentTask
    LDR     R1, [R1]
    STR     R0, [R1]        //TCBPtr->StackPtr = R0

PendSVHandler_nosave
    LDR     R0, =CurrentTask
    LDR     R1, =NextTask
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

            TaskSetReady(ptcb);
        }
    }

    if (ListCount(&TaskTable[CurrentTask->Prio]) > 1)
    {
        if (--CurrentTask->Slice == 0)
        {
            ListRemoveHead(&TaskTable[CurrentTask->Prio]);
            ListInsertTail(&TaskTable[CurrentTask->Prio], &(CurrentTask->LinkNode));

            CurrentTask->Slice = TASK_RUN_TIMESLICE;
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

void SetSysTickPeriod(uint32_t ms)
{
    SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1;
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    SysTick->VAL   = 0;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
}
