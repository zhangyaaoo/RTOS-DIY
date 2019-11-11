#define __TINYOS_TIME_C_
#include "tinyos.h"

void TaskDelay(unsigned int DelayTicks)
{
    uint32_t status = TaskEnterCritical();

    TaskInsertToDelayList(CurrentTask, DelayTicks);

    TaskReliefReady(CurrentTask);

    TaskExitCritical(status);

    TaskSched();
}

void TaskInsertToDelayList(TCB_t *pTcb, uint32_t DelayTicks)
{
    pTcb->DelayTicks = DelayTicks;
    ListInsertHead(&TaskDelayList, &(pTcb->DelayNode));
    pTcb->TaskState |= TINYOS_TASK_STATE_DELAYED;
}

void TaskRmvFromDelayList(TCB_t *pTcb)
{
    ListRemoveNode(&TaskDelayList, &pTcb->DelayNode);
    pTcb->TaskState &= ~TINYOS_TASK_STATE_DELAYED;
}
