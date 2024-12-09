#include "AnyID_Boot_Tcp232_HL.h"

#define TCP232_INT_CHANNEL        USART2_IRQn

//const PORT_INF TCP232_PORT_LNK = {GPIOB, GPIO_Pin_14};
const PORT_INF TCP232_MODE_CFG = {GPIOC, GPIO_Pin_1};
const PORT_INF TCP232_PORT_RST = {GPIOC, GPIO_Pin_0};
const PORT_INF TCP232_PORT_TX = {GPIOA, GPIO_Pin_2};
const PORT_INF TCP232_PORT_RX = {GPIOA, GPIO_Pin_3};
void Tcp232_InitInterface(u32 baudrate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_DeInit(TCP232_PORT);

    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    Tcp232_DisableCfg();
    GPIO_InitStructure.GPIO_Pin = TCP232_MODE_CFG.Pin;
    GPIO_Init(TCP232_MODE_CFG.Port, &GPIO_InitStructure);
    Tcp232_DisableCfg();

    Tcp232_DisableRst();
    GPIO_InitStructure.GPIO_Pin = TCP232_PORT_RST.Pin;
    GPIO_Init(TCP232_PORT_RST.Port, &GPIO_InitStructure);
    Tcp232_DisableRst();

   /* GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = TCP232_PORT_LNK.Pin;
    GPIO_Init(TCP232_PORT_LNK.Port, &GPIO_InitStructure);
*/


    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

    GPIO_InitStructure.GPIO_Pin = TCP232_PORT_TX.Pin;
    GPIO_Init(TCP232_PORT_TX.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TCP232_PORT_RX.Pin;
    GPIO_Init(TCP232_PORT_RX.Port, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(TCP232_PORT, &USART_InitStructure);

    USART_ITConfig(TCP232_PORT, USART_IT_RXNE, ENABLE);
    USART_Cmd(TCP232_PORT, ENABLE);
}

void Tcp232_ConfigInt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = TCP232_INT_CHANNEL;

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UART_RX >> 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_PRIORITY_UART_RX & 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void Tcp232_EnableInt(FunctionalState rxState, FunctionalState txState)
{
    USART_ClearITPendingBit(TCP232_PORT, USART_IT_RXNE | USART_FLAG_TXE);

    USART_ITConfig(TCP232_PORT, USART_IT_RXNE, rxState);
    USART_ITConfig(TCP232_PORT, USART_IT_TC, txState);
}

void Tcp232_WriteByte(u8 ch)
{
    while(((TCP232_PORT)->SR & USART_FLAG_TXE) == (u16)RESET);
	(TCP232_PORT)->DR = (ch & (u16)0x01FF);
}

void Tcp232_WriteBuffer(u8 *pBuffer, u32 len)
{
    u32 i = 0;
    for(i = 0; i < len; i++)
    {
        Tcp232_WriteByte(pBuffer[i]);
    }
}
