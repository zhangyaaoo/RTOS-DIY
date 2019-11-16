#define __APP_C_
#include "app.h"

uint32_t FlagOne;
uint32_t FlagTwo;
uint32_t FlagThree;
uint32_t FlagFour;

void TaskOne(void *param)
{
    while (1)
    {
        FlagOne = 0;
        TaskDelay(1);
        FlagOne = 1;
        TaskDelay(1);
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        TaskDelay(1);
        FlagTwo = 0;
        TaskDelay(1);
    }
}

void TaskThree(void *param)
{
    while (1)
    {
        FlagThree = 1;
        TaskDelay(2);
        FlagThree = 0;
        TaskDelay(2);
    }
}

void TaskFour(void *param)
{
    while (1)
    {
        printf("hello,world!\r\n");
        TaskDelay(20);
    }
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}

void AppInit(void)
{
    TaskOneTCBPtr   = TaskInit(TaskOne, 0, (void *)0x11111111, &TaskOneTCB, &TaskOneStackBuf[1024]);
    TaskTwoTCBPtr   = TaskInit(TaskTwo, 1, (void *)0x22222222, &TaskTwoTCB, &TaskTwoStackBuf[1024]);
    TaskThreeTCBPtr = TaskInit(TaskThree, 1, (void *)0x33333333, &TaskThreeTCB, &TaskThreeStackBuf[1024]);
    TaskFourTCBPtr  = TaskInit(TaskFour, 2, (void *)0x44444444, &TaskFourTCB, &TaskFourStackBuf[1024]);
}

