#ifndef _ANYIDIO_HL_
#define _ANYIDIO_HL_

#include "AnyID_R542_Config.h"

#define FMSC_DO_MAX_NUM         16

#define  FMSC_DO_0              {GPIOD, GPIO_Pin_14}
#define  FMSC_DO_1              {GPIOD, GPIO_Pin_15}
#define  FMSC_DO_2              {GPIOD, GPIO_Pin_0}
#define  FMSC_DO_3              {GPIOD, GPIO_Pin_1}
#define  FMSC_DO_4              {GPIOE, GPIO_Pin_7}
#define  FMSC_DO_5              {GPIOE, GPIO_Pin_8}
#define  FMSC_DO_6              {GPIOE, GPIO_Pin_9}
#define  FMSC_DO_7              {GPIOE, GPIO_Pin_10}
#define  FMSC_DO_8              {GPIOE, GPIO_Pin_11}
#define  FMSC_DO_9              {GPIOE, GPIO_Pin_12}
#define  FMSC_DO_10             {GPIOE, GPIO_Pin_13}
#define  FMSC_DO_11             {GPIOE, GPIO_Pin_14}
#define  FMSC_DO_12             {GPIOE, GPIO_Pin_15}
#define  FMSC_DO_13             {GPIOD, GPIO_Pin_8}
#define  FMSC_DO_14             {GPIOD, GPIO_Pin_9}
#define  FMSC_DO_15             {GPIOD, GPIO_Pin_10}


extern const PORT_INF FMSC_DO_PORT[FMSC_DO_MAX_NUM];
extern u16 g_nIOStatus;
#define IO_CtrOutHigh(i)         do{FMSC_DO_PORT[i].Port->BSRRL = FMSC_DO_PORT[i].Pin; g_nIOStatus |= (1 << i);}while(0)
#define IO_CtrOutLow(i)          do{FMSC_DO_PORT[i].Port->BSRRH = FMSC_DO_PORT[i].Pin; g_nIOStatus &= ~(1 << i);}while(0)


#define FMSC_DI_MAX_NUM         16

#define  FMSC_DI_0              {GPIOF, GPIO_Pin_0}
#define  FMSC_DI_1              {GPIOF, GPIO_Pin_2}
#define  FMSC_DI_2              {GPIOF, GPIO_Pin_4}
#define  FMSC_DI_3              {GPIOF, GPIO_Pin_6}
#define  FMSC_DI_4              {GPIOF, GPIO_Pin_8}
#define  FMSC_DI_5              {GPIOF, GPIO_Pin_10}
#define  FMSC_DI_6              {GPIOF, GPIO_Pin_12}
#define  FMSC_DI_7              {GPIOF, GPIO_Pin_14}
#define  FMSC_DI_8              {GPIOG, GPIO_Pin_0}
#define  FMSC_DI_9              {GPIOG, GPIO_Pin_2}
#define  FMSC_DI_10             {GPIOG, GPIO_Pin_4}
#define  FMSC_DI_11             {GPIOG, GPIO_Pin_6}
#define  FMSC_DI_12             {GPIOG, GPIO_Pin_8}
#define  FMSC_DI_13             {GPIOG, GPIO_Pin_10}
#define  FMSC_DI_14             {GPIOG, GPIO_Pin_12}
#define  FMSC_DI_15             {GPIOG, GPIO_Pin_14}

extern const PORT_INF FMSC_DI_PORT[FMSC_DI_MAX_NUM];
//

void IO_Init();
void IO_Delayms(u32 n);

