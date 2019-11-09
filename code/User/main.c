#define __MAIN_C_
#include "main.h"

uint32_t FlagOne;
uint32_t FlagTwo;

TCB_t *TaskOneTCBPtr;             //任务一控制块指针
TCB_t *TaskTwoTCBPtr;             //任务二控制块指针
TCB_t *TaskIdleTCBPtr;            //空闲任务控制块指针

TCB_t TaskOneTCB;                 //任务一控制块
TCB_t TaskTwoTCB;                 //任务二控制块
TCB_t TaskIdleTCB;                //空闲任务控制块

Stack_t TaskOneStackBuf[1024];  //任务一堆栈
Stack_t TaskTwoStackBuf[1024];  //任务二堆栈
Stack_t TaskIdleStackBuf[1024]; //空闲任务堆栈

void Delay(int n);
void SetSysTickPeriod(uint32_t ms);

void TaskOne(void *param);
void TaskTwo(void *param);
void TaskIdle(void *param);

int main(void)
{
    IntDisable();

    SetSysTickPeriod(10);

    TinyOSInit();

    TaskOneTCBPtr = TaskInit(TaskOne, 1, (void *)0x11111111, &TaskOneTCB, &TaskOneStackBuf[1024]);
    TaskTwoTCBPtr = TaskInit(TaskTwo, 2, (void *)0x22222222, &TaskTwoTCB, &TaskTwoStackBuf[1024]);
    TaskIdleTCBPtr = TaskInit(TaskIdle, (PRIO_NUM_MAX - 1), (void *)0x0, &TaskIdleTCB, &TaskIdleStackBuf[1024]);

    TinyOSStart();

    IntEnable();

    return 0;
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}

void TaskOne(void *param)
{
    while (1)
    {
        FlagOne = 1;
        TaskDealy(1);
        FlagOne = 0;
        TaskDealy(1);
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        TaskDealy(1);
        FlagTwo = 0;
        TaskDealy(1);
    }
}

void TaskIdle(void *param)
{
    while (1)
    {
        ;
    }
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

