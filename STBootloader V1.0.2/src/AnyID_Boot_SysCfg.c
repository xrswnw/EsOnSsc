#include "AnyID_Boot_SysCfg.h"
#include "AnyID_Boot_Uart.h"
#include "AnyID_Boot_SysTick_HL.h"
#include "AnyID_Boot_FRam.h"
#include "AnyID_Boot_R485.h"
u32 g_nSysState = 0;
u32 g_nDeviceNxtEraseAddr = 0;
u32 g_nDeviceNxtDownloadAddr = 0;


#define SYS_BOOT_VER_SIZE               50
const u8 SYS_BOOT_VERSION[SYS_BOOT_VER_SIZE] = "Boot V3.1_22032601 GD32F4xx";
void Sys_Delayms(u32 n)
{
    //162.72MHZ
    n *= 0x08000;
    n++;
    while(n--);
}

void Sys_CfgClock(void)
{
    u32 StartUpCounter = 0, HSEStatus = 0;

#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif
    // Reset the RCC clock configuration to the default reset state
    // Set HSION bit
    RCC->CR |= (uint32_t)0x00000001;

    // Reset CFGR register
    RCC->CFGR = 0x00000000;

    // Reset HSEON, CSSON and PLLON bits
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    // Reset PLLCFGR register
    RCC->PLLCFGR = 0x24003010;

    // Reset HSEBYP bit
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    // Disable all interrupts
    RCC->CIR = 0x00000000;

    // Configure the System clock source, PLL Multiplier and Divider factors, AHB/APBx prescalers and Flash settings
    ///****************************************************************************
    //            PLL (clocked by HSE) used as System clock source                *
    ///****************************************************************************
    // Enable HSE
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    // Wait till HSE is ready and if Time out is reached exit
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        // Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        PWR->CR |= PWR_CR_VOS;

        // HCLK = SYSCLK / 1
        // AHB prescaler
        // 162.72
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

        // PCLK2 = HCLK / 2
        // APB high-speed prescaler (APB2)
        // 81.36
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

        // PCLK1 = HCLK / 4
        // APB Low speed prescaler (APB1)
        // 40.68
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

        // Configure the main PLL
        //HSE = 13.56M
        //
        // PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
        #define PLL_M      12
        #define PLL_N      336
        //PLL_N / PLL_M = 24
        //PLL_VCO = 13.56 * 24 = 325.44
        // SYSCLK = PLL_VCO / PLL_P = 162.72
        #define PLL_P      2
        // USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ
        #define PLL_Q      7
        //192<=N<=432   2<=M<=63
        RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

        // Enable the main PLL
        RCC->CR |= RCC_CR_PLLON;

        // Wait till the main PLL is ready
        while((RCC->CR & RCC_CR_PLLRDY) == 0);

        // Configure Flash prefetch, Instruction cache, Data cache and wait state
        FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

        // Select the main PLL as system clock source
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= RCC_CFGR_SW_PLL;

        // Wait till the main PLL is used as system clock source
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    }
    else
    {
        while(1);
    }
}

void Sys_CfgPeriphClk(FunctionalState state)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                           RCC_AHB1Periph_GPIOB |
                             RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOC, ENABLE);

    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
    
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |
                           RCC_APB1Periph_UART5 |
                           RCC_APB1Periph_USART3, ENABLE);  
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG |
                           RCC_APB2Periph_USART1, ENABLE);
}

void Sys_CfgNVIC(void)
{
    // Configure the Vector Table location add offset address ------------------
#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; // Vector Table Relocation in Internal SRAM
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; // Vector Table Relocation in Internal FLASH
#endif
}

