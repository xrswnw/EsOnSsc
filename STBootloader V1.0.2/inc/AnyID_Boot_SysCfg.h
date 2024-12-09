#ifndef _ANYID_BOOT_SYSCFG_
#define _ANYID_BOOT_SYSCFG_


#include "AnyID_Boot_Config.h"
#include "AnyID_Boot_Uart.h"
#include "AnyID_Boot_TCP232.h"
#include "AnyID_Boot_Lora.h"

extern u32 g_nSysState;
#define SYS_STAT_IDLE           0x00000001	    //系统初始化后处于空闲状态
#define SYS_STAT_RUNLED         0x00000002
#define SYS_STAT_DOWNLOAD       0x00000040
#define SYS_STAT_JMP            0x00000080
#define SYS_STAT_RESET          0x00000100
#define SYS_STAT_ALARMLED       0x00000200
#define SYS_STAT_LORACHK        0x00000400
#define SYS_STAT_LORATX         0x00000800

void Sys_Jump(u32 address);
typedef  void (*pSysFunction)(void);
//灯
//BSRRH 表示reset
//BSRRL 表示set
extern const PORT_INF SYS_RUNLED_COM;
#define Sys_RunLedOn()              SYS_RUNLED_COM.Port->BSRRH = SYS_RUNLED_COM.Pin
#define Sys_RunLedOff()             SYS_RUNLED_COM.Port->BSRRL = SYS_RUNLED_COM.Pin

extern const PORT_INF SYS_ALARMLED_COM;
#define Sys_AlarmLedOn()            SYS_ALARMLED_COM.Port->BSRRH = SYS_ALARMLED_COM.Pin
#define Sys_AlarmLedOff()           SYS_ALARMLED_COM.Port->BSRRL = SYS_ALARMLED_COM.Pin

extern const PORT_INF SYS_ALARMBUZZER_COM;
#define Sys_AlarmBuzzerOff()        SYS_ALARMBUZZER_COM.Port->BSRRH = SYS_ALARMBUZZER_COM.Pin
#define Sys_AlarmBuzzerOn()         SYS_ALARMBUZZER_COM.Port->BSRRL = SYS_ALARMBUZZER_COM.Pin

void Sys_Delay5ms(u32 n);

void Sys_CfgClock(void);
void Sys_CfgPeriphClk(FunctionalState state);
void Sys_CfgNVIC(void);

#define Sys_SoftReset()     (*((u32 *)0xE000ED0C) = 0x05fa0004)

void Sys_CtrlIOInit(void);
void Sys_Init(void);

void Sys_UartTask(void);
void Sys_ProcessUartFrame(u8 *pBuffer, u16 len);

void Sys_LedTask(void);
void Sys_UartTask(void);
void Sys_BootTask(void);
void Sys_ProcessBootFrame(UART_RCVFRAME *pRcvFrame, u8 com);
void Sys_Tcp232Task(void);
void Sys_R485Task(void);
#define SYS_VER_HEAD                            "D543FNX "
#define SYS_VER_HEAD_SIZE                       6
    
#define SYS_VERSION_SIZE                        50  
#define SYS_BOOT_VER_ADDR                       0x08005000          //版本固定在当前系统，F4单片机的位置和F1/L4都不一样
#define SYS_APP_START_ADDR                      0x08004000
#define SYS_APP_START_SECTOR                    0x08
#define SYS_BOOT_VER_SECTOR                     SYS_APP_START_SECTOR
BOOL Sys_CheckAppEmpty(void);
BOOL Sys_CheckVersion(void);
void Sys_LoraTask(void);
#define SYS_FRAME_BROADCAST_ADDR                0xFFFF

#define SYS_COM_UART        0
#define SYS_COM_TCP232      1
#define SYS_COM_485         2
#define SYS_COM_LORA        3
#endif
