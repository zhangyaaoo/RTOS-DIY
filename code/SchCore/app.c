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
        TaskDelay(1);
        FlagOne = 0;
        TaskDelay(1);
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        Delay(10);
        FlagTwo = 0;
        Delay(10);
    }
}

void TaskThree(void *param)
{
    while (1)
    {
        FlagThree = 1;
        Delay(10);
        FlagThree = 0;
        Delay(10);
    }
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}
