#define __APP_C_
#include "app.h"

uint32_t FlagOne;
uint32_t FlagTwo;
uint32_t FlagThree;

void TaskOne(void *param)
{
    while (1)
    {
        FlagOne = 1;
        TaskSuspend(TaskOneTCBPtr);
        FlagOne = 0;
        TaskSuspend(TaskOneTCBPtr);
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        TaskDelay(1);
        TaskunSuspend(TaskOneTCBPtr);
        FlagTwo = 0;
        TaskDelay(1);
        TaskunSuspend(TaskOneTCBPtr);
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

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}
