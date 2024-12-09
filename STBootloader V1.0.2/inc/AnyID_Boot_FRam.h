#ifndef _ANYID_BOOT_FRAM_
#define _ANYID_BOOT_FRAM_

#include "AnyID_FRam.h"


#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA
#define FRAM_BOOT_APP_NONE                  0xFFFF      //crc校验错误，或者IIC读取失败

#define FRAM_BR_9600                        0
#define FRAM_BR_38400                       1
#define FRAM_BR_115200                      2

#define FRAM_BR_NR9600                      5
#define FRAM_BR_NR38400                     7
#define FRAM_BR_NR115200                    11
typedef struct framBootDevicePar{
    u16 appState;
    u16 br;
    u16 addr;
    u16 crc;
}FRAM_BOOTPARAMS;
extern FRAM_BOOTPARAMS g_sFramBootParamenter;


#define FRAME_MEM_SIZE                      512
#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_BOOT_INFO_ADDR - sizeof(FRAM_BOOTPARAMS))
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS))  //共512字节，boot是8个字节


void Fram_ReadBootParamenter(void);
BOOL Fram_WriteBootParamenter(void);
u32 Fram_GetUartBaudrate(void);

BOOL Fram_WriteBackupBootParamenter(void);
#endif

