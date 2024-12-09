#ifndef _ANYID_RSYSTICK_HL_
#define _ANYID_SYSTICK_HL_


#include "AnyID_R542_Config.h"


extern vu32 g_nSysTick;
extern vs32 g_nSysDelay;

#define STICK_1MS                           168000                    //72M��ϵͳʱ��



void STick_InitSysTick(void);

#define STick_StartSysTick()                SysTick->CTRL  |= SysTick_CTRL_ENABLE_Msk
#define STick_StopSysTick()                 SysTick->VAL = 0;  SysTick->CTRL  &= (~SysTick_CTRL_ENABLE_Msk)

#define STick_SetDelayTime(delay)           (g_nSysDelay = delay) //��һ��TickΪ��λ
#define STick_IsDelayTimeOver()             (g_nSysDelay <= 0)



#define STICK_TIME_MS                       5


#endif

