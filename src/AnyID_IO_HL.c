#include "AnyID_IO_HL.h"

const PORT_INF FMSC_DO_PORT[FMSC_DO_MAX_NUM] = 
{ 
  FMSC_DO_0, FMSC_DO_1, FMSC_DO_2, FMSC_DO_3, FMSC_DO_4, FMSC_DO_5, FMSC_DO_6, FMSC_DO_7, FMSC_DO_8, FMSC_DO_9, FMSC_DO_10, FMSC_DO_11, FMSC_DO_12, FMSC_DO_13, FMSC_DO_14, FMSC_DO_15
};

const PORT_INF FMSC_DI_PORT[FMSC_DI_MAX_NUM] = 
{ 
  FMSC_DI_0, FMSC_DI_1, FMSC_DI_2, FMSC_DI_3, FMSC_DI_4, FMSC_DI_5, FMSC_DI_6, FMSC_DI_7, FMSC_DI_8, FMSC_DI_9, FMSC_DI_10, FMSC_DI_11, FMSC_DI_12, FMSC_DI_13, FMSC_DI_14, FMSC_DI_15
};

u16 g_nIOStatus = 0;

void IO_Init()
{
    u8 index = 0;
    
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    

    for(index = 0; index < FMSC_DO_MAX_NUM; index++)
    {
        GPIO_InitStructure.GPIO_Pin = FMSC_DO_PORT[index].Pin;
        GPIO_Init(FMSC_DO_PORT[index].Port, &GPIO_InitStructure);
    }
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  
    for(index = 0; index < FMSC_DI_MAX_NUM; index++)
    {
        GPIO_InitStructure.GPIO_Pin = FMSC_DI_PORT[index].Pin;
        GPIO_Init(FMSC_DI_PORT[index].Port, &GPIO_InitStructure);
    }
}



void IO_Delayms(u32 n)
{
    n *= 0x7FFF;
    n++;
    while(n--);
}