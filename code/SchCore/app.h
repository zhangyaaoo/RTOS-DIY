#ifndef __APP_H_
#define __APP_H_

#ifdef __APP_C_
#define APP_EXT
#else
#define APP_EXT extern
#endif

#include "tinyos.h"

APP_EXT TCB_t   TaskOneTCB;                 //任务一控制块
APP_EXT TCB_t  *TaskOneTCBPtr;              //任务一控制块指针
APP_EXT Stack_t TaskOneStackBuf[1024];      //任务一堆栈

APP_EXT TCB_t   TaskTwoTCB;                 //任务二控制块
APP_EXT TCB_t  *TaskTwoTCBPtr;              //任务二控制块指针
APP_EXT Stack_t TaskTwoStackBuf[1024];      //任务二堆栈

APP_EXT TCB_t   TaskThreeTCB;               //任务三控制块
APP_EXT TCB_t  *TaskThreeTCBPtr;            //任务三控制块指针
APP_EXT Stack_t TaskThreeStackBuf[1024];    //任务三堆栈

APP_EXT TCB_t   TaskFourTCB;                //任务四控制块
APP_EXT TCB_t  *TaskFourTCBPtr;             //任务四控制块指针
APP_EXT Stack_t TaskFourStackBuf[1024];     //任务四堆栈

APP_EXT void TaskOne(void *param);
APP_EXT void TaskTwo(void *param);
APP_EXT void TaskThree(void *param);
APP_EXT void TaskFour(void *param);

APP_EXT void Delay(int n);

#endif /* __APP_H_ */
