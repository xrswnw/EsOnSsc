#ifndef _ANYID_R542_WDG_HL_
#define _ANYID_R542_WDG_HL_

#include "AnyID_R542_Config.h"


void WDG_InitIWDG(void);
BOOL WDG_IsIWDGReset(void);


#define WDG_FeedIWDog()	IWDG_ReloadCounter()


#endif

