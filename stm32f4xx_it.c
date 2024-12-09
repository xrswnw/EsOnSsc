#include "stm32f4xx_it.h"
#include "AnyID_SysCfg.h"


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
    //STick_StopSysTick();

    g_nSysTick++;
    
    if(g_nSysTick % 201 == 0x00)
    {
        a_SetState(g_nSysState, SYS_STAT_RUNLED);
    }
}

