#define __MAIN_C_
#include "main.h"

uint32_t FlagOne; 
uint32_t FlagTwo; 

TCB *TaskOneTCBPtr;             //任务一控制块指针
TCB *TaskTwoTCBPtr;             //任务二控制块指针

TCB TaskOneTCB;                 //任务一控制块
TCB TaskTwoTCB;                 //任务二控制块

Stack_t TaskOneStackBuf[1024];  //任务一堆栈
Stack_t TaskTwoStackBuf[1024];  //任务二堆栈

void Delay(int n);

void TaskOne(void *param);
void TaskTwo(void *param);

int main(void)
{
    TinyOSInit();
    
    TaskInit(TaskOne, (void *)0x11111111, TaskOneTCBPtr, &TaskOneStackBuf[1024]);
    TaskInit(TaskTwo, (void *)0x22222222, TaskTwoTCBPtr, &TaskTwoStackBuf[1024]);
 
    TinyOSStart();
    
    return 0;
}

void Delay(int n)
{
    volatile int i = n;
    while (--i);
}

void TaskOne(void *param)
{
    while (1)
    {
        FlagOne = 1;
        Delay(1000);
        FlagOne = 0;
        Delay(1000);     
        TaskSched();
    }
}

void TaskTwo(void *param)
{
    while (1)
    {
        FlagTwo = 1;
        Delay(1000);
        FlagTwo = 0;
        Delay(1000); 
        TaskSched();
    }
}