////

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum)                       ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDRE(addr)                             *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)                      MEM_ADDRE(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr                              (GPIOA_BASE + 20) //0x40020014
#define GPIOB_ODR_Addr                              (GPIOB_BASE + 20) //0x40020414 
#define GPIOC_ODR_Addr                              (GPIOC_BASE + 20) //0x40020814 
#define GPIOD_ODR_Addr                              (GPIOD_BASE + 20) //0x40020C14 
#define GPIOE_ODR_Addr                              (GPIOE_BASE + 20) //0x40021014 
#define GPIOF_ODR_Addr                              (GPIOF_BASE + 20) //0x40021414    
#define GPIOG_ODR_Addr                              (GPIOG_BASE + 20) //0x40021814   
#define GPIOH_ODR_Addr                              (GPIOH_BASE + 20) //0x40021C14    
#define GPIOI_ODR_Addr                              (GPIOI_BASE + 20) //0x40022014     

#define GPIOA_IDR_Addr                              (GPIOA_BASE + 16) //0x40020010 
#define GPIOB_IDR_Addr                              (GPIOB_BASE + 16) //0x40020410 
#define GPIOC_IDR_Addr                              (GPIOC_BASE + 16) //0x40020810 
#define GPIOD_IDR_Addr                              (GPIOD_BASE + 16) //0x40020C10 
#define GPIOE_IDR_Addr                              (GPIOE_BASE + 16) //0x40021010 
#define GPIOF_IDR_Addr                              (GPIOF_BASE + 16) //0x40021410 
#define GPIOG_IDR_Addr                              (GPIOG_BASE + 16) //0x40021810 
#define GPIOH_IDR_Addr                              (GPIOH_BASE + 16) //0x40021C10 
#define GPIOI_IDR_Addr                              (GPIOI_BASE + 16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)                                    BIT_ADDR(GPIOA_ODR_Addr, n)  //输出 
#define PAin(n)                                     BIT_ADDR(GPIOA_IDR_Addr, n)  //输入 

#define PBout(n)                                    BIT_ADDR(GPIOB_ODR_Addr, n)  //输出 
#define PBin(n)                                     BIT_ADDR(GPIOB_IDR_Addr, n)  //输入 

#define PCout(n)                                    BIT_ADDR(GPIOC_ODR_Addr, n)  //输出 
#define PCin(n)                                     BIT_ADDR(GPIOC_IDR_Addr, n)  //输入 

#define PDout(n)                                    BIT_ADDR(GPIOD_ODR_Addr, n)  //输出 
#define PDin(n)                                     BIT_ADDR(GPIOD_IDR_Addr, n)  //输入 

#define PEout(n)                                    BIT_ADDR(GPIOE_ODR_Addr, n)  //输出 
#define PEin(n)                                     BIT_ADDR(GPIOE_IDR_Addr, n)  //输入

#define PFout(n)                                    BIT_ADDR(GPIOF_ODR_Addr, n)  //输出 
#define PFin(n)                                     BIT_ADDR(GPIOF_IDR_Addr, n)  //输入

#define PGout(n)                                    BIT_ADDR(GPIOG_ODR_Addr, n)  //输出 
#define PGin(n)                                     BIT_ADDR(GPIOG_IDR_Addr, n)  //输入

#define PHout(n)                                    BIT_ADDR(GPIOH_ODR_Addr, n)  //输出 
#define PHin(n)                                     BIT_ADDR(GPIOH_IDR_Addr, n)  //输入

#define PIout(n)                                    BIT_ADDR(GPIOI_ODR_Addr, n)  //输出 
#define PIin(n)                                     BIT_ADDR(GPIOI_IDR_Addr, n)  //输入

//LED灯
#define DOCH0                                       PDout(14)
#define DOCH1                                       PDout(15)
#define DOCH2                                       PDout(0)
#define DOCH3                                       PDout(1)
#define DOCH4                                       PEout(7)
#define DOCH5                                       PEout(8)
#define DOCH6                                       PEout(9)
#define DOCH7                                       PEout(10)
#define DOCH8                                       PEout(11)
#define DOCH9                                       PEout(12)
#define DOCH10                                      PEout(13)
#define DOCH11                                      PEout(14)
#define DOCH12                                      PEout(15)
#define DOCH13                                      PDout(8)
#define DOCH14                                      PDout(9)
#define DOCH15                                      PDout(10)


#define DICH0                                     ((FMSC_DI_PORT[0].Port->IDR & FMSC_DI_PORT[0].Pin) != (uint32_t)Bit_RESET)
#define DICH1                                     ((FMSC_DI_PORT[1].Port->IDR & FMSC_DI_PORT[1].Pin) != (uint32_t)Bit_RESET)
#define DICH2                                     ((FMSC_DI_PORT[2].Port->IDR & FMSC_DI_PORT[2].Pin) != (uint32_t)Bit_RESET)
#define DICH3                                     ((FMSC_DI_PORT[3].Port->IDR & FMSC_DI_PORT[3].Pin) != (uint32_t)Bit_RESET)
#define DICH4                                     ((FMSC_DI_PORT[4].Port->IDR & FMSC_DI_PORT[4].Pin) != (uint32_t)Bit_RESET)
#define DICH5                                     ((FMSC_DI_PORT[5].Port->IDR & FMSC_DI_PORT[5].Pin) != (uint32_t)Bit_RESET)
#define DICH6                                     ((FMSC_DI_PORT[6].Port->IDR & FMSC_DI_PORT[6].Pin) != (uint32_t)Bit_RESET)
#define DICH7                                     ((FMSC_DI_PORT[7].Port->IDR & FMSC_DI_PORT[7].Pin) != (uint32_t)Bit_RESET)
#define DICH8                                     ((FMSC_DI_PORT[8].Port->IDR & FMSC_DI_PORT[8].Pin) != (uint32_t)Bit_RESET)
#define DICH9                                     ((FMSC_DI_PORT[9].Port->IDR & FMSC_DI_PORT[9].Pin) != (uint32_t)Bit_RESET)
#define DICH10                                    ((FMSC_DI_PORT[10].Port->IDR & FMSC_DI_PORT[10].Pin) != (uint32_t)Bit_RESET)
#define DICH11                                    ((FMSC_DI_PORT[11].Port->IDR & FMSC_DI_PORT[11].Pin) != (uint32_t)Bit_RESET)
#define DICH12                                    ((FMSC_DI_PORT[12].Port->IDR & FMSC_DI_PORT[12].Pin) != (uint32_t)Bit_RESET)
#define DICH13                                    ((FMSC_DI_PORT[13].Port->IDR & FMSC_DI_PORT[13].Pin) != (uint32_t)Bit_RESET)
#define DICH14                                    ((FMSC_DI_PORT[14].Port->IDR & FMSC_DI_PORT[14].Pin) != (uint32_t)Bit_RESET)
#define DICH15                                    ((FMSC_DI_PORT[15].Port->IDR & FMSC_DI_PORT[15].Pin) != (uint32_t)Bit_RESET)

#endif

