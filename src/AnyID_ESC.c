#include "AnyID_ESC.h"

/*
* 修改日志：
* V0.01 - 2021/12/1
* 合并AX598100和ecatmain文件
* 使用ENABLE_ESC_INT/DISABLE_ESC_INT，因为SSC生成的代码中使用此语句
*/

#include "ecat_def.h"
#include "ecatappl.h"

#include "Application.h"


UALEVENT EscALEvent;            //contains the content of the ALEvent register (0x220), this variable is updated on each Access to the Esc

//如本地有多个中断，此处需要将所有中断屏蔽，不可遗漏；
//所有本地中断抢占优先级必须相同，禁止抢占；
//中断服务程序中不能进行开或关中断的操作，防止应用程序在关中断操作中，被打断进入中断服务程序，退出时开中断引起的部分关中断操作失败。
void ENABLE_ESC_INT()
{
	//NVIC_EnableIRQ(x); //应用程序中断函数
	NVIC_EnableIRQ(ESC_INT_IRQn);
    
    #ifdef DC_SUPPORTED
        NVIC_EnableIRQ(ESC_SYNC0_IRQn);
        NVIC_EnableIRQ(ESC_SYNC1_IRQn);
    #endif 
}

void DISABLE_ESC_INT()
{
	//NVIC_DisableIRQ(x);//应用程序中断函数
	NVIC_DisableIRQ(ESC_INT_IRQn);
    
    #ifdef DC_SUPPORTED
        NVIC_DisableIRQ(ESC_SYNC0_IRQn);
        NVIC_DisableIRQ(ESC_SYNC1_IRQn);
    #endif 
}

//外部中断初始化
void ESC_INT_Init(void)
{
    Esc_IntInit();
}

/*******************************************************************************
    Function:
        void ether_cat_escirq_cb()

    Summary:
        Interrupt service routine for the interrupt from ESC
*******************************************************************************/

void ESC_INTIRQHandler(void) //  ExtInterruptHandler(void)  AL事件中断
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

/*******************************************************************************
    Function:
        void ether_cat_sync0_cb()

    Summary:
        Interrupt service routine for the interrupt from SYNC0
*******************************************************************************/
void ESC_SYNC0IRQHandler(void)
{
	if(EXTI_GetITStatus(ESC_SYNC0_EXTI) == SET)
	{
		Sync0_Isr();
		/* reset the interrupt flag */
		ESC_ACK_SYNC0_INT;
	}
}
///*******************************************************************************
//    Function:
//        void ether_cat_sync1_cb()

//    Summary:
//        Interrupt service routine for the interrupt from SYNC1
//*******************************************************************************/
void ESC_SYNC1IRQHandler(void)   // Sync1Isr(void)
{	
  	if(EXTI_GetITStatus(ESC_SYNC1_EXTI) == SET)
	{
        Sync1_Isr();
        /* reset the interrupt flag */
        ESC_ACK_SYNC1_INT;
    }
}

/*******************************************************************************
    Function:
        void PDI_Init_SYNC_Interrupts()

    Summary:
        Register Callback function for PDI SYNC0 and SYNC1 interrupts
*******************************************************************************/
void PDI_Init_SYNC_Interrupts()
{
	SYNC0_INT_Init();
	SYNC1_INT_Init();
}
#endif // DC_SUPPORTED


/*******************************************************************************
    Function:
        void PDI_Init_SYNC_Interrupts()

    Summary:
        Register Callback function for PDI ESC interrupts
*******************************************************************************/
void PDI_IRQ_Interrupt()
{
	ESC_INT_Init();
}
/*******************************************************************************
    Function:
        UINT16 PDI_GetTimer(void)

    Summary:
        Get the 1ms current timer value
    Description:
        This routine gets the 1ms current timer value.
*******************************************************************************/
UINT16 PDI_GetTimer()
{
	return ((UINT16)TIM_GetCounter(PDI_TIMER));
}

/*******************************************************************************
    Function:
        void PDI_ClearTimer(void)

    Summary:
        Clear the 1ms current timer value
    Description:
        This routine clears the 1ms current timer value.
*******************************************************************************/
void PDI_ClearTimer(void)
{
	TIM_SetCounter(PDI_TIMER,0);
}

