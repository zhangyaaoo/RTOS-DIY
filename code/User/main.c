/**
  ******************************************************************************
  * @file    main.c 
  * @author  ZS
  * @version 
  * @date    23-Jan-2017
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */

#define __MAIN_C_

/******************************** Includes ************************************/
#include "main.h"



/***************************** Private typedef ********************************/

/***************************** Private define *********************************/

/****************************** Private macro *********************************/

/**************************** Private variables *******************************/
uint32_t FlagOne; 
uint32_t FlagTwo; 



TCB *TaskOneTCBPtr;             //����һ���ƿ�ָ��
TCB *TaskTwoTCBPtr;             //��������ƿ�ָ��

TCB TaskOneTCB;                 //����һ���ƿ�
TCB TaskTwoTCB;                 //��������ƿ�

Stack_t TaskOneStackBuf[1024];  //����һ��ջ
Stack_t TaskTwoStackBuf[1024];  //�������ջ

/*********************** Private function prototypes **************************/
void Delay(int n);
void SetSysTickPeriod(uint32_t ms);

void TaskOne(void *param);
void TaskTwo(void *param);


/**************************** Private functions *******************************/

/** @funcname  main
  * @brief     None  
  * @param     None
  * @retval    None
  */
int main(void)
{
	IntDisable();
	
	SetSysTickPeriod(10);
	
    TinyOSInit();
    
    TaskInit(TaskOne, (void *)0x11111111, TaskOneTCBPtr, &TaskOneStackBuf[1024]);
    TaskInit(TaskTwo, (void *)0x22222222, TaskTwoTCBPtr, &TaskTwoStackBuf[1024]);
 
    TinyOSStart();
	
	IntEnable();
    
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
    }
}


void SetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD  = ms * SystemCoreClock / 1000 - 1; 
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL   = 0;                           
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk; 
}









/************************ (C) COPYRIGHT ZS ************** END OF FILE *********/
