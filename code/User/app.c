#define __APP_C_
#include "app.h"

uint32_t FlagOne;
uint32_t FlagTwo;
uint32_t FlagThree;
uint32_t FlagFour;

Sem_t sem_one;
Sem_t sem_two;
Sem_t sem_three;

uint32_t is_destroyed;

void TaskOne(void *param)
{
    uint32_t ret = 0;
    SemInit(&sem_one, 0, 10);
    SemInit(&sem_two, 0, 10);
    SemInit(&sem_three, 2, 10);
    while (1)
    {
        if (0 == FlagOne)
        {
            FlagOne = 1;
        }
        else
        {
            FlagOne = 0;
        }
        printf("task one: waiting sem_one\r\n");
        ret = SemWait(&sem_one, 0);
        if (ErrorCode_NoError == ret)
        {
            printf("task one: has got sem_one\r\n");
        }
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        TaskDelay(5);
        printf("task two: notify sem_one\r\n");
        SemNotify(&sem_one);
        FlagTwo = 0;
        TaskDelay(5);
        printf("task two: notify sem_one\r\n");
        SemNotify(&sem_one);
    }
}

void TaskThree(void *param)
{
    uint32_t ret = 0;
    while (1)
    {
        if (0 == FlagThree)
        {
            FlagThree = 1;
        }
        else
        {
            FlagThree = 0;
        }

        printf("task three: waiting sem_two, 2 ticks timeout\r\n");
        ret = SemWait(&sem_two,2);
        if (ErrorCode_Timeout == ret)
        {
            printf("task three: waiting sem_two, has timeout\r\n");
        }

        if (1 != is_destroyed)
        {
            printf("task three: waiting sem_three\r\n");
            SemWait(&sem_three, 0);
        }
        else
        {
            printf("task three: sem_three has been destoryed\r\n");
        }
    }
}

void TaskFour(void *param)
{
    SemInfo_t sem_info;

    SemGetInfo(&sem_three, &sem_info);
    printf("Count = %d\r\n", sem_info.Count);
    printf("MaxConut = %d\r\n", sem_info.MaxCount);
    printf("TaskCount = %d\r\n", sem_info.TaskCount);

    TaskDelay(50);

    SemGetInfo(&sem_three, &sem_info);
    printf("Count = %d\r\n", sem_info.Count);
    printf("MaxConut = %d\r\n", sem_info.MaxCount);
    printf("TaskCount = %d\r\n", sem_info.TaskCount);
    SemNoWaitGet(&sem_three);
    SemGetInfo(&sem_three, &sem_info);
    printf("Count = %d\r\n", sem_info.Count);
    printf("MaxConut = %d\r\n", sem_info.MaxCount);
    printf("TaskCount = %d\r\n", sem_info.TaskCount);
    SemDestroy(&sem_three);
    is_destroyed = 1;
    while (1)
    {
        printf("hello,world!\r\n");
        TaskDelay(50);
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

