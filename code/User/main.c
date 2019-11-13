#define __MAIN_C_
#include "main.h"

int main(void)
{
    IntDisable();

    SetSysTickPeriod(10);

    TinyOSInit();

    TaskOneTCBPtr   = TaskInit(TaskOne, 0, (void *)0x11111111, &TaskOneTCB, &TaskOneStackBuf[1024]);
    TaskTwoTCBPtr   = TaskInit(TaskTwo, 1, (void *)0x22222222, &TaskTwoTCB, &TaskTwoStackBuf[1024]);
    TaskThreeTCBPtr = TaskInit(TaskThree, 1, (void *)0x33333333, &TaskThreeTCB, &TaskThreeStackBuf[1024]);
    TaskFourTCBPtr  = TaskInit(TaskFour, 2, (void *)0x44444444, &TaskFourTCB, &TaskFourStackBuf[1024]);

    TinyOSStart();

    IntEnable();

    return 0;
}

