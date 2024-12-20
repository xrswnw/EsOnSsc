#include "AnyID_Boot_R485_HL.h"

#define R485_INT_CHANNEL        USART3_IRQn
const PORT_INF R485_PORT_TX = {GPIOB, GPIO_Pin_10};
const PORT_INF R485_PORT_RX = {GPIOB, GPIO_Pin_11};
const PORT_INF R485_PORT_CTRL = {GPIOC, GPIO_Pin_3};

void R485_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = R485_PORT_TX.Pin;
    GPIO_Init(R485_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R485_PORT_RX.Pin;
    GPIO_Init(R485_PORT_RX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = R485_PORT_CTRL.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(R485_PORT_CTRL.Port, &GPIO_InitStructure);
    
    R485_EnableRx();
    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(R485_PORT, &USART_InitStructure);

    USART_ITConfig(R485_PORT, USART_IT_RXNE, ENABLE);
    USART_Cmd(R485_PORT, ENABLE);
}
void R485_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = R485_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void R485_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(R485_PORT, USART_IT_RXNE | USART_IT_TC);

    USART_ITConfig(R485_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(R485_PORT, USART_IT_TC, txState);
}

void R485_WriteByte(u8 ch)
{
    while(((R485_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(R485_PORT)->DR = (ch & (u16)0x01FF);
}
void R485_Delay5ms(u32 n)
{
    n *= 0x10000;
    n++;
    while(n--);
}

void R485_WriteBuffer(u8 *pFrame, u16 len)
{
    u32 i = 0;
    R485_EnableTx();
    for(i = 0; i < len; i++)
    {
        while(((R485_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	    (R485_PORT)->DR = (pFrame[i] & (u16)0x01FF);
    }
    R485_Delay5ms(2);
    R485_EnableRx();
}
