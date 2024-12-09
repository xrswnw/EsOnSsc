#ifndef _ANYID_R542_SYSCFG_
#define _ANYID_R542_SYSCFG_


#include "Application.h"

#include "string.h"
#include <stdlib.h>
#define SYS_LET_TEST                    0


extern u32 g_nAlarmDelay;
extern u32 g_nReaderLedDelay;


extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001	    //系统初始化后处于空闲状态
#define SYS_STAT_RUNLED         0x00000002




#define Sys_SoftReset()     (*((u32 *)0xE000ED0C) = 0x05fa0004)

void Sys_Delay5ms(u32 n);
void Sys_Delay1ms(u32 n);
void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);

void Sys_CtrlIOInit(void);
void Sys_Init(void);


void Sys_LedTask(void);
void Sys_EspAppTask();
#endif