const PORT_INF SYS_RUNLED_COM = {GPIOB, GPIO_Pin_15};                       //板载没有LED
const PORT_INF SYS_ALARMLED_COM = {GPIOC, GPIO_Pin_6};
const PORT_INF SYS_ALARMBUZZER_COM = {GPIOB, GPIO_Pin_14};
void Sys_CtrlIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SYS_RUNLED_COM.Pin;
    GPIO_Init(SYS_RUNLED_COM.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SYS_ALARMLED_COM.Pin;
    GPIO_Init(SYS_ALARMLED_COM.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SYS_ALARMBUZZER_COM.Pin;
    GPIO_Init(SYS_ALARMBUZZER_COM.Port, &GPIO_InitStructure);
}
u32 g_nSysRomNum = 0;  
void Sys_Init(void)
{
#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
    //
    Sys_CfgClock();

    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);

    //禁止中断
    Sys_DisableInt();

    Sys_CtrlIOInit();
    Sys_RunLedOn();
    Sys_AlarmLedOn();

    FRam_InitInterface();
    Fram_ReadBootParamenter();
    //如果appState状态正常，但是版本信息校验错误，恢复默认状态
    if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)
    {
        if(Sys_CheckVersion() == FALSE)
        {
            g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
            g_sFramBootParamenter.br = FRAM_BR_9600;
        }
    }
    else if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_NONE)
    {
        g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
        g_sFramBootParamenter.addr = 0x0001;
        g_sFramBootParamenter.br = FRAM_BR_9600;
        if(Sys_CheckVersion() == TRUE && Sys_CheckAppEmpty() == FALSE)          //状态错误，但是版本信息校验正确
        {
            g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
        }
    }
    
   // g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
    
    R485_InitInterface(UART_BAUDRARE);
    R485_ConfigInt();
    R485_EnableInt(ENABLE, DISABLE);

    Lora_InitInterface(LORA_BAUDRARE);
    Lora_ConfigInt();
    Lora_EnableInt(ENABLE, DISABLE);
    //SysTick 初始化 5ms
    STick_InitSysTick();
    Sys_RunLedOff();
    Sys_AlarmLedOff();
  
    g_nSysRomNum = a_GetCrc32((u8 *)STM32_CPUID_ADDR, STM32_CPUID_LEN);
    //系统空闲状态
    if(g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)
    {
        a_SetState(g_nSysState, SYS_STAT_IDLE);
    }
    else
    {
        a_SetState(g_nSysState, SYS_STAT_DOWNLOAD);
    }

    //使能中断
    Sys_EnableInt();
}

void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
        static u32 ledTimes = 0;

        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);

        ledTimes++;

        if(ledTimes & 0x01)
        {
            Sys_AlarmLedOn();
        }
        else
        {
            Sys_AlarmLedOff();
        }

    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
}

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
void Sys_Jump(u32 address)
{
    u32 stackAddr = 0;
    Sys_DisableInt();
    stackAddr = *((vu32 *)address);
    //查看栈地址是否在RAM区或者CCRAM区
    if((stackAddr & 0x2FFE0000) == 0x20000000 || (stackAddr & 0x1FFF0000) == 0x10000000)
    {
        JumpAddress = *(vu32 *)(address + 4);
        Jump_To_Application = (pFunction) JumpAddress;

        __set_MSP(*(vu32 *)address);
        Jump_To_Application();
    }
    //else
    {
        a_SetState(g_nSysState, SYS_STAT_IDLE);
        g_nSysTick = 0;
    }
    //while(1)
    {
    #if SYS_ENABLE_WDT
        WDG_FeedIWDog();
    #endif
    }
    Sys_EnableInt();
}

