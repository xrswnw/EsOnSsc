#include "AnyID_IO.h"

void IO_Test()
{
    u8 index = 0;
    
    for(index = 0; index < FMSC_DO_MAX_NUM; index++)
    {
        IO_CtrOutHigh(index);
        IO_Delayms(100);
    }

    for(index = 0; index < FMSC_DO_MAX_NUM; index++)
    {
        IO_CtrOutLow(index);
        IO_Delayms(100);
    }
}

DI_CHKSTATUS g_sDIChkStatus[FMSC_DI_MAX_NUM];
BOOL IO_ChkDiTask(u8 value, u8 index)
{
    DI_CHKSTATUS *chkSatus = NULL;
  
    chkSatus = g_sDIChkStatus + index;
    if(chkSatus->value ^ (BOOL)value)
    {
        if(chkSatus->tick >= 3)
        {
            chkSatus->value = (BOOL)value;
        }
        else
        {
            chkSatus->tick++;
        }
    }
    else
    {
        chkSatus->tick = 0;
    }
    
    return chkSatus->value;
}