#ifndef _ANYID_R542_FRAM_
#define _ANYID_R542_FRAM_

#include "AnyID_FRam.h"

#define FRAME_INFO_ADDR     0x100                                               //固件升级从480字节开始写，共512字节
#define FRAM_VAR_ADDR       (FRAME_INFO_ADDR + 3)
#endif

