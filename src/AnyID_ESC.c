#include "AnyID_ESC.h"

/*
* 修改日志：
* V1.00 - 20250410
* TR5832方部分源码修正
*/

#include "ecat_def.h"
#include "ecatappl.h"
#include "Application.h"

UALEVENT EscALEvent;

//如本地有多个中断，此处需要将所有中断屏蔽，不可遗漏；
//所有本地中断抢占优先级必须相同，禁止抢占；
//中断服务程序中不能进行开或关中断的操作，防止应用程序在关中断操作中，被打断进入中断服务程序，退出时开中断引起的部分关中断操作失败。
//部分函数直接由协议栈调用，不修改其命名，否则协议栈倘若更换部分适配性差

void ENABLE_ESC_INT()
{
	NVIC_EnableIRQ(ESC_INT_IRQn);
    
    #ifdef DC_SUPPORTED
        NVIC_EnableIRQ(ESC_SYNC0_IRQn);
        NVIC_EnableIRQ(ESC_SYNC1_IRQn);
    #endif 
}

void DISABLE_ESC_INT()
{
	NVIC_DisableIRQ(ESC_INT_IRQn);
    
    #ifdef DC_SUPPORTED
        NVIC_DisableIRQ(ESC_SYNC0_IRQn);
        NVIC_DisableIRQ(ESC_SYNC1_IRQn);
    #endif 
}

void Eec_Test()
{ 
    u8 i=0;
    u16 addr[4]={0}, reg[4]={0}, len=4, startAddr = 0x0000, startEn = 1;
   
	while(1)
	{
		if(startEn == 1)
		{
            for(i = 0; i < len; i++)
            {
                addr[i] = startAddr + (i * 2);
                HW_EscWriteWord(reg[i], addr[i]);
                HW_EscReadWord(reg[i], addr[i]);
            }
            HW_EscWriteWord(reg[i], addr[i]);
            HW_EscReadWord(reg[i + 1], addr[i]);
            Esc_Delayms(1);
		}
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		Esc_Delayms(300);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		Esc_Delayms(200);
	}
}

u8 HW_Init(void)				//硬件层面初始化，ESC到PDI中断的映射
{
    u32 intMask = 0;
  
    SPI_Init();
	do
	{
        intMask = 0x93;
        HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
        intMask = 0;
        HW_EscReadDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
	} while (intMask != 0x93);
	
    intMask = 0x00;
	HW_EscWriteDWord(intMask, ESC_AL_EVENTMASK_OFFSET);
    
#ifdef DC_SUPPORTED				//DC时钟支持
    PDI_InitSyncInterrupt();             
#endif
    
	PDI_InitTimerInterrupt();
	PDI_InitInterrupt();

    ENABLE_ESC_INT();
    
    return 0;
}

static void GetInterruptRegister(void)                              //拉取AL事件寄存器
{
    HW_EscRead((MEM_ADDR *)&EscALEvent.Word, ESC_AL_EVENT_OFFSET, 2);
}

static void ISR_GetInterruptRegister(void)
{
    HW_EscReadIsr((MEM_ADDR *)&EscALEvent.Word, ESC_AL_EVENT_OFFSET, 2);
}

void HW_Release(void)
{
    //释放硬件资源
}

u16 HW_GetALEventRegister(void)
{
  GetInterruptRegister();
  return EscALEvent.Word;
}

u16 HW_GetALEventRegister_Isr(void)
{
  ISR_GetInterruptRegister();
  return EscALEvent.Word;
}

void HW_EscRead(MEM_ADDR *pData, u16 Address, u16 Len )
{
    u16 i = Len;
    u8 *pTmpData = (u8 *)pData;

    /* loop for all bytes to be read */
    while ( i-- > 0 )
    {
      DISABLE_ESC_INT();
      SELECT_SPI
        
      Esc_RegOp(ESC_RD, Address);
      *pTmpData = RxTxSpiData(0xFF);
      pTmpData++;
      Address++;

      DESELECT_SPI
      ENABLE_ESC_INT();
    }
}

