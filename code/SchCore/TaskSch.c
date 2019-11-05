/**
  ******************************************************************************
  * @file    TaskSch.c 
  * @author  ZS
  * @version 
  * @date    28-Aug-1993
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */

#define __TASKSCH_C_

/******************************** Includes ************************************/
#include "TaskSch.h"


/***************************** Private typedef ********************************/

/***************************** Private define *********************************/

/****************************** Private macro *********************************/

/**************************** Private variables *******************************/

/*********************** Private function prototypes **************************/
void TaskRunFirst(void);
void TaskSwitch(void);



/**************************** Private functions *******************************/

/** @funcname  TaskInit
  * @brief     任务初始化  
  * @param     
  *   ptask : 任务函数的指针
  *   param : 任务函数的传参
  *   ptcb  : 任务控制块的指针
  *   pstack: 任务堆栈的栈顶地址
  * @retval    None
  */
void TaskInit(TASK_T *ptask, void *param, TCB *ptcb, Stack_t *pstack)
{
    *(--pstack) = (unsigned int)(1 << 24);
    *(--pstack) = (unsigned int)ptask;
    *(--pstack) = (unsigned int)0x14141414u;
    *(--pstack) = (unsigned int)0x12121212u;
    *(--pstack) = (unsigned int)0x03030303u;
    *(--pstack) = (unsigned int)0x02020202u;
    *(--pstack) = (unsigned int)0x01010101u;
    *(--pstack) = (unsigned int)param;
    
    *(--pstack) = (unsigned int)0x11111111u;
    *(--pstack) = (unsigned int)0x10101010u;
    *(--pstack) = (unsigned int)0x09090909u;
    *(--pstack) = (unsigned int)0x08080808u;
    *(--pstack) = (unsigned int)0x07070707u;
    *(--pstack) = (unsigned int)0x06060606u;
    *(--pstack) = (unsigned int)0x05050505u;
    *(--pstack) = (unsigned int)0x04040404u;
        
    ptcb->StackPtr = pstack;
}

void TinyOSInit(void)
{
    CurrentTCBPtr = (void *)0;
    NextTCBPtr    = (void *)0;
    
    TaskOneTCBPtr = &TaskOneTCB;
    TaskTwoTCBPtr = &TaskTwoTCB;
    
    TaskTable[0]  = TaskOneTCBPtr;
    TaskTable[1]  = TaskTwoTCBPtr;
}


void TinyOSStart(void)
{
    if (CurrentTCBPtr == (void *)0)
    {
        NextTCBPtr = TaskTable[0];
    }
    
    TaskRunFirst();
}


void TaskRunFirst(void)
{
    __set_PSP(0);
    
    MEM8(NVIC_SYSPRI14) = NVIC_PENDSV_PRI;
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void TaskSched(void)
{
    if (CurrentTCBPtr == TaskTable[0])
    {
        NextTCBPtr = TaskTable[1];
    }
    else
    {
        NextTCBPtr = TaskTable[0];
    }
    TaskSwitch();
}


void TaskSwitch(void)
{
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}


/** @funcname  example1
  * @brief     None  
  * @param     None
  * @retval    None
  */
__asm void PendSV_Handler(void)
{
    IMPORT  CurrentTCBPtr
    IMPORT  NextTCBPtr
    
    MRS     R0, PSP
    CBZ     R0, PendSVHandler_nosave
    STMDB   R0!, {R4-R11}       //将R4-R11压入任务堆栈
    LDR     R1, =CurrentTCBPtr
    LDR     R1, [R1]
    STR     R0, [R1]            //TCBPtr->StackPtr = R0

PendSVHandler_nosave    
    LDR     R0, =CurrentTCBPtr
    LDR     R1, =NextTCBPtr      
    LDR     R2, [R1]                    
    STR     R2, [R0]                    // CurrentTCBPtr = NextTCBPtr             
    
    LDR     R0, [R2]
    LDMIA   R0!, {R4-R11}               //将R4-R11弹出任务堆栈
    
    MSR     PSP, R0                              
    ORR     LR, LR, #0x04
    
    BX      LR
}



void SysTick_Handler(void) 
{
    TaskSched();
}

__asm void IntDisable(void)
{
	CPSID   I
	BX      LR
}

__asm void IntEnable(void)
{
	CPSIE   I
	BX      LR
}

/************************ (C) COPYRIGHT ZS ************** END OF FILE *********/