/*******************************************************************************
    Function:
    void PDI_Timer_Interrupt(void)

    Summary:
     This function configure and enable the TIMER interrupt for 1ms

    Description:
    This function configure and enable the TIMER interrupt for 1ms
*******************************************************************************/
void PDI_Timer_Interrupt(void)                                      //狗时钟
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	RCC_APB1PeriphClockCmd(PDI_TIMER_RCU, ENABLE);                 
	
    TIM_TimeBaseInitStructure.TIM_Period = 1999;                    
	TIM_TimeBaseInitStructure.TIM_Prescaler = 84;                     //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(PDI_TIMER,&TIM_TimeBaseInitStructure);         //初始化TIM3
	
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                        //不用打开中断，无需处理，底层累计时间判断
	TIM_Cmd(PDI_TIMER,ENABLE); //使能定时器3		
}

void ESC_TEST()
{
    u16 Addr[4]={0};
    u16 RegVal[4]={0};
    u16 RdLen=4;
    u16 StartAddr = 0x0000;
    u16 StartEn = 1;
    u8 i=0;
    
	while(1)
	{
		if(StartEn == 1 )
		{
            for(i=0;i<RdLen;i++)
            {
                Addr[i]=StartAddr+i*2;
                HW_EscWriteWord(RegVal[i], Addr[i]);
                HW_EscReadWord(RegVal[i], Addr[i]);
            }
            HW_EscWriteWord(RegVal[i], Addr[i]);
            HW_EscReadWord(RegVal[i+1], Addr[i]);
            Esc_Delayms(1);
		}
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		Esc_Delayms(300);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		Esc_Delayms(200);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0 if initialization was successful

 \brief    This function intialize the Process Data Interface (PDI) and the host controller.
*////////////////////////////////////////////////////////////////////////////////////////
UINT8 HW_Init(void)				//硬件层面初始化，ESC到PDI中断的映射
{
    UINT32 intMask = 0;
  
    SPI_Init();
//	ESC_TEST();
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
	PDI_Init_SYNC_Interrupts();             
#endif
    
	PDI_Timer_Interrupt();
	PDI_IRQ_Interrupt();

    ENABLE_ESC_INT();
    
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Command    ESC_WR performs a write access; ESC_RD performs a read access.

 \brief The function addresses the EtherCAT ASIC via SPI for a following SPI access.
*////////////////////////////////////////////////////////////////////////////////////////
static void AddressingEsc( UINT16 Address, UINT8 Command )
{
  VARVOLATILE UBYTETOWORD tmp;
  tmp.Word = ( Address << 3 ) | Command;
  /* select the SPI */
  SELECT_SPI;
  /* send the first address/command byte to the ESC 
     receive the first AL Event Byte*/
  EscALEvent.Byte[0] = RxTxSpiData(tmp.Byte[1]);
  EscALEvent.Byte[1] = RxTxSpiData(tmp.Byte[0]);

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief  The function operates a SPI access without addressing.

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void GetInterruptRegister(void)
{
    VARVOLATILE UINT8            dummy;
    HW_EscRead((MEM_ADDR *)&dummy, 0, 1);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief  The function operates a SPI access without addressing.
        Shall be implemented if interrupts are supported else this function is equal to "GetInterruptRegsiter()"

        The first two bytes of an access to the EtherCAT ASIC always deliver the AL_Event register (0x220).
        It will be saved in the global "EscALEvent"
*////////////////////////////////////////////////////////////////////////////////////////
static void ISR_GetInterruptRegister(void)
{
  VARVOLATILE UINT8 dummy;
  HW_EscReadIsr((MEM_ADDR *)&dummy, 0, 1);
}
/*ECATCHANGE_END(V5.12) EL9800 1*/



/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    This function shall be implemented if hardware resources need to be release
        when the sample application stops
*////////////////////////////////////////////////////////////////////////////////////////
void HW_Release(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  This function gets the current content of ALEvent register
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister(void)
{
  GetInterruptRegister();
  return EscALEvent.Word;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    first two Bytes of ALEvent register (0x220)

 \brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_GetALEventRegister()"
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 HW_GetALEventRegister_Isr(void)
{
  ISR_GetInterruptRegister();
  return EscALEvent.Word;
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param RunLed            desired EtherCAT Run led state
 \param ErrLed            desired EtherCAT Error led state

  \brief    This function updates the EtherCAT run and error led
*////////////////////////////////////////////////////////////////////////////////////////
//void HW_SetLed(UINT8 RunLed,UINT8 ErrLed)
//{
////      LED_ECATGREEN = RunLed;
////      LED_ECATRED   = ErrLed;
//}

/*ECATCHANGE_START(V5.12) EL9800 1*/
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  This function operates the SPI read access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscRead( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{
    /* HBu 24.01.06: if the SPI will be read by an interrupt routine too the
                     mailbox reading may be interrupted but an interrupted
                     reading will remain in a SPI transmission fault that will
                     reset the internal Sync Manager status. Therefore the reading
                     will be divided in 1-byte reads with disabled interrupt */
    UINT16 i = Len;
    UINT8 *pTmpData = (UINT8 *)pData;

    /* loop for all bytes to be read */
    while ( i-- > 0 )
    {
      /*ECATCHANGE_START(V5.12) EL9800 1*/
      /* the reading of data from the ESC can be interrupted by the
       AL Event ISR, in that case the address has to be reinitialized,
       in that case the status flag will indicate an error because
       the reading operation was interrupted without setting the last
       sent byte to 0xFF */
      DISABLE_ESC_INT();
      /*ECATCHANGE_END(V5.12) EL9800 1*/

      AddressingEsc( Address, ESC_RD );
//			RxTxSpiData(0xFF);
      /*Each Byte will be read with a new addressing phase so the out data is 0xFF*/
      *pTmpData = RxTxSpiData(0xFF);

      pTmpData++;

      /*ECATCHANGE_START(V5.12) EL9800 1*/        
      /* there has to be at least 15 ns + CLK/2 after the transmission is finished
      before the SPI1_SEL signal shall be 1 */
      DESELECT_SPI
      ENABLE_ESC_INT();
      /*ECATCHANGE_END(V5.12) EL9800 1*/
      /* next address */
      Address++;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves read data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

\brief  The SPI PDI requires an extra ESC read access functions from interrupts service routines.
        The behaviour is equal to "HW_EscRead()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscReadIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{
    UINT16 i = Len;
    UINT8 data = 0;

   UINT8 *pTmpData = (UINT8 *)pData;

    /* send the address and command to the ESC */
     AddressingEsc( Address, ESC_RD );
//			RxTxSpiData(0xFF);
    /* loop for all bytes to be read */
    while ( i-- > 0 )
    {
        if ( i == 0 )
        {
            /* when reading the last byte the DI pin shall be 1 */
            data = 0xFF;
        }

        *pTmpData = RxTxSpiData(data);

        pTmpData++;
    }
    
    /* there has to be at least 15 ns + CLK/2 after the transmission is finished
       before the SPI1_SEL signal shall be 1 */
    DESELECT_SPI
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

  \brief  This function operates the SPI write access to the EtherCAT ASIC.
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWrite( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{
    UINT16 i = Len;
    VARVOLATILE UINT8 dummy;

    UINT8 *pTmpData = (UINT8 *)pData;

    /* loop for all bytes to be written */
    while ( i-- > 0 )
    {
/*ECATCHANGE_START(V5.12) EL9800 1*/
        /* the reading of data from the ESC can be interrupted by the
           AL Event ISR, so every byte will be written separate */
        DISABLE_ESC_INT();
/*ECATCHANGE_END(V5.12) EL9800 1*/

        /* HBu 24.01.06: wrong parameter ESC_RD */
         AddressingEsc( Address, ESC_WR );

         dummy = RxTxSpiData(*pTmpData);
         pTmpData++;

        /* there has to be at least 15 ns + CLK/2 after the transmission is finished
           before the SPI1_SEL signal shall be 1 */

        DESELECT_SPI
        ENABLE_ESC_INT();
/*ECATCHANGE_END(V5.12) EL9800 1*/

        /* next address */
        Address++;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param pData        Pointer to a byte array which holds data to write or saves write data.
 \param Address     EtherCAT ASIC address ( upper limit is 0x1FFF )    for access.
 \param Len            Access size in Bytes.

 \brief  The SPI PDI requires an extra ESC write access functions from interrupts service routines.
        The behaviour is equal to "HW_EscWrite()"
*////////////////////////////////////////////////////////////////////////////////////////
void HW_EscWriteIsr( MEM_ADDR *pData, UINT16 Address, UINT16 Len )
{
    UINT16 i = Len;
    VARVOLATILE UINT16 dummy;
    UINT8 *pTmpData = (UINT8 *)pData;

    /* send the address and command to the ESC */
     AddressingEsc( Address, ESC_WR );
     /* loop for all bytes to be written */
    while ( i-- > 0 )
    {

         dummy = RxTxSpiData(*pTmpData);
         pTmpData++;
    }

    /* there has to be at least 15 ns + CLK/2 after the transmission is finished
       before the SPI1_SEL signal shall be 1 */
    DESELECT_SPI
}
/*ECATCHANGE_END(V5.12) EL9800 1*/
