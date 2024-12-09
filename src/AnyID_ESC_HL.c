#include "AnyID_ESC_HL.h"

const PORT_INF ESC_CS = {GPIOA, GPIO_Pin_4};
const PORT_INF ESC_SPI_PORT = {GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7};
const PORT_INF ESC_SYNC0 = {GPIOA, GPIO_Pin_0};
const PORT_INF ESC_SYNC1 = {GPIOA, GPIO_Pin_1};
const PORT_INF ESC_INT = {GPIOA, GPIO_Pin_2};
void Esc_InitInterface(void)
{
    SPI_InitTypeDef   SPI_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    GPIO_PinAFConfig(ESC_SPI_PORT.Port, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(ESC_SPI_PORT.Port, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(ESC_SPI_PORT.Port, GPIO_PinSource7, GPIO_AF_SPI1);

	//是不是和GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz有关系？
    //cs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz

    GPIO_InitStructure.GPIO_Pin = ESC_CS.Pin;
    GPIO_Init(ESC_CS.Port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = ESC_SPI_PORT.Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(ESC_SPI_PORT.Port, &GPIO_InitStructure);

    SPI_Cmd(ESC_SPI, DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //MAX 50m
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(ESC_SPI, &SPI_InitStructure);

    SPI_Cmd(ESC_SPI, ENABLE);
}

u8 Esc_WriteByte(u8 byte)
{
   if(SPI_I2S_GetFlagStatus(ESC_SPI, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(ESC_SPI);
    }

    while(SPI_I2S_GetFlagStatus(ESC_SPI, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(ESC_SPI, byte);
    while(SPI_I2S_GetFlagStatus(ESC_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    return SPI_I2S_ReceiveData(ESC_SPI);
}

void Esc_IntInit()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

 	GPIO_InitStructure.GPIO_Pin = ESC_INT.Pin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ESC_INT.Port, &GPIO_InitStructure);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);                  //使能SYSCFG时钟
	SYSCFG_EXTILineConfig(ESC_INT_PORT_SOURCE, ESC_INT_PIN_SOURCE);         //PE2 连接到中断线2
	
    EXTI_InitStructure.EXTI_Line = ESC_INT_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
    EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = ESC_INT_IRQn;//外部中断0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure);//配置
}
void Esc_Sync0Init()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = ESC_SYNC0.Pin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ESC_SYNC0.Port, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);                      //使能SYSCFG时钟
	SYSCFG_EXTILineConfig(ESC_SYNC0_PORT_SOURCE, ESC_SYNC0_PIN_SOURCE);         //PE2 连接到中断线2
	
    EXTI_InitStructure.EXTI_Line = ESC_SYNC0_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	 
    NVIC_InitStructure.NVIC_IRQChannel = ESC_SYNC0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;        //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;               //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             
    NVIC_Init(&NVIC_InitStructure);	
	
	EXTI_ClearITPendingBit(ESC_SYNC0_EXTI);
}

void Esc_Sync1Init()
{
    NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = ESC_SYNC1.Pin; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ESC_SYNC1.Port, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(ESC_SYNC1_PORT_SOURCE, ESC_SYNC1_PIN_SOURCE);         //PE2 连接到中断线2
	
    EXTI_InitStructure.EXTI_Line = ESC_SYNC1_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = ESC_SYNC1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;        //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;               //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             
    NVIC_Init(&NVIC_InitStructure);	
	
	EXTI_ClearITPendingBit(ESC_SYNC0_EXTI);
}

void Esc_Delayms(u32 n)
{
    n *= 0x7FFF;
    n++;
    while(n--);
}

void Esc_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	ESC_SPI->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	ESC_SPI->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(ESC_SPI,ENABLE); //使能SPI1
} 

