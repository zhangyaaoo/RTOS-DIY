#define __TINYOS_SEM_C_
#include "tinyos.h"

void SemInit(Sem_t *Sem, uint32_t StartCount, uint32_t MaxCount)
{
    EventInit(&Sem->Event, EventTypeSem);

    Sem->MaxCount = MaxCount;
    if (MaxCount == 0)
    {
        Sem->Count = StartCount;
    }
    else
    {
        Sem->Count = (StartCount > MaxCount) ? MaxCount : StartCount;
    }
}

uint32_t SemWait(Sem_t *Sem, uint32_t WaitTicks)
{
    uint32_t status = TaskEnterCritical();

    if (Sem->Count > 0)
    {
        --Sem->Count;
        TaskExitCritical(status);
        return ErrorCode_NoError;
    }
    else
    {
        EventWait(&Sem->Event, CurrentTask, (void *)0,  EventTypeSem, WaitTicks);
        TaskExitCritical(status);

        TaskSched();

        return CurrentTask->WaitEventRslt;
    }
}

uint32_t SemNoWaitGet(Sem_t *Sem)
{
    uint32_t status = TaskEnterCritical();

    if (Sem->Count > 0)
    {
        --Sem->Count;
        TaskExitCritical(status);

        return ErrorCode_NoError;
    }
    else
    {
        TaskExitCritical(status);

        return ErrorCode_ResourceUnavaliable;
    }
}

void SemNotify (Sem_t *Sem)
{
    TCB_t *pTcb;
    uint32_t status = TaskEnterCritical();

    if (EventWaitCount(&Sem->Event) > 0)
    {
        pTcb = EventWakeUp(&Sem->Event, (void *)0, ErrorCode_NoError);

        if (pTcb->Prio < CurrentTask->Prio)
        {
            TaskSched();
        }
    }
    else
    {
        ++Sem->Count;

        if ((Sem->MaxCount != 0) && (Sem->Count > Sem->MaxCount))
        {
            Sem->Count = Sem->MaxCount;
        }
    }

    TaskExitCritical(status);
}


void SemGetInfo(Sem_t *Sem, SemInfo_t *Info)
{
    uint32_t status = TaskEnterCritical();

    Info->Count = Sem->Count;
    Info->MaxCount = Sem->MaxCount;
    Info->TaskCount = EventWaitCount(&Sem->Event);

    TaskExitCritical(status);
}


uint32_t SemDestroy(Sem_t *Sem)
{
    uint32_t status = TaskEnterCritical();

    uint32_t Count = EventRemoveAll(&Sem->Event, (void *)0, ErrorCode_Deleted);
    Sem->Count = 0;
    TaskExitCritical(status);

    if (Count > 0)
    {
        TaskSched();
    }
    return Count;
}

