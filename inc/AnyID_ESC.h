#ifndef _ANYID_ESC_
#define _ANYID_ESC_

#include "AnyID_ESC_HL.h"
#include  "esc.h"
#include "stm32f4xx.h"

#define ESC_CMD_SERIAL_READ                 0x03
#define ESC_CMD_FAST_READ                   0x0B
#define ESC_CMD_DUAL_OP_READ                0x3B
#define ESC_CMD_DUAL_IO_READ                0xBB
#define ESC_CMD_QUAD_OP_READ                0x6B
#define ESC_CMD_QUAD_IO_READ                0xEB
#define ESC_CMD_SERIAL_WRITE                0x02
#define ESC_CMD_DUAL_DATA_WRITE             0x32
#define ESC_CMD_DUAL_ADDR_DATA_WRITE        0xB2
#define ESC_CMD_QUAD_DATA_WRITE             0x62
#define ESC_CMD_QUAD_ADDR_DARA_WRITE        0xE2

#define ESC_CMD_SERIAL_READ_DUMMY           0
#define ESC_CMD_FAST_READ_DUMMY             1
#define ESC_CMD_DUAL_OP_READ_DUMMY          1
#define ESC_CMD_DUAL_IO_READ_DUMMY          2
#define ESC_CMD_QUAD_OP_READ_DUMMY          1
#define ESC_CMD_QUAD_IO_READ_DUMMY          4
#define ESC_CMD_SERIAL_WRITE_DUMMY          0
#define ESC_CMD_DUAL_DATA_WRITE_DUMMY       0
#define ESC_CMD_DUAL_ADDR_DATA_WRITE_DUMMY  0
#define ESC_CMD_QUAD_DATA_WRITE_DUMMY       0
#define ESC_CMD_QUAD_ADDR_DARA_WRITE_DUMMY  0

#define ESC_CSR_CMD_REG                 0x304
#define ESC_CSR_DATA_REG                0x300
#define ESC_WRITE_BYTE                  0x80
#define ESC_READ_BYTE                   0xC0
#define ESC_CSR_BUSY                    0x80

#define ESC_RD                          0x02 /**< \brief Indicates a read access to ESC or EEPROM*/
#define ESC_WR                          0x04 /**< \brief Indicates a write access to ESC or EEPROM.*/

typedef union
{
  unsigned short   Word;
  unsigned char    Byte[2];
} UBYTETOWORD;

typedef union 
{
    u8 Byte[2];
    u16    Word;
}
UALEVENT;

#define SELECT_SPI          {Esc_CsLow();}
#define DESELECT_SPI        {Esc_CsHigh();}
#define RxTxSpiData(u8)     Esc_WriteByte(u8)
#define SPI_Init()          Esc_InitInterface()

#ifdef DC_SUPPORTED

#endif 

#define TIMER_INTERVAL      1 // ms
#define TIMER_INTERVAL      1 // ms
#define PDI_TIMER           TIM3
#define PDI_TIMER_RCU       RCC_APB1Periph_TIM3
//#define PDI_TIMER_IRQn      TIMER6_IRQn

/*---------------------------------------------
-    hardware timer settings
-----------------------------------------------*/

#define ECAT_TIMER_INC_P_MS        1000 /**< \brief 625 ticks per ms*/

#ifndef HW_GetTimer
#define HW_GetTimer() (PDI_GetTimer()) /**< \brief Access to the hardware timer*/
#endif

#ifndef HW_ClearTimer
#define HW_ClearTimer() (PDI_ClearTimer()) /**< \brief Clear the hardware timer*/
#endif

#define HW_EscReadWord(WordValue, Address)              HW_EscRead(((MEM_ADDR *)&(WordValue)),((u16)(Address)),2) /**< \brief 16Bit ESC read access*/
#define HW_EscReadDWord(DWordValue, Address)            HW_EscRead(((MEM_ADDR *)&(DWordValue)),((u16)(Address)),4) /**< \brief 32Bit ESC read access*/
#define HW_EscReadMbxMem(pData,Address,Len)             HW_EscRead(((MEM_ADDR *)(pData)),((u16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default read function is used.*/

#define HW_EscReadWordIsr(WordValue, Address)           HW_EscReadIsr(((MEM_ADDR *)&(WordValue)),((u16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC read access*/
#define HW_EscReadDWordIsr(DWordValue, Address)         HW_EscReadIsr(((MEM_ADDR *)&(DWordValue)),((u16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC read access*/


#define HW_EscWriteWord(WordValue, Address)             HW_EscWrite(((MEM_ADDR *)&(WordValue)),((u16)(Address)),2) /**< \brief 16Bit ESC write access*/
#define HW_EscWriteDWord(DWordValue, Address)           HW_EscWrite(((MEM_ADDR *)&(DWordValue)),((u16)(Address)),4) /**< \brief 32Bit ESC write access*/
#define HW_EscWriteMbxMem(pData,Address,Len)            HW_EscWrite(((MEM_ADDR *)(pData)),((u16)(Address)),(Len)) /**< \brief The mailbox data is stored in the local uC memory therefore the default write function is used.*/

#define HW_EscWriteWordIsr(WordValue, Address)          HW_EscWriteIsr(((MEM_ADDR *)&(WordValue)),((u16)(Address)),2) /**< \brief Interrupt specific 16Bit ESC write access*/
#define HW_EscWriteDWordIsr(DWordValue, Address)        HW_EscWriteIsr(((MEM_ADDR *)&(DWordValue)),((u16)(Address)),4) /**< \brief Interrupt specific 32Bit ESC write access*/

#endif //_AX58100_H_


#if defined(_AX58100_) && (_AX58100_ == 1)
    #define PROTO
#else
    #define PROTO extern
#endif

/*-----------------------------------------------------------------------------------------
------
------    Global functions
------
-----------------------------------------------------------------------------------------*/
PROTO u8 HW_Init(void);
PROTO void HW_Release(void);
PROTO u16 HW_GetALEventRegister(void);

PROTO u16 HW_GetALEventRegister_Isr(void);

PROTO void HW_SetLed(u8 RunLed,u8 ErrLed);

PROTO void HW_EscRead( MEM_ADDR * pData, u16 Address, u16 Len );
PROTO void HW_EscReadIsr( MEM_ADDR *pData, u16 Address, u16 Len );

PROTO void HW_EscWrite( MEM_ADDR *pData, u16 Address, u16 Len );

PROTO void HW_EscWriteIsr( MEM_ADDR *pData, u16 Address, u16 Len );

PROTO u16 PDI_GetTimer(void);
PROTO void   PDI_ClearTimer(void);

PROTO void DISABLE_ESC_INT(void);
PROTO void ENABLE_ESC_INT(void);

//-------------------
void Esc_RegOp(u8 cmd, u16 add);
void PDI_InitInterrupt();
void PDI_InitSyncInterrupt();
void PDI_InitTimerInterrupt(void);
#undef    PROTO
/** @}*/

