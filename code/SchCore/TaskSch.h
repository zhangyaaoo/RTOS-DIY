/**
  ******************************************************************************
  * @file    TaskSch.h 
  * @author  ZS
  * @version 
  * @date    28-Aug-1993
  * @brief   Header for TaskSch.c module
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */
 
/**************** Define to prevent recursive inclusion ***********************/
#ifndef __TASKSCH_H_
#define __TASKSCH_H_

#ifdef __TASKSCH_C_
#define TASKSCH_EXT
#else 
#define TASKSCH_EXT extern
#endif


/****************************** Includes **************************************/
#include "ARMCM3.h"

/************************** Exported macro ************************************/

#define NVIC_INT_CTRL       0xE000ED04      //Interrupt control state register.
#define NVIC_SYSPRI14       0xE000ED22      //System priority register (priority 14).
#define NVIC_PENDSV_PRI     0xFF            //PendSV priority value (lowest).
#define NVIC_PENDSVSET      0x10000000      //Value to trigger PendSV exception.


#define MEM32(ADDR)         *((volatile unsigned int  *)ADDR)
#define MEM8(ADDR)          *((volatile unsigned char *)ADDR)
    

#define TASK_NUM_MAX        5u

/************************** Exported types ************************************/
typedef struct TaskCtrlBlock
{
    unsigned int *StackPtr;     //Stack Pointer
}TCB;

typedef unsigned int Stack_t;

typedef void TASK_T(void *);    //��������������


/************************* Exported variables *********************************/
TASKSCH_EXT TCB *CurrentTCBPtr;
TASKSCH_EXT TCB *NextTCBPtr;

TASKSCH_EXT TCB *TaskTable[TASK_NUM_MAX];

extern TCB *TaskOneTCBPtr;              //����һ���ƿ�ָ��
extern TCB *TaskTwoTCBPtr;              //��������ƿ�ָ��

extern TCB TaskOneTCB;                  //����һ���ƿ�
extern TCB TaskTwoTCB;                  //��������ƿ�

/************************* Exported constants *********************************/

/************************* Exported functions *********************************/
TASKSCH_EXT void TinyOSInit(void);
TASKSCH_EXT void TinyOSStart(void);
TASKSCH_EXT void TaskInit(TASK_T *ptask, void *param, TCB *ptcb, Stack_t *pstack);
TASKSCH_EXT void TaskSched(void);
TASKSCH_EXT void IntDisable(void);
TASKSCH_EXT void IntEnable(void);





#endif /* __TASKSCH_H_ */

/************************ (C) COPYRIGHT ZS ************** END OF FILE *********/
