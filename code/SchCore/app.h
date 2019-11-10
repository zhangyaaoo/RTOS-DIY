#ifndef __APP_H_
#define __APP_H_

#ifdef __APP_C_
#define APP_EXT
#else
#define APP_EXT extern
#endif

#include "tinyos.h"

APP_EXT TCB_t *TaskOneTCBPtr;             //����һ���ƿ�ָ��
APP_EXT TCB_t *TaskTwoTCBPtr;             //��������ƿ�ָ��
APP_EXT TCB_t *TaskThreeTCBPtr;           //���������ƿ�ָ��

APP_EXT TCB_t TaskOneTCB;                 //����һ���ƿ�
APP_EXT TCB_t TaskTwoTCB;                 //��������ƿ�
APP_EXT TCB_t TaskThreeTCB;               //���������ƿ�

APP_EXT Stack_t TaskOneStackBuf[1024];    //����һ��ջ
APP_EXT Stack_t TaskTwoStackBuf[1024];    //�������ջ
APP_EXT Stack_t TaskThreeStackBuf[1024];  //��������ջ

APP_EXT void TaskOne(void *param);
APP_EXT void TaskTwo(void *param);
APP_EXT void TaskThree(void *param);

APP_EXT void Delay(int n);

#endif /* __APP_H_ */
