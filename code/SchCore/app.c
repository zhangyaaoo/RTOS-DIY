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
    TaskInfo_t TaskInfo;

    while (1)
    {
        FlagFour = 1;
        TaskDelay(2);
        FlagFour = 0;
        TaskDelay(2);
        TaskGetInfo(CurrentTask, &TaskInfo);
        TaskGetInfo(TaskOneTCBPtr, &TaskInfo);
    }
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}