void HW_EscReadIsr( MEM_ADDR *pData, u16 Address, u16 Len )
{
    u16 i = Len;
    u8 data = 0;
    u8 *pTmpData = (u8 *)pData;

    SELECT_SPI
      
    Esc_RegOp(ESC_RD, Address);
    
    while ( i-- > 0 )
    {
        if ( i == 0 )
        {
            data = 0xFF;
        }

        *pTmpData = RxTxSpiData(data);
        pTmpData++;
    }
    
    DESELECT_SPI
}

void HW_EscWrite( MEM_ADDR *pData, u16 Address, u16 Len )
{
    u16 i = Len;
    VARVOLATILE u8 dummy;
    u8 *pTmpData = (u8 *)pData;
    
    while ( i-- > 0 )
    {
        DISABLE_ESC_INT();
        SELECT_SPI
      
        Esc_RegOp(ESC_WR, Address);
    
        dummy = RxTxSpiData(*pTmpData);
        pTmpData++;
        Address++;

        DESELECT_SPI
        ENABLE_ESC_INT();
    }
}

void HW_EscWriteIsr( MEM_ADDR *pData, u16 Address, u16 Len )
{
    u16 i = Len;
    VARVOLATILE u16 dummy;
    u8 *pTmpData = (u8 *)pData;

    SELECT_SPI
      
    Esc_RegOp(ESC_WR, Address);
    while ( i-- > 0 )
    {

         dummy = RxTxSpiData(*pTmpData);
         pTmpData++;
    }

    /* there has to be at least 15 ns + CLK/2 after the transmission is finished
       before the SPI1_SEL signal shall be 1 */
    DESELECT_SPI
}

void Esc_RegOp(u8 cmd, u16 add)
{
    u16 spiCmd = 0;
    
    spiCmd = ((add << 3) | cmd);

    RxTxSpiData((spiCmd & 0xFF00) >> 8);
    RxTxSpiData((spiCmd & 0x00FF) >> 0);
}

void Esc_IntIRQHandler(void)                                                    //  ExtInterruptHandler(void)  AL事件中断
{
	if(EXTI_GetITStatus(ESC_INT_EXTI) == SET)
	{
        TestCalcTimeFun(1, 2);
		PDI_Isr();
        TestCalcTimeFun(0, 2);
		/* reset the interrupt flag */
		ESC_ACK_ESC_INT;
	}
}

void PDI_InitInterrupt()                                                        //PDI中断，主要是ESC和SYNC，若有必要，ESC其GPIO亦可
{   
	Esc_IntInit();
}

void PDI_InitTimerInterrupt(void)                                               //Pdi接口定时拉取数据
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(PDI_TIMER_RCU, ENABLE);                 
	
    TIM_TimeBaseInitStructure.TIM_Period = 1999;                    
	TIM_TimeBaseInitStructure.TIM_Prescaler = 84;                     
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(PDI_TIMER,&TIM_TimeBaseInitStructure);
	
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                                    //不用打开中断，无需处理，底层累计时间判断
	TIM_Cmd(PDI_TIMER,ENABLE); 		
}

u16 PDI_GetTimer()
{
	return ((u16)TIM_GetCounter(PDI_TIMER));
}

void PDI_ClearTimer(void)
{
	TIM_SetCounter(PDI_TIMER,0);
}

#ifdef DC_SUPPORTED
#define SYNC0

    void SYNC0_INT_Init(void)
    {
        Esc_Sync0Init();
    }

    void SYNC1_INT_Init(void)
    {
        Esc_Sync1Init();
    }

    void ESC_SYNC0IRQHandler(void)
    {
        if(EXTI_GetITStatus(ESC_SYNC0_EXTI) == SET)
        {
            Sync0_Isr();
            /* reset the interrupt flag */
            ESC_ACK_SYNC0_INT;
        }
    }

    void ESC_SYNC1IRQHandler(void)   // Sync1Isr(void)
    {	
        if(EXTI_GetITStatus(ESC_SYNC1_EXTI) == SET)
        {
            Sync1_Isr();
            /* reset the interrupt flag */
            ESC_ACK_SYNC1_INT;
        }
    }

    void PDI_InitSyncInterrupt()
    {
        SYNC0_INT_Init();
        SYNC1_INT_Init();
    }
#endif
