#ifndef _ANYID_UART_RECEIVE_
#define _ANYID_UART_RECEIVE_

#include "AnyID_Type.h"
#include "AnyID_Stdlib.h"

#define UART_BUFFER_MAX_LEN                 (32 + 512)
#define UART_TXT_FRM_DATA_MAX_LEN           255
#define UART_TXT_FRM_DATA_MIN_LEN           0
#define UART_TXT_FRM_FLAG_HEAD1             'a'
#define UART_TXT_FRM_FLAG_HEAD2             'b'
#define UART_TXT_FRM_FLAG_TRAIL1            '\r'
#define UART_TXT_FRM_FLAG_TRAIL2            '\n'

#define UART_TXT_FRM_POS_HEAD1              0
#define UART_TXT_FRM_POS_HEAD2              1
#define UART_TXT_FRM_POS_ADDR               2
#define UART_TXT_FRM_POS_LEN                4
#define UART_TXT_FRM_POS_CMD                6
#define UART_TXT_FRM_POS_PAR                8

#define UART_TXT_FRM_MIN_LEN                12


#define UART_HEX_FRM_FLAG_HEAD1             0x7E
#define UART_HEX_FRM_FLAG_HEAD2             0x55

#define UART_HEX_FRM_MIN_LEN                11

#define UART_HEX_FRM_POS_HEAD1              0
#define UART_HEX_FRM_POS_HEAD2              1
#define UART_HEX_FRM_POS_LEN                2
#define UART_HEX_FRM_POS_SRCADDR            3
#define UART_HEX_FRM_POS_DESTADDR           5
#define UART_HEX_FRM_POS_CMD                7
#define UART_HEX_FRM_POS_RFU                8
#define UART_HEX_FRM_POS_PAR                9

#define UART_HEX_RFRM_POS_HEAD1             0
#define UART_HEX_RFRM_POS_HEAD2             1
#define UART_HEX_RFRM_POS_LEN               2
#define UART_HEX_RFRM_POS_SRCADDR           3
#define UART_HEX_RFRM_POS_DESTADDR          5
#define UART_HEX_RFRM_POS_FLAG              7
#define UART_HEX_RFRM_POS_CMD               8
#define UART_HEX_RFRM_POS_RFU               9
#define UART_HEX_RFRM_POS_PAR               10

#define UART_HEX_FRM_RESPONSE_FLAG          0x1F
#define UART_HEX_FRM_PARAM_RFU              0x00

#define UART_FLAG_RCV                       0x80
#define UART_STAT_IDLE                      0x00
#define UART_STAT_HEAD1                     0x81
#define UART_STAT_HEAD2                     0x82
#define UART_STAT_LEN1                      0x83
#define UART_STAT_LEN2                      0x84
#define UART_STAT_DATA                      0x85
#define UART_STAT_TO                        0x06
#define UART_STAT_END                       0x07


//bootÏà¹Ø
#define UART_FRAME_CMD_BOOT             1
#define UART_FRAME_CMD_DL               2
#define UART_FRAME_CMD_JMP              3
#define UART_FRAME_CMD_RESET            4
#define UART_FRAME_CMD_ERASE            5

typedef struct uartRcvFrame{
    u8 state;
    u16 rcvLen;
    u16 framePos;
    u16 frameLen;
    u8 buffer[UART_BUFFER_MAX_LEN];
    u32 idleTime;
}UART_RCVFRAME;

void Uart_ReceiveFrame(u8 byte, UART_RCVFRAME *pRcvFrame);
#define Uart_IncIdleTime(t, rcvFrame)      do{\
                                                if((rcvFrame).state & UART_FLAG_RCV)\
                                                {\
                                                    (rcvFrame).idleTime++;\
                                                    if((rcvFrame).idleTime >= 2)\
                                                    {\
                                                        (rcvFrame).state = UART_STAT_TO;\
                                                    }\
                                                }\
                                            }while(0)

BOOL Uart_IsRcvFrameOver(UART_RCVFRAME *pRcvFrame);
#define Uart_IsRcvFrame(rcvFrame)           (Uart_IsRcvFrameOver(&(rcvFrame)))

#define Uart_ResetFrame(rcvFrame)           (memset(&(rcvFrame), 0, sizeof(UART_RCVFRAME)))

extern const u8 g_aAsictoData[256];
#define Uart_GetDataFromAsic(p)             ((g_aAsictoData[(p)[0]] << 4) | (g_aAsictoData[(p)[1]]))

extern const u8 g_aDatatoAsic[16];
#define Uart_GetAsicFromData(p, d)          ((p)[0] = g_aDatatoAsic[((d) >> 4) & 0x0F], (p)[1] = g_aDatatoAsic[((d) >> 0) & 0x0F])

BOOL Uart_CheckTxtFrame(UART_RCVFRAME *pRcvFrame);


#define Uart_GetHexFrameLength(p)           (((p)[UART_HEX_FRM_POS_LEN]) + 3)
#define Uart_GetHexFrameCrc(p, len)         (*((u16 *)(p + len - 2)))
BOOL Uart_CheckHexFrame(UART_RCVFRAME *pRcvFrame);

#endif

