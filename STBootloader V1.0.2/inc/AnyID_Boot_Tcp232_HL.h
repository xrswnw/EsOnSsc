#ifndef _ANYID_BOOT_TCP232_HL_
#define _ANYID_BOOT_TCP232_HL_

#include "AnyID_Boot_Config.h"

#define TCP232_PORT             USART2
#define TCP232_BAUDRARE         115200
#define Tcp232_RxIRQHandler		USART2_IRQHandler

#define Tcp232_EnableTxInt()      do{USART_ClearITPendingBit(TCP232_PORT, USART_FLAG_TC); USART_ITConfig(TCP232_PORT, USART_IT_TC, ENABLE);}while(0)
#define Tcp232_DisableTxInt()     do{USART_ClearITPendingBit(TCP232_PORT, USART_FLAG_TC); USART_ITConfig(TCP232_PORT, USART_IT_TC, DISABLE);}while(0)

#define Tcp232_ReadByte()       ((u16)(TCP232_PORT->DR & (u16)0x01FF))

extern const PORT_INF TCP232_MODE_CFG;
#define Tcp232_DisableCfg()       TCP232_MODE_CFG.Port->BSRRL = TCP232_MODE_CFG.Pin
#define Tcp232_EnableCfg()        TCP232_MODE_CFG.Port->BSRRH = TCP232_MODE_CFG.Pin

extern const PORT_INF TCP232_PORT_RST;
#define Tcp232_DisableRst()       TCP232_PORT_RST.Port->BSRRL = TCP232_PORT_RST.Pin
#define Tcp232_EnableRst()        TCP232_PORT_RST.Port->BSRRH = TCP232_PORT_RST.Pin


void Tcp232_InitInterface(u32 baudrate);
void Tcp232_ConfigInt(void);
void Tcp232_EnableInt(FunctionalState rxState, FunctionalState txState);
void Tcp232_WriteByte(u8 ch);
void Tcp232_WriteBuffer(u8 *pBuffer, u32 len);

#define Tcp232_ClearReg()       Tcp232_ReadByte()

#endif
