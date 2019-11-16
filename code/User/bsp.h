#ifndef __BSP_H_
#define __BSP_H_

#ifdef __BSP_C_
#define BSP_EXT
#else
#define BSP_EXT extern
#endif

#include "tinyos.h"

BSP_EXT void USART2_Config(void);
BSP_EXT uint8_t USART2_Send_Byte(uint8_t data);
BSP_EXT void BspInit(void);

#endif /* __BSP_H_ */
