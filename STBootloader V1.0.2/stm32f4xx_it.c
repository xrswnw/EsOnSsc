#include "stm32f4xx_it.h"
#include "AnyID_Boot_SysCfg.h"
#include "AnyID_Boot_Uart.h"
#include "AnyID_Boot_SysTick_HL.h"
#include "AnyID_Boot_R485.h"
void NMI_Handler(void)
{
    while (1);
}

void HardFault_Handler(void)
{
    while (1);
}

void MemManage_Handler(void)
{
    while (1);
}

void BusFault_Handler(void)
{
    while (1);
}

void UsageFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void)
{
    while (1);
}
void DebugMon_Handler(void)
{
    while (1);
}

void PendSV_Handler(void)
{
    while (1);
}

void SysTick_Handler(void)
{
    STick_StopSysTick();

    g_nSysTick++;

    if(g_nSysTick % 11 == 0)
    {
        a_SetStateBit(g_nSysState, SYS_STAT_RUNLED);
    }
    a_SetStateBit(g_nSysState, SYS_STAT_LORACHK);

    Uart_IncIdleTime(STICK_TIME_MS, g_sUartRcvFrame);
    Uart_IncIdleTime(STICK_TIME_MS, g_sR485RcvFrame);
    Uart_IncIdleTime(STICK_TIME_MS, g_sTcp232RcvFrame);
    STick_StartSysTick();
}

void Tcp232_RxIRQHandler()
{
	if(USART_GetITStatus(TCP232_PORT, USART_IT_RXNE) != RESET)
    {
        u8 byte = 0;
        USART_ClearITPendingBit(TCP232_PORT, USART_IT_RXNE);
        byte = Tcp232_ReadByte();
        Uart_ReceiveFrame(byte, &g_sTcp232RcvFrame);
    }
    else
    {
        Tcp232_ReadByte();
    }
    TCP232_PORT->SR &= (~0x3FF);

}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(UART_PORT, USART_IT_RXNE) != RESET)
    {
        u8 byte = 0;
        USART_ClearITPendingBit(UART_PORT, USART_IT_RXNE);
        byte = Uart_ReadByte();

       Uart_ReceiveFrame(byte, &g_sUartRcvFrame);
    }
    UART_PORT->SR &= (~0x3FF);
}


void R485_RxIRQHandler(void)
{
    if(USART_GetITStatus(R485_PORT, USART_IT_RXNE) != RESET)
    {
        u8 byte = 0;
        USART_ClearITPendingBit(R485_PORT, USART_IT_RXNE);
        byte = R485_ReadByte();
        Uart_ReceiveFrame(byte, &g_sR485RcvFrame);
    }
    else
    {
        Uart_ReadByte();
    }
    R485_PORT->SR &= (~0x3FF);
}


void Lora_RxIRQHandler(void)
{
    if(USART_GetITStatus(LORA_PORT, USART_IT_RXNE) != RESET)
    {
        u8 byte = 0;
        USART_ClearITPendingBit(LORA_PORT, USART_IT_RXNE);
        byte = Lora_ReadByte();
        Uart_ReceiveFrame(byte, &g_sLoraRcvFrame);
    }
    else
    {
        Lora_ReadByte();
    }
    LORA_PORT->SR &= (~0x3FF);
}
