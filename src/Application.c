#define _Application_ 1
#include "Application.h"
#undef _Application_



BOOL App_EcInit()
{
    BOOL bOk = FALSE;
    
    HW_Init();								//PDI过程数据接口初始化
	MainInit(); 							//主函数初始化，包括ESC和COE	
	
    //	ReadCOEParas(UserParaAddr);
	FoE_APPL_Init();
	bRunApplication = TRUE;		            //设备处于运行态标志
    
    return bOk;
}

TOBJ7000 g_sLegacyDoData = {0};
void App_MonitorDate()
{
    if(memcmp(&DOUnit20x7000, &g_sLegacyDoData, sizeof(TOBJ7000)))
    {
        DOCH0 = DOUnit20x7000.DO1;
		DOCH1 = DOUnit20x7000.DO2;
		DOCH2 = DOUnit20x7000.DO3;
		DOCH3 = DOUnit20x7000.DO4;
		DOCH4 = DOUnit20x7000.DO5;
		DOCH5 = DOUnit20x7000.DO6;
		DOCH6 = DOUnit20x7000.DO7;
		DOCH7 = DOUnit20x7000.DO8;

		DOCH8 = DOUnit20x7000.DO9;
		DOCH9 = DOUnit20x7000.DO10;
		DOCH10 = DOUnit20x7000.DO11;
		DOCH11 = DOUnit20x7000.DO12;
		DOCH12 = DOUnit20x7000.DO13;
		DOCH13 = DOUnit20x7000.DO14;
		DOCH14 = DOUnit20x7000.DO15;
		DOCH15 = DOUnit20x7000.DO16;
		memcpy(&g_sLegacyDoData, &DOUnit20x7000, sizeof(TOBJ7000));
    }

    DIUnit10x6000.DI1 = IO_ChkDiTask(DICH0, 0);
    DIUnit10x6000.DI2 = IO_ChkDiTask(DICH1, 1);
    DIUnit10x6000.DI3 = IO_ChkDiTask(DICH2, 2);
    DIUnit10x6000.DI4 = IO_ChkDiTask(DICH3, 3);
    DIUnit10x6000.DI5 = IO_ChkDiTask(DICH4, 4);
    DIUnit10x6000.DI6 = IO_ChkDiTask(DICH5, 5);
    DIUnit10x6000.DI7 = IO_ChkDiTask(DICH6, 6);
    DIUnit10x6000.DI8 = IO_ChkDiTask(DICH7, 7);
    DIUnit10x6000.DI9 = IO_ChkDiTask(DICH8, 8);
    DIUnit10x6000.DI10 = IO_ChkDiTask(DICH9, 9);
    DIUnit10x6000.DI11 = IO_ChkDiTask(DICH10, 10);
    DIUnit10x6000.DI12 = IO_ChkDiTask(DICH11, 11);
    DIUnit10x6000.DI13 = IO_ChkDiTask(DICH12, 12);
    DIUnit10x6000.DI14 = IO_ChkDiTask(DICH13, 13);
    DIUnit10x6000.DI15 = IO_ChkDiTask(DICH14, 14);
    DIUnit10x6000.DI16 = IO_ChkDiTask(DICH15, 15);
}

void StopOutput()
{
	DOCH0 = 0;
	DOCH1 = 0;
	DOCH2 = 0;
	DOCH3 = 0;
	DOCH4 = 0;
	DOCH5 = 0;
	DOCH6 = 0;
	DOCH7 = 0;
	
	DOCH8 = 0;
	DOCH9 = 0;
	DOCH10 = 0;
	DOCH11 = 0;
	DOCH12 = 0;
	DOCH13 = 0;
	DOCH14 = 0;
	DOCH15 = 0;
}

//LED初始化
void GPIO_HW_Init()
{

}

////主站接收的从站pdo数据，映射到COE中
void APPL_CoeTxPdoMapping(u16* pData)
{
    UINT16 *pInputs = (UINT16 *)pData;

	*pInputs++ = *(&DIUnit10x6000.u16SubIndex0 + 1);
}

void APPL_CoeRxPdoMapping(u16* pData)
{
    u16 index = 0, *pTempParams = NULL;
    
    pTempParams = pData;
    for(index = 0; index < sRxPDOassign.u16SubIndex0; index++)
    {
        switch(sRxPDOassign.aEntries[index])
        {
            case 0x1600:
              
                *(&DOUnit20x7000.u16SubIndex0 + 1)= SWAPWORD(*pTempParams);
              break;
            default:
              
              break;
        }
    }
}

void TestCalcTimeFun(u8 Cmd, u8 CurSeq)
{
#ifdef TestEnable
  TestCalcTimeFunction(Cmd, CurSeq);
#endif // TestEnable
}



//   应用层逻辑设计

const u8 READER_VERSION[READER_VERSION_SIZE]@0x08005000 = "";

READER_DEVICE_PARAMETER g_sDeviceParamenter = {0};  
void App_Delayms(u32 n)
{
    n *= 0x7FFF;
    n++;
    while(n--);
}


void App_ResetDeviceParamenter(void)
{
    memset(&g_sDeviceParamenter, 0, sizeof(g_sDeviceParamenter));
}

BOOL App_WriteDeviceParamenter(void)
{
    g_sDeviceParamenter.crc = a_GetCrc((u8 *)(&g_sDeviceParamenter), sizeof(g_sDeviceParamenter) - 2);
    return FRam_WriteBuffer(FRAME_INFO_ADDR, sizeof(g_sDeviceParamenter), (u8 *)(&g_sDeviceParamenter));
}

void App_ReadDeviceParamenter(void)
{
    BOOL b = FALSE;
    if(FRam_ReadBuffer(FRAME_INFO_ADDR, sizeof(g_sDeviceParamenter), (u8 *)(&g_sDeviceParamenter)))
    {
        u16 crc = 0;

        crc = a_GetCrc((u8 *)(&g_sDeviceParamenter), sizeof(g_sDeviceParamenter) - 2);
        if(crc == g_sDeviceParamenter.crc)
        {
            b = TRUE;
        }
    }
    
    if(!b)
    {
        App_ResetDeviceParamenter();
        App_WriteDeviceParamenter();
    }

    Fram_ReadBootParamenter();
    if((g_sFramBootParamenter.appState != FRAM_BOOT_APP_OK) ||
       (g_sFramBootParamenter.br != FRAM_BOOT_BAUDRATE115200) ||
       (g_sFramBootParamenter.addr != g_sDeviceParamenter.addr))
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        g_sFramBootParamenter.br = FRAM_BOOT_BAUDRATE115200;
        g_sFramBootParamenter.addr = g_sDeviceParamenter.addr;
        Fram_WriteBootParamenter();
        Fram_WriteBackupBootParamenter();
    }
}






