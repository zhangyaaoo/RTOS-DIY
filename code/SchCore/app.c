#define __APP_C_
#include "app.h"

uint32_t FlagOne;
uint32_t FlagTwo;
uint32_t FlagThree;
uint32_t FlagFour;

void TaskOneDelCallBackFunc(void *param)
{
    FlagOne = 0;
}

void TaskOne(void *param)
{
    TaskSetCleanCallFunc(TaskOneTCBPtr, (CLEAN_FUNC_t *)TaskOneDelCallBackFunc, (void *)0);
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
    uint32_t TaskOneDelFlag = 0;
    while (1)
    {
        FlagTwo = 1;
        TaskDelay(1);
        FlagTwo = 0;
        TaskDelay(1);
        if (!TaskOneDelFlag)
        {
            TaskDelForce(TaskOneTCBPtr);
            TaskOneDelFlag = 1;
        }
    }
}

void TaskThree(void *param)
{
    while (1)
    {
        if (TaskIsReqedDel())
        {
            FlagThree = 0;
            TaskDelSelf();
        }
        FlagThree = 1;
        TaskDelay(2);
        FlagThree = 0;
        TaskDelay(2);
    }
}
void TaskFour(void *param)
{
    uint32_t TaskThreeDelFlag = 0;
    while (1)
    {
        FlagFour = 1;
        TaskDelay(2);
        FlagFour = 0;
        TaskDelay(2);
        if (!TaskThreeDelFlag)
        {
            TaskReqDel(TaskThreeTCBPtr);
            TaskThreeDelFlag = 1;
        }
    }
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}
