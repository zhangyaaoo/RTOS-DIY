#define __TINYOS_EVENT_C_
#include "tinyos.h"

void EventInit(Event_t *pEvent, EventType_t EventType)
{
    pEvent->EventType = EventType;
    ListInit(&pEvent->WaitedTaskList);
}

void EventWait(Event_t *Event, TCB_t *pTcb, void *Msg, uint32_t State, uint32_t TimeOut)
{
    uint32_t status = TaskEnterCritical();

    pTcb->TaskState |= State;
    pTcb->WaitEvent = Event;
    pTcb->EventMsg = Msg;
    pTcb->WaitEventRslt = ErrorCode_NoError;

    TaskRmFromRdyTable(pTcb);

    ListInsertTail(&Event->WaitedTaskList, &pTcb->LinkNode);

    if (TimeOut)
    {
        TaskInsertToDelayList(pTcb, TimeOut);
    }

    TaskExitCritical(status);
}


TCB_t *EventWakeUp(Event_t *Event, void *Msg, uint32_t Result)
{
    Node_t  *Node;
    TCB_t   *pTcb = (TCB_t *)0;

    uint32_t status = TaskEnterCritical();

    if((Node = ListRemoveHead(&Event->WaitedTaskList)) != (Node_t *)0)
    {
        pTcb = CONTAINER_OF(Node, TCB_t, LinkNode);

        pTcb->WaitEvent = (Event_t *)0;
        pTcb->EventMsg = Msg;
        pTcb->WaitEventRslt = Result;
        pTcb->TaskState &= ~TINYOS_TASK_WAIT_EVENT_MASK;

        if (pTcb->DelayTicks != 0)
        {
            TaskRmvFromDelayList(pTcb);
        }

        TaskSetReady(pTcb);
    }

    TaskExitCritical(status);

    return pTcb;
}


void EventRemoveTask (TCB_t *pTcb, void *Msg, uint32_t Result)
{
    uint32_t status = TaskEnterCritical();

    ListRemoveNode(&pTcb->WaitEvent->WaitedTaskList, &pTcb->LinkNode);

    pTcb->WaitEvent = (Event_t *)0;
    pTcb->EventMsg = Msg;
    pTcb->WaitEventRslt = Result;
    pTcb->TaskState &= ~TINYOS_TASK_WAIT_EVENT_MASK;

    TaskExitCritical(status);
}

uint32_t EventRemoveAll(Event_t *Event, void *Msg, uint32_t Result)
{
    Node_t  *Node;
    TCB_t *pTcb;
    uint32_t Count;

    uint32_t status = TaskEnterCritical();

    Count = ListCount(&Event->WaitedTaskList);

    while ((Node = ListRemoveHead(&Event->WaitedTaskList)) != (Node_t *)0)
    {
        pTcb = CONTAINER_OF(Node, TCB_t, LinkNode);

        pTcb->WaitEvent = (Event_t *)0;
        pTcb->EventMsg = Msg;
        pTcb->WaitEventRslt = Result;
        pTcb->TaskState &= ~TINYOS_TASK_WAIT_EVENT_MASK;

        if (pTcb->DelayTicks != 0)
        {
            TaskRmvFromDelayList(pTcb);
        }

        TaskSetReady(pTcb);
    }

    TaskExitCritical(status);

    return Count;
}

uint32_t EventWaitCount(Event_t *Event)
{
    uint32_t Count = 0;

    uint32_t status = TaskEnterCritical();

    Count = ListCount(&Event->WaitedTaskList);

    TaskExitCritical(status);

    return Count;
}

