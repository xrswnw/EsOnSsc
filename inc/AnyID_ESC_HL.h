#ifndef _ANYID_ESC_HL_
#define _ANYID_ESC_HL_

#include "AnyID_R542_Config.h"

#define ESC_SPI             SPI1

extern const PORT_INF ESC_CS;
#define Esc_CsLow()       GPIO_ResetBits(ESC_CS.Port, ESC_CS.Pin)
#define Esc_CsHigh()      GPIO_SetBits(ESC_CS.Port, ESC_CS.Pin)


#define ESC_INT_RCU             RCC_AHB1Periph_GPIOA
#define ESC_INT_PORT            GPIOA
#define ESC_INT_PORT_SOURCE     EXTI_PortSourceGPIOA
#define ESC_INT_PIN             GPIO_Pin_2
#define ESC_INT_EXTI            EXTI_Line2
#define ESC_INT_PIN_SOURCE      EXTI_PinSource2
#define ESC_INT_IRQn      	    EXTI2_IRQn
#define ESC_ACK_ESC_INT         {EXTI_ClearITPendingBit(ESC_INT_EXTI);}//清除LINE0上的中断标志位  
#define Esc_IntIRQHandler       EXTI2_IRQHandler

extern const PORT_INF ESC_SYNC0;
#define ESC_SYNC0_PORT_SOURCE       EXTI_PortSourceGPIOA
#define ESC_SYNC0_PIN               GPIO_Pin_0
#define ESC_SYNC0_EXTI              EXTI_Line0
#define ESC_SYNC0_PIN_SOURCE        EXTI_PinSource0
#define ESC_SYNC0_IRQn              EXTI0_IRQn
#define ESC_ACK_SYNC0_INT           {EXTI_ClearITPendingBit(ESC_SYNC0_EXTI);}//清除LINE0上的中断标志位  
#define ESC_SYNC0IRQHandler         EXTI0_IRQHandler

extern const PORT_INF ESC_SYNC1;
#define ESC_SYNC1_PORT_SOURCE       EXTI_PortSourceGPIOA
#define ESC_SYNC1_PIN               GPIO_Pin_1
#define ESC_SYNC1_EXTI              EXTI_Line1
#define ESC_SYNC1_PIN_SOURCE        EXTI_PinSource1
#define ESC_SYNC1_IRQn              EXTI1_IRQn
#define ESC_ACK_SYNC1_INT           {EXTI_ClearITPendingBit(ESC_SYNC1_EXTI);}//清除LINE0上的中断标志位  
#define ESC_SYNC1IRQHandler         EXTI1_IRQHandler

void Esc_InitInterface(void);
u8 Esc_WriteByte(u8 byte);
void Esc_Delayms(u32 n);
void Esc_SetSpeed(u8 SPI_BaudRatePrescaler);
void Esc_IntInit();
void Esc_Sync0Init();
void Esc_Sync1Init();
#endif

