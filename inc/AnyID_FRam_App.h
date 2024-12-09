#ifndef _FRAM_HL_
#define _FRAM_HL_

#include "AnyID_FRam_HL.h"


#define FRAM_ADDR_READ          0xA1
#define FRAM_ADDR_WRITE         0xA0
#define FRAM_ADDR_MASK          0x0E

#define FRAM_PAGE_SIZE          32
#define FRAM_PAGE_MASK          0x003F

BOOL FRam_ReadBuffer(u16 addr, u16 len, u8 *pBuffer);
BOOL FRam_WritePage(u16 addr, u16 len, u8 *pBuffer);
BOOL FRam_WriteBuffer(u16 addr, u16 len, u8 *pBuffer);
#endif
