#define __BSP_C_
#include "tinyos.h"
#include "bsp.h"

void USART2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef   USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* System Clocks Configuration */
    /* Enable USART2 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure the GPIO ports */
    /* Configure USART2_Rx(PA3) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2_Tx(PA2) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* NVIC configuration */

    /* Configure the NVIC Preemption Priority Bits */
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART2 configuration */
    /* USART2 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable USART2 Receive interrupts */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);

    USART_ClearFlag(USART2, USART_FLAG_TC);

    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
}

uint8_t USART2_Send_Byte(uint8_t data)
{
    uint8_t temp = 0;

    if (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {
        USART_SendData(USART1, data);
    }
    else
    {
        temp = 0x01;
    }

    return temp;
}

void USART2_IRQHandler(void)
{
    uint8_t temp1;
    uint8_t temp2;

    temp1 = USART2->SR;
    if (temp1 & 0x20)
    {
        temp2 = USART_ReceiveData(USART2);
    }
    else
    {
        temp2 = USART_ReceiveData(USART2);
        USART2->SR &= 0xC0;
        temp1++; temp2++;   //防止变量未使用的警告
    }
}

void BspInit(void)
{
    SetSysTickPeriod(10);
    USART2_Config();
}

