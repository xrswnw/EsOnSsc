#ifndef _ANYID_BOOT_CONFIG_
#define _ANYID_BOOT_CONFIG_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"


//STM32ƽ̨���
#if (defined(_ANYID_STM32F4_))
    #include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_rtc.h"

    //�˿ڶ���
    typedef struct Port_Inf
    {
        GPIO_TypeDef* Port;
        u16 Pin;
    } PORT_INF;
#endif
#define STM32_CPUID_ADDR                (0x1FFF7A10)
#define STM32_CPUID_LEN                 12

//���ȼ�������2:2
//#define INT_PRIORITY_GROUP              2
//#define INT_PRIORITY_STICK              15  //STICK�����ȼ����
//#define INT_PRIORITY_UART_RX            14  //���ڽ���
//#define INT_PRIORITY_RTC                1
//
//
//#define INT_PRIORITY_HIGH               0
//#define INT_PRIORITY_LOW                15

#define INT_PRIORITY_GROUP              2
#define INT_PRIORITY_STICK              0
#define INT_PRIORITY_RTC                1
#define INT_PRIORITY_UART_RX            14  //���ڽ���
#define INT_PRIORITY_UART_TXDMA         8  //���ڷ���DMA
#define INT_PRIORITY_DOUTDMA            4   //ADDMA
#define INT_PRIORITY_DOUTIRDA           4   //ADDMA
#define INT_PRIORITY_EM4094_DIN         0   //15693�ķ��Ͷ�ʱ�����ȼ����
#define INT_PRIORITY_BT_IO              0   //SAM��IO���ݣ���INT_PRIORITY_EM4094_DIN����ͬһ��ʱ�̹���
#define INT_PRIORITY_AUDIO_IO           0


#define INT_PRIORITY_HIGH               0
#define INT_PRIORITY_LOW                15



#define Sys_EnableInt()                 __enable_irq()
#define Sys_DisableInt()                __disable_irq()

#define Sys_SoftReset()                 (*((u32 *)0xE000ED0C) = 0x05fa0004)


#endif

