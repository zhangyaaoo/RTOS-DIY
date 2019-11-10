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

void TaskInsertToDelayList(TCB_t *ptcb, uint32_t DelayTicks)
{
    ptcb->DelayTicks = DelayTicks;
    ListInsertHead(&TaskDelayList, &(ptcb->DelayNode));
    ptcb->TaskState |= TINYOS_TASK_STATE_DELAYED;
}

void TaskRmvFromDelayList(TCB_t *ptcb)
{
    ListRemoveNode(&TaskDelayList, &ptcb->DelayNode);
	ptcb->TaskState |= ~TINYOS_TASK_STATE_DELAYED;
}
