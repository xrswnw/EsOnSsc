#ifndef _ANYID_IO_
#define _ANYID_IO_

#include "AnyID_IO_HL.h"



typedef struct dIChkStaus
{
    BOOL value;
    u8 tick;
    u8 rfu;
}DI_CHKSTATUS;

extern DI_CHKSTATUS g_sDIChkStatus[FMSC_DI_MAX_NUM];

#define IO_CHKINDEX(index)      g_sDIChkStatus(index)

void IO_Test();
BOOL IO_ChkDiTask(u8 value, u8 index);
#endif

