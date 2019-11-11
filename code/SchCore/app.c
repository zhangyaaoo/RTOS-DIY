#define __APP_C_
#include "app.h"

uint32_t FlagOne;
uint32_t FlagTwo;
uint32_t FlagThree;
uint32_t FlagFour;

Event_t EventTestNormal;

void TaskOne(void *param)
{
    EventInit(&EventTestNormal, EventTypeUnknown);

    while (1)
    {
        uint32_t count = EventWaitCount(&EventTestNormal);

        uint32_t wakeUpCount = EventRemoveAll(&EventTestNormal, (void *)0, 0);
        if (wakeUpCount > 0)
        {
            TaskSched();

            count = EventWaitCount(&EventTestNormal);
        }

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
        EventWait(&EventTestNormal, CurrentTask, (void *)0, 0, 0);
        TaskSched();

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
        EventWait(&EventTestNormal, CurrentTask, (void *)0, 0, 0);
        TaskSched();

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
        TCB_t *rdyTask = EventWakeUp(&EventTestNormal, (void *)0, 0);
        TaskSched();

        FlagFour = 1;
        TaskDelay(2);
        FlagFour = 0;
        TaskDelay(2);
    }
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}
