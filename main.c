#include "AnyID_SysCfg.h"

int main(void)
{
    Sys_Init();
    while(1)
    {
        Sys_LedTask();
        Sys_EspAppTask();
    }
}
