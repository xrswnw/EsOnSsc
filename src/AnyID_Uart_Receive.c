#include "AnyID_Uart_Receive.h"

const u8 g_aAsictoData[256] =
{
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const u8 g_aDatatoAsic[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void Uart_ReceiveFrame(u8 byte, UART_RCVFRAME *pRcvFrame)
{
    if((pRcvFrame->state != UART_STAT_END) || (pRcvFrame->state != UART_STAT_TO))
    {
        if(pRcvFrame->rcvLen < UART_BUFFER_MAX_LEN)
        {
            pRcvFrame->buffer[pRcvFrame->rcvLen++] = byte;
            pRcvFrame->state |= UART_FLAG_RCV;
            pRcvFrame->idleTime = 0;
        }
        else
        {
           pRcvFrame->state = UART_STAT_END;
        }
    }
}

BOOL Uart_CheckTxtFrame(UART_RCVFRAME *pRcvFrame)
{
    BOOL b = FALSE;
    u16 pos = 0;
    u8 len = 0;
    u8 sum1 = 0, sum2 = 0;
    u16 i = 0;

    if(pRcvFrame->rcvLen >= UART_TXT_FRM_MIN_LEN)
    {
        while(pos + UART_TXT_FRM_MIN_LEN <= pRcvFrame->rcvLen)
        {
            if(pRcvFrame->buffer[pos + 0] == UART_TXT_FRM_FLAG_HEAD1 && pRcvFrame->buffer[pos + 1] == UART_TXT_FRM_FLAG_HEAD2)
            {
                len = (Uart_GetDataFromAsic(pRcvFrame->buffer + pos + UART_TXT_FRM_POS_LEN) << 1) + UART_TXT_FRM_MIN_LEN;
                if(pRcvFrame->buffer[pos + len - 1] == UART_TXT_FRM_FLAG_TRAIL2 && pRcvFrame->buffer[pos + len - 2] == UART_TXT_FRM_FLAG_TRAIL1)
                {
                    sum1 = Uart_GetDataFromAsic(pRcvFrame->buffer + pos + len - 4);
                    for(i = pos + 2; i < pos + len - 4; i++)
                    {
                        sum2 += pRcvFrame->buffer[i];
                    }
                    if(sum1 == sum2)
                    {
                        b = TRUE;
                        pRcvFrame->frameLen = len;
                        pRcvFrame->framePos = pos;
                        break;
                    }
                }
            }
            pos++;
        }
    }

    return b;
}



BOOL Uart_CheckHexFrame(UART_RCVFRAME *pRcvFrame)
{
    u16 frameLen = 0;
    u16 framePos = 0;
    u16 crc1 = 0, crc2 = 0;
    
    while((framePos + UART_HEX_FRM_MIN_LEN) <= pRcvFrame->rcvLen)
    {
        if(pRcvFrame->buffer[framePos + 0] == UART_HEX_FRM_FLAG_HEAD1 && pRcvFrame->buffer[framePos + 1] == UART_HEX_FRM_FLAG_HEAD2)
        {
            frameLen = Uart_GetHexFrameLength(pRcvFrame->buffer + framePos);
            if((framePos + frameLen) <= pRcvFrame->rcvLen && frameLen >= UART_HEX_FRM_MIN_LEN)
            {
                crc1 = Uart_GetHexFrameCrc(pRcvFrame->buffer + framePos, frameLen);
                crc2 = a_GetCrc(pRcvFrame->buffer + framePos + UART_HEX_FRM_POS_LEN, frameLen - 4);

                if(crc1 == crc2)
                {
                    pRcvFrame->framePos = framePos;
                    pRcvFrame->frameLen = frameLen;
                    return TRUE;
                }
            }
            else if(frameLen == (0x00 + 0x03))//³¬³¤Ö¡
            {
                if(pRcvFrame->rcvLen > UART_TXT_FRM_MIN_LEN)
                {
                    crc1 = Uart_GetHexFrameCrc(pRcvFrame->buffer + framePos, pRcvFrame->rcvLen);
                    crc2 = a_GetCrc(pRcvFrame->buffer + framePos + UART_HEX_FRM_POS_LEN, pRcvFrame->rcvLen - 4);
                    if(crc1 == crc2)
                    {
                        pRcvFrame->framePos = framePos;
                        pRcvFrame->frameLen = frameLen;
                        return TRUE;
                    }
                }
            }
        }
        framePos++;
    }

    return FALSE;
}

BOOL Uart_IsRcvFrameOver(UART_RCVFRAME *pRcvFrame)
{
    BOOL bOver = FALSE;
    if(pRcvFrame->state == UART_STAT_TO)
    {
        if(pRcvFrame->rcvLen > 0)
        {
            bOver = TRUE;
        }
        else
        {
            pRcvFrame->state = UART_STAT_IDLE;
        }
    }
    else if(pRcvFrame->state == UART_STAT_END)
    {
        bOver = TRUE;
    }

    return bOver;
}
/*
BOOL Uart_CheckHexFrame(UART_RCVFRAME *pRcvFrame, u16 *pframePos, u16 *pFrameLen)
{
    u16 frameLen = 0;
    u16 framePos = 0;
    while((framePos + UART_HEX_FRM_MIN_LEN) <= pRcvFrame->index)
    {
        if(pRcvFrame->buffer[framePos + 0] == UART_HEX_FRM_FLAG_HEAD1 && pRcvFrame->buffer[framePos + 1] == UART_HEX_FRM_FLAG_HEAD2)
        {
            frameLen = Uart_GetFrameLength(pRcvFrame->buffer + framePos);
            if((framePos + frameLen) <= pRcvFrame->index && frameLen >= UART_HEX_FRM_MIN_LEN)
            {
                u16 crc1 = 0, crc2 = 0;
                crc1 = Uart_GetFrameCrc(pRcvFrame->buffer + framePos, frameLen);
                crc2 = a_GetCrc(pRcvFrame->buffer + framePos + UART_HEX_FRM_POS_LEN, frameLen - 4);

                if(crc1 == crc2)
                {
                    *pframePos = framePos;
                    *pFrameLen = frameLen;
                    return TRUE;
                }
            }
        }
        framePos++;
    }

    return FALSE;
}
*/