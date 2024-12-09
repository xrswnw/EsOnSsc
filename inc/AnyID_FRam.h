#ifndef _ANYID_FRAM_
#define _ANYID_FRAM_

#include "AnyID_FRam_App.h"

#define FRAME_INFO_ADDR                     0x000

#define FRAM_BOOT_APP_OK                    0x5555
#define FRAM_BOOT_APP_FAIL                  0xAAAA


#define FRAM_BOOT_BAUDRATE9600              5
#define FRAM_BOOT_BAUDRATE38400             7
#define FRAM_BOOT_BAUDRATE115200            11
typedef struct framBootDevicePar{
    u16 appState;
    u16 br;
    u16 addr;
    u16 crc;
}FRAM_BOOTPARAMS;
extern FRAM_BOOTPARAMS g_sFramBootParamenter;

#define FRAME_MEM_SIZE                      512
#define FRAME_SN_ADDR                       (FRAME_BOOT_INFO_BACKUP_ADDR - sizeof(FRAM_SNINFO))
#define FRAME_BOOT_INFO_BACKUP_ADDR         (FRAME_BOOT_INFO_ADDR - sizeof(FRAM_BOOTPARAMS))
#define FRAME_BOOT_INFO_ADDR                (FRAME_MEM_SIZE - sizeof(FRAM_BOOTPARAMS))  //共512字节，boot是8个字节

#pragma pack(push)  //保存对齐状态
#pragma pack(1)     //设定为1字节对齐
#define FRAME_SN_SIZE                       (32 - 2 - 1)    //CRC + len
typedef struct frameSnInfo{
    u8 len;
    u8 sn[FRAME_SN_SIZE];
    u16 crc;
}FRAM_SNINFO;
//extern FRAM_SNINFO g_sFramSnInfo;
#pragma pack(pop)   //保存对齐状态

BOOL Fram_WriteBootParamenter(void);
BOOL Fram_WriteBackupBootParamenter(void);
void Fram_ReadBootParamenter(void);
u32 Fram_GetBaudrate(void);
#endif

