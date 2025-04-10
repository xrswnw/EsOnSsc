#ifndef Application_H
#define Application_H

#include "stm32f4xx.h"
#include <stdio.h>

#if defined(_Application_) && (_Application_ == 1)
#define PROTO
#else
#define PROTO extern
#endif




PROTO u8 MaxTestSeq //测试用
#ifdef _Application_
= 5        
#endif
;

PROTO u16 TestCalcTime[10]
#ifdef _Application_
= {0}         
#endif
;
PROTO u8 TestCurSeq
#ifdef _Application_
= 0          //MC
#endif
;
PROTO u8 TestLastSeq
#ifdef _Application_
= 0          //MC
#endif
;




typedef union
{
	u16 Word;
	u8 Byte[2];
	struct
	{	
		unsigned int Byte_1:1;
		unsigned int Byte_2:1;
		unsigned int Byte_3:1;
		unsigned int Byte_4:1;
		unsigned int Byte_5:1;
		unsigned int Byte_6:1;
		unsigned int Byte_7:1;
		unsigned int Byte_8:1;
		unsigned int Byte_9:1;
		unsigned int Byte_10:1;
		unsigned int Byte_11:1;
		unsigned int Byte_12:1;
		unsigned int Byte_13:1;
		unsigned int Byte_14:1;
		unsigned int Byte_15:1;
		unsigned int Byte_16:1;
	}bit;
}WORD;


#define VendorCOEAddr   0x08000900  //COE参数的起始地址

#define BootloaderAddr  0x08000000  //0KB～2KB,bootloader占用前3KB(3*1024)Byte，闪存页大小为1KB，所以每次擦除最小单位为1KB
#define UpdateFlagAddr  0x08000800  //2KB～3KB,固件更新标志存放地址
#define VendorParaAddr  0x08000C00  //3KB～4KB,厂商备份参数存放地址
#define UserParaAddr    0x08001000  //4KB～5KB,用户可设参数存放地址
#define AppAddr         0x08001400  //5KB～43KB,应用程序存放地址

#define FirmStartFlashAddr   0x08010C00   //43KB～128KB固件缓存地址首地址

#define NewFirmFlagAddr  0x08000800  //固件更新标志地址
#define NewFirmCntAddr   0x08000804  //新固件占用字节数存放地址
#define XmlUpdataAddr  0x08000808  //XML更新标志，会因为接收新的固件，写NewFirmFlagAddr而擦除整个扇区，而变为0xFFFF

PROTO u32 FirmEndFlashAddr  //固件缓存地址末端地址，FirmStartFlashAddr+固件大小
#ifdef _Application_
= 0x08010C00
#endif
;

//判断参数是否重置的标志
PROTO u32 ParasKey
#ifdef _Application_
= 0x00007767          //MC
#endif
;


void MY_Application(void);
void APPL_CoeRxPdoMapping(u16* pData);
void APPL_CoeTxPdoMapping(u16* pData);
void StopOutput();
void TestCalcTimeFun(u8 Cmd, u8 Seq);






//   应用层逻辑设计

#include "AnyID_SysTick_HL.h"
#include "AnyID_FRam.h"
#include "AnyID_IO.h"

#include "ecat_def.h"
#include "ecatfoe.h"


#include "SSC-Device.h"
#include "applInterface.h"
#include "bootloaderappl.h"

#include "AnyID_ESC.h"



#define READER_VERSION_SIZE             50
extern const u8 READER_VERSION[READER_VERSION_SIZE];


#define READER_UHF_DELAY_TIME_DEAFULT       0
typedef struct flashDevicePar{
    u8 config;
    u16 addr;
    u16 crc;
}READER_DEVICE_PARAMETER;
extern READER_DEVICE_PARAMETER g_sDeviceParamenter;

void App_Delayms(u32 n);

void App_ResetDeviceParamenter(void);
BOOL App_WriteDeviceParamenter(void);
void App_ReadDeviceParamenter(void);

BOOL App_EcInit();
void App_MonitorDate();


#endif 










