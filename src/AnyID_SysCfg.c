#include "AnyID_SysCfg.h"

u32 g_nSysState = SYS_STAT_IDLE;

#define SYS_ENABLE_WDT              0

#define SYS_LED_DISPLAY             0

void Sys_Delayms(u32 n)
{
    n *= 0x7FFF;
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
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

        // PCLK2 = HCLK / 2
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

        // PCLK1 = HCLK / 4
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

        // Configure the main PLL
        //HSE = 8M
        // PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
        #define PLL_M      12
        #define PLL_N      336
        // SYSCLK = PLL_VCO / PLL_P
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
                           RCC_AHB1Periph_GPIOC |
                           RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE |
                           RCC_AHB1Periph_GPIOF |
                           RCC_AHB1Periph_GPIOG , ENABLE);

    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG |
                           RCC_APB2Periph_SPI1 |
                           RCC_APB2Periph_EXTIT, ENABLE);
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

void Sys_CtrlIOInit(void)
{

}

void Sys_Init(void)
{
    Sys_CfgClock();
    Sys_CfgNVIC();
    Sys_CfgPeriphClk(ENABLE);

#if SYS_ENABLE_WDT
    WDG_InitIWDG();
#endif
    //禁止中断
    Sys_DisableInt();
    
    RCC_ClocksTypeDef RccClock = {0};
	RCC_GetClocksFreq(&RccClock);
    
	HW_Init();								//PDI过程数据接口初始化
	MainInit(); 							//主函数初始化，包括ESC和COE	
	
    //	ReadCOEParas(UserParaAddr);
	FoE_APPL_Init();
	bRunApplication = TRUE;		//设备处于运行态标志
  
    Sys_Delayms(100);
  
    STick_InitSysTick();

    IO_Init();
    IO_Test();
    Sys_EnableInt();
    
    //系统空闲状态
    a_SetState(g_nSysState, SYS_STAT_IDLE); //开机采集一次电池电量和温度
}


u32 g_nSysAlarmDelay = 0;
void Sys_LedTask(void)
{
    if(a_CheckStateBit(g_nSysState, SYS_STAT_RUNLED))
    {
      /*  u8 index = 0;

        index  = rand() % FMSC_IO_MAX_NUM;
        
        if((1 << index) & g_nIOStatus)
        {
            IO_CtrOutLow(index);
        }
        else
        {
            IO_CtrOutHigh(index);
        }
        a_ClearStateBit(g_nSysState, SYS_STAT_RUNLED);
        */
    }   

}

void Sys_EspAppTask()
{
    if(bRunApplication)
    {
        MainLoop();
        MY_Application();	//档频却齇B初始化完成
    }
    else
    {
        HW_Release();           //异常停止，硬件层复位操作
    }
}