#include "AnyID_Boot_SysCfg.h"
#include "AnyID_Boot_Uart.h"

int main(void)
{
    Sys_Init();
    while(1)
    {
        Sys_BootTask();
        Sys_LedTask();
        //Sys_UartTask();
		//Sys_Tcp232Task();
        Sys_R485Task();
        Sys_LoraTask();
    }
}


#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
{
    while(1)
    {
    }
}
#endif