void Sys_BootTask(void)
{
    if(!a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
    {
        if(g_nSysTick > 200 && g_sFramBootParamenter.appState == FRAM_BOOT_APP_OK)                //TICK = STICK_TIME_MS = 5MS, 延时200ms等待boot选择
        {
            a_SetStateBit(g_nSysState, SYS_STAT_JMP);
        }
    }

    if(a_CheckStateBit(g_nSysState, SYS_STAT_JMP))
    {
        Sys_RunLedOff();
        Sys_Jump(SYS_APP_START_ADDR);
    }
}

void Sys_UartTask(void)
{
    //串口错误处理:重新初始化
    if(USART_GetFlagStatus(UART_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(UART_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        Uart_InitInterface(UART_BAUDRARE);
        Uart_ConfigInt();
        Uart_EnableInt(ENABLE, DISABLE);
    }

    //串口数据帧解析
    if(Uart_IsRcvFrame(g_sUartRcvFrame))
    {
        Sys_ProcessBootFrame(&g_sUartRcvFrame, SYS_COM_UART);
        Uart_ResetFrame(g_sUartRcvFrame);
    }

}

void Sys_R485Task(void)
{
    //串口错误处理:重新初始化
    if(USART_GetFlagStatus(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(R485_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        Uart_InitInterface(R485_BAUDRARE);
        Uart_ConfigInt();
        Uart_EnableInt(ENABLE, DISABLE);
    }

    //串口数据帧解析
    if(Uart_IsRcvFrame(g_sR485RcvFrame))
    {
        Sys_ProcessBootFrame(&g_sR485RcvFrame, SYS_COM_485);
        Uart_ResetFrame(g_sR485RcvFrame);
    }
}
void Sys_Tcp232Task(void)
{
    //串口错误处理:重新初始化
    if(USART_GetFlagStatus(TCP232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(TCP232_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        Tcp232_InitInterface(TCP232_BAUDRARE);
        Tcp232_ConfigInt();
        Tcp232_EnableInt(ENABLE, DISABLE);
    }

    //串口数据帧解析
    if(Uart_IsRcvFrame(g_sTcp232RcvFrame))
    {
        Sys_ProcessBootFrame(&g_sTcp232RcvFrame, SYS_COM_TCP232);
        Uart_ResetFrame(g_sTcp232RcvFrame);
    }
}

void Sys_LoraTask(void)
{
    //串口错误处理:重新初始化
    if(USART_GetFlagStatus(LORA_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE))
    {
        USART_ClearFlag(LORA_PORT, USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
        Lora_InitInterface(LORA_BAUDRARE);
        Lora_ConfigInt();
        Lora_EnableInt(ENABLE, DISABLE);
    }

    //串口数据帧解析
    if(Uart_IsRcvFrame(g_sLoraRcvFrame))
    {
        Sys_ProcessBootFrame(&g_sLoraRcvFrame, SYS_COM_LORA);
        Uart_ResetFrame(g_sLoraRcvFrame);
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_LORACHK))
    {
        if(g_sLoraInfo.sate == LORA_INFO_STAT_LORA_BUSY || g_sLoraInfo.sate == LORA_INFO_STAT_LORA_IDLE)
        {
            if(Lora_DataIdle())
            {
                g_sLoraInfo.sate = LORA_INFO_STAT_LORA_LOADING;
                g_sLoraInfo.busyTick = g_nSysTick;
                g_nSysRomNum = a_lfsr(g_nSysRomNum);
                g_sLoraInfo.romTime = 5 + (g_nSysRomNum % 20);
            }
            else
            {
                g_sLoraInfo.sate = LORA_INFO_STAT_LORA_BUSY;
            }
        }
        else if(g_sLoraInfo.sate == LORA_INFO_STAT_LORA_LOADING)
        {
            if(g_sLoraInfo.busyTick + g_sLoraInfo.romTime <= g_nSysTick)
            {
                if(Lora_DataIdle())
                {
                  g_sLoraInfo.sate = LORA_INFO_STAT_LORA_IDLE;
                }     
                else
                {
                    g_sLoraInfo.sate = LORA_INFO_STAT_LORA_BUSY;
                }
            }
        }
        a_ClearStateBit(g_nSysState, SYS_STAT_LORACHK);
    }
    
    if(a_CheckStateBit(g_nSysState, SYS_STAT_LORATX) && Lora_Idle())
    {
        a_ClearStateBit(g_nSysState, SYS_STAT_LORATX);
        //Lora_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
    }
}

BOOL Sys_CheckVersion(void)
{
    BOOL b = FALSE;
    u8 *p = (u8 *)SYS_BOOT_VER_ADDR;
    u8 i = 0, c = 0;
        
    if(memcmp(p, SYS_VER_HEAD, SYS_VER_HEAD_SIZE) == 0) //设备型号正确
    {
        for(i = SYS_VER_HEAD_SIZE; i < SYS_VERSION_SIZE; i++)
        {
            c = *p++;
            if((c < ' ' || c > 127) && (c != 0x00))
            {
                break;
            }
        }
        if(i == SYS_VERSION_SIZE)
        {
            b = TRUE;
        }
    }
    
    return b;
}

#define SYS_BOOT_CHK_APP_LEN               128
BOOL Sys_CheckAppEmpty(void)
{
    BOOL b = FALSE;
    u16 i = 0;
    u32 *p = (u32 *)(SYS_APP_START_ADDR + 512); //检查第一个扇区的最后512直接
    for(i = 0; i < SYS_BOOT_CHK_APP_LEN; i++)
    {
        if(p[i] != 0xFFFFFFFF)
        {
            break;
        }
    }
    if(i == SYS_BOOT_CHK_APP_LEN)
    {
        b = TRUE;
    }
    
    return b;
}


void Sys_ProcessBootFrame(UART_RCVFRAME *pRcvFrame, u8 com)
{
    u16 crc1 = 0, crc2 = 0;
    u16 destAddr = 0;
    memcpy(&g_sUartTempRcvFrame, pRcvFrame, sizeof(UART_RCVFRAME));


    crc1 = Uart_GetFrameCrc(g_sUartTempRcvFrame.buffer, g_sUartTempRcvFrame.index);
    crc2 = a_GetCrc(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_LEN, g_sUartTempRcvFrame.index - 4);

    if(crc1 == crc2)
    {
      
        destAddr = *((u16 *)(g_sUartTempRcvFrame.buffer + 5));
        if((destAddr != g_sFramBootParamenter.addr) && (destAddr != 0xFFFF))               //0000??FFFF?????
        {
            return ;
        }
        u8 cmd = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_CMD];
        
        //if()
        switch(cmd)
        {
            case UART_FRAME_CMD_RESET:
                g_nSysTick = 0;
                g_sUartTxFrame.len = Uart_RspReset(g_sFramBootParamenter.addr);
                break;
            case UART_FRAME_CMD_ERASE:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bOk = FALSE;
                    u8 sector = 0;
                    sector = g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR];
                    if(sector >= SYS_APP_START_SECTOR)
                    {
                        if(g_nDeviceNxtEraseAddr == sector)                 //擦除地址必须是连续的，否则会有区域未擦除
                        {
                            g_nDeviceNxtEraseAddr = sector + 0x08;          //注意这里增加的数目
                            bOk = Uart_EraseFlash(sector);
                            g_sUartTxFrame.len = Uart_RspErase(bOk, g_sFramBootParamenter.addr);
                        }
                    }
                }
                break;
            case UART_FRAME_CMD_DL:
                if(a_CheckStateBit(g_nSysState, SYS_STAT_DOWNLOAD))
                {
                    BOOL bCheck = FALSE;
                    u32 addr = 0;
                    u32 size = 0;

                    if(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_LEN] == 0x00)
                    {
                        bCheck = (BOOL)(g_sUartTempRcvFrame.buffer[UART_FRAME_POS_PAR + 0]);
                        addr = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 1));
                        size = *((u32 *)(g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 5));
                        if(addr >= SYS_APP_START_ADDR)
                        {
                            //第一次不需要判定地址连续问题，因为boot程序是从后向前写数据，第一次不知道地址是什么
                            if(addr + size == g_nDeviceNxtDownloadAddr || g_nDeviceNxtDownloadAddr == 0)
                            {
                                //帧参数之前部分 + 基本参数(1 + 4 + 4) + size + crclen;
                                //frameLen = UART_FRAME_POS_PAR + 9 + size + 2;
                                Sys_AlarmBuzzerOn();
                                Sys_RunLedOn();
                                Sys_AlarmLedOn();

                                if(Uart_WriteImagePage(addr, g_sUartTempRcvFrame.buffer + UART_FRAME_POS_PAR + 9, size))
                                {
                                    g_sUartTxFrame.len = Uart_RspDownload(bCheck, addr, size, g_sFramBootParamenter.addr);
                                }

                                Sys_AlarmBuzzerOff();
                                Sys_RunLedOff();
                                Sys_AlarmLedOff();
                            }
                        }
                    }
                }
                break;
            case UART_FRAME_CMD_BOOT:
                g_sUartTxFrame.len = Uart_RspBoot(g_sFramBootParamenter.addr);
                if(com == SYS_COM_UART)
                {
                    Uart_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
                    g_sUartTxFrame.len = 0;
                }
				else if(com == SYS_COM_TCP232)
				{
					Tcp232_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
                    g_sUartTxFrame.len = 0;
				}
                else if(com == SYS_COM_485)
                {
                    R485_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
                    g_sUartTxFrame.len = 0;
                }
                else if(com == SYS_COM_LORA)
                {
                   // a_SetStateBit(g_nSysState, SYS_STAT_LORATX);
                    Lora_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
                }
					
                a_ClearStateBit(g_nSysState, SYS_STAT_IDLE);
                a_SetStateBit(g_nSysState, SYS_STAT_DOWNLOAD);
                g_sFramBootParamenter.appState = FRAM_BOOT_APP_FAIL;
                Fram_WriteBootParamenter();
                
                Uart_EraseFlash(SYS_BOOT_VER_SECTOR);               //版本信息区域擦除 ,扇区1（0x084000 - 0x08007FFF）
                g_nDeviceNxtEraseAddr = SYS_APP_START_SECTOR;
                g_nDeviceNxtDownloadAddr = 0;   
                break;
            case UART_FRAME_CMD_JMP:
                if(Sys_CheckVersion() == TRUE)
                {
                    g_sUartTxFrame.len = Uart_RspJmp(g_sFramBootParamenter.addr);
                    a_SetStateBit(g_nSysState, SYS_STAT_JMP);
                    g_sFramBootParamenter.appState = FRAM_BOOT_APP_OK;
                    Fram_WriteBootParamenter();
                }
                break;
            case UART_FRAME_CMD_VER:
                g_sUartTxFrame.len = Uart_RspFrame(g_sUartTxFrame.frame, cmd, (u8 *)SYS_BOOT_VERSION, SYS_BOOT_VER_SIZE, UART_FRAME_FLAG_OK, UART_FRAME_RSP_NOERR, g_sFramBootParamenter.addr);
                break;
            default:
                break;
        }
    }
    if(g_sUartTxFrame.len)
    {
        if(com == SYS_COM_UART)
        {
            Uart_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        }
        else if(com == SYS_COM_TCP232)
        {
            Tcp232_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        }
        else if(com == SYS_COM_485)
        {
            R485_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        }
        else if(com == SYS_COM_LORA)
        {   
            //a_SetStateBit(g_nSysState, SYS_STAT_LORATX);
           Lora_WriteBuffer(g_sUartTxFrame.frame, g_sUartTxFrame.len);
        } 
        Sys_Delayms(2);     //等待最后一个字节发送完成
        g_sUartTxFrame.len = 0;
    }
}

