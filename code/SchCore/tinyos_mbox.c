#define __TINYOS_MBOX_C_

#include "tinyos.h"

/** @funcname  MboxInit
  * @brief     Initilize the mail box.
  * @param     Mbox     The mailbox to be initilized.
  *            MsgBuf   The msg buffer.
  *            MaxCount The amount of msg that we can store.
  * @retval    None
  */
void MboxInit(Mbox_t *Mbox, void **MsgBuf, uint32_t MaxCount)
{
    EventInit(&Mbox->Event, EventTypeMbox);
    Mbox->Count = 0;
    Mbox->MaxCount = MaxCount;
    Mbox->ReadIndex = 0;
    Mbox->WriteIndex = 0;
    Mbox->MsgBuf = MsgBuf;
}

/** @funcname  MboxWait
  * @brief     等待邮箱内的消息
  * @param     Mbox      等待的邮箱
  *            Msg       存储消息的地方
  *            WaitTicks 等待消息的超时时间，为 0 则无限等待
  * @retval    返回等待的结果
  *            可能的结果：ErrorCode_NoError
  *                        ErrorCode_Timeout
  *                        ErrorCode_ResourceUnavaliable
  */
uint32_t MboxWait(Mbox_t *Mbox, void **Msg, uint32_t WaitTicks)
{
    uint32_t status = TaskEnterCritical();      //进入临界区

    /* 判断邮箱内是否有消息 */
    if (Mbox->Count > 0)
    {
        //有消息，则取出消息
        --Mbox->Count;
        *Msg = Mbox->MsgBuf[Mbox->ReadIndex++];

        if (Mbox->WriteIndex >= Mbox->MaxCount)
        {
            Mbox->ReadIndex  = 0;
        }

        TaskExitCritical(status);               //退出临界区

        return ErrorCode_NoError;
    }
    else
    {
        //没有消息，则将当前任务插入到等待邮箱消息的任务队列中去，切换任务
        EventWait(&Mbox->Event, CurrentTask, (void *)0,  EventTypeMbox, WaitTicks);

        TaskExitCritical(status);                   //退出临界区

        TaskSched();    //切换出当前任务

        *Msg = CurrentTask->EventMsg;   //当任务切换回来时，从当前任务中取出消息

        return CurrentTask->WaitEventRslt;  //返回等待结果
    }
}

/** @funcname  MboxNoWaitGet
  * @brief     获取邮箱内的消息，没有消息则立即返回
  * @param     Mbox      等待的邮箱
  *            Msg       存储消息的地方
  * @retval    返回等待的结果
  *            可能的结果：ErrorCode_NoError
  *                        ErrorCode_ResourceUnavaliable
  */
uint32_t MboxNoWaitGet(Mbox_t *Mbox, void **Msg)
{
    uint32_t status = TaskEnterCritical();      //进入临界区

    /* 判断邮箱内是否有消息 */
    if (Mbox->Count > 0)
    {
        //有消息，则取出消息
        --Mbox->Count;
        *Msg = Mbox->MsgBuf[Mbox->ReadIndex++];

        if (Mbox->WriteIndex == Mbox->MaxCount)
        {
            Mbox->ReadIndex  = 0;
        }

        TaskExitCritical(status);               //退出临界区

        return ErrorCode_NoError;
    }
    else
    {
        //没有消息，则返回
        TaskExitCritical(status);                   //退出临界区

        return ErrorCode_ResourceUnavaliable;   //返回等待结果
    }


}

/** @funcname  MboxNotify
  * @brief     通知消息可用，唤醒等待任务队列中的一个任务，或者将消息插入到消息队列中
  * @param     Mbox      通知的邮箱
  *            Msg       存储消息的地方
  *            NotifyOpt 通知的选项
  * @retval    返回通知的结果
  *            可能的结果：ErrorCode_NoError
  *                        ErrorCode_ResourceFull
  */
uint32_t MboxNotify(Mbox_t *Mbox, void *Msg, uint32_t NotifyOpt)
{
    uint32_t status = TaskEnterCritical();      //进入临界区

    //判断邮箱中是否有任务等待消息
    if (EventWaitCount(&Mbox->Event) > 0)
    {
        //有任务等待消息，则唤醒位于队列头部(最先等待)的任务
        TCB_t *pTcb = EventWakeUp(&Mbox->Event, (void *)0, ErrorCode_NoError);
        //如果唤醒的任务的优先级高于当前任务的优先级，则切换到高优先级的任务
        if (pTcb->Prio > CurrentTask->Prio)
        {
            TaskSched();
        }
    }
    else
    {
        //如果没有任务等待消息，则将消息缓存到邮箱的消息缓冲区中去

        //邮箱消息缓冲区是否存满
        if (Mbox->Count >= Mbox->MaxCount)
        {
            TaskExitCritical(status);
            return ErrorCode_ResourceFull;
        }

        //可以选择将消息是否插入到消息队列的头部，以优先取出
        if (NotifyOpt & MBOXSENDFRONT)
        {
            if (Mbox->ReadIndex <= 0)
            {
                Mbox->ReadIndex = Mbox->MaxCount - 1;
            }
            else
            {
                --Mbox->ReadIndex;
            }
            Mbox->MsgBuf[Mbox->ReadIndex] = Msg;
        }
        else
        {
            Mbox->MsgBuf[Mbox->WriteIndex++] = Msg;
            if (Mbox->WriteIndex >= Mbox->MaxCount)
            {
                Mbox->WriteIndex = 0;
            }
        }

        //消息计数自增
        Mbox->Count++;
    }

    TaskExitCritical(status);                   //退出临界区
    return ErrorCode_NoError;
}

/** @funcname  MboxClear
  * @brief     清空邮箱中的所有消息
  * @param     Mbox  待清空的邮箱
  * @retval    None
  */
void MboxClear(Mbox_t *Mbox)
{
    uint32_t status = TaskEnterCritical();

    if (EventWaitCount(&Mbox->Event) == 0)
    {
        Mbox->Count = 0;
        Mbox->ReadIndex = 0;
        Mbox->WriteIndex = 0;
    }

    TaskExitCritical(status);
}

/** @funcname  MboxDestroy
  * @brief     销毁邮箱
  * @param     Mbox
  * @retval    None
  */
uint32_t MboxDestroy(Mbox_t *Mbox)
{
    uint32_t count = 0;
    uint32_t status;

    status = TaskEnterCritical();

    //清空时间控制块中的任务
    count = EventRemoveAll(&Mbox->Event, (void *)0, ErrorCode_Deleted);

    TaskExitCritical(status);

    //清空过程中，可能有任务就绪，执行任务调度
    if (count > 0)
    {
        TaskSched();
    }

    return count;
}

/** @funcname  MboxDestroy
  * @brief     销毁邮箱
  * @param     Mbox
  * @retval    None
  */
void MboxGetInfo(Mbox_t *Mbox, MboxInfo_t *Info)
{
    uint32_t status = TaskEnterCritical();      //进入临界区

    /* 拷贝需要的消息 */
    Info->Count = Mbox->Count;
    Info->MaxCount = Mbox->MaxCount;
    Info->TaskCount = EventWaitCount(&Mbox->Event);

    TaskExitCritical(status);                   //退出临界区
}

