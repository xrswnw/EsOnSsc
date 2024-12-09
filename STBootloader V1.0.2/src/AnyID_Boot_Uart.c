#include "AnyID_Boot_Uart.h"

UART_RCVFRAME g_sUartRcvFrame = {0};
UART_TXFRAME g_sUartTxFrame = {0};

#define POLYNOMIAL                      0x8408   //x^16 + x^12 + x^5 + 1
#define PRESET_VALUE                    0xFFFF
#define ISO15693_CRC_RESULT             0x0F47
u16 Uart_CalCrc(u8 *pFrame, u16 len)
{
    u16 crc = 0;
    u16 i = 0, j = 0;
    crc = PRESET_VALUE;
    for(i = 0; i< len; i++)
    {
        crc = crc ^ pFrame[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x0001)
            {
                crc = (crc >> 1) ^ POLYNOMIAL;
            }
            else
            {
                crc = (crc >> 1);

            }
        }
    }
    crc = ~crc;
    return crc;
}

u32 Uart_RspFrame(u8 *pBuffer, u8 cmd, u8 *pParams, u8 paramsLen, u8 flag, u8 err, u16 addr)
{
    u32 pos = 0;
    u16 crc = 0;

    pBuffer[pos++] = 0x7E; // frame head first byte
    pBuffer[pos++] = 0x55; // frame haed second byte
    pBuffer[pos++] = 9 + paramsLen + 2;   // command length
    pBuffer[pos++] = (addr >> 0) & 0xFF;
    pBuffer[pos++] = (addr >> 8) & 0xFF;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = 0x00;
    pBuffer[pos++] = UART_FRAME_RESPONSE_FLAG;
    pBuffer[pos++] = cmd;   // computer command serial number
    pBuffer[pos++] = 0x00;       // RFU
    
    if(flag == 0)
    {
        memcpy(pBuffer + pos, pParams, paramsLen);
    }
    else
    {
        memset(pBuffer + pos, 0, paramsLen);
    }
    pos += paramsLen;    
    pBuffer[pos++] = flag;
    pBuffer[pos++] = err;

    crc = Uart_CalCrc(pBuffer + 2, pBuffer[2] - 1);
    pBuffer[pos++] = crc & 0xFF;
    pBuffer[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}


u32 Uart_RspBoot(u16 addr)
{
    u32 pos = 0;
    u16 crc = 0;

    g_sUartTxFrame.frame[pos++] = 0x7E; // frame head first byte
    g_sUartTxFrame.frame[pos++] = 0x55; // frame haed second byte
    g_sUartTxFrame.frame[pos++] = 9;   // command length
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_RESPONSE_FLAG;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_CMD_BOOT;   // computer command serial number
    g_sUartTxFrame.frame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(g_sUartTxFrame.frame + 2, g_sUartTxFrame.frame[2] - 1);
    g_sUartTxFrame.frame[pos++] = crc & 0xFF;
    g_sUartTxFrame.frame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}
u32 Uart_RspJmp(u16 addr)
{
    u32 pos = 0;
    u16 crc = 0;

    g_sUartTxFrame.frame[pos++] = 0x7E; // frame head first byte
    g_sUartTxFrame.frame[pos++] = 0x55; // frame haed second byte
    g_sUartTxFrame.frame[pos++] = 9;   // command length
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_RESPONSE_FLAG;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_CMD_JMP;   // computer command serial number
    g_sUartTxFrame.frame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(g_sUartTxFrame.frame + 2, g_sUartTxFrame.frame[2] - 1);
    g_sUartTxFrame.frame[pos++] = crc & 0xFF;
    g_sUartTxFrame.frame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_RspReset(u16 addr)
{
    u32 pos = 0;
    u16 crc = 0;

    g_sUartTxFrame.frame[pos++] = 0x7E; // frame head first byte
    g_sUartTxFrame.frame[pos++] = 0x55; // frame haed second byte
    g_sUartTxFrame.frame[pos++] = 9;   // command length
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_RESPONSE_FLAG;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_CMD_RESET;   // computer command serial number
    g_sUartTxFrame.frame[pos++] = 0x00;       // RFU

    crc = Uart_CalCrc(g_sUartTxFrame.frame + 2, g_sUartTxFrame.frame[2] - 1);
    g_sUartTxFrame.frame[pos++] = crc & 0xFF;
    g_sUartTxFrame.frame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_RspDownload(BOOL bCheck, u32 addr, u32 size, u16 add)
{
    u32 pos = 0;
    u16 crc = 0;

    g_sUartTxFrame.frame[pos++] = 0x7E; // frame head first byte
    g_sUartTxFrame.frame[pos++] = 0x55; // frame haed second byte
    g_sUartTxFrame.frame[pos++] = 0;   // command length
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_RESPONSE_FLAG;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_CMD_DL;   // computer command serial number
    g_sUartTxFrame.frame[pos++] = 0x00;       // RFU

    g_sUartTxFrame.frame[pos++] = bCheck;
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 16) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 24) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (size >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (size >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (size >> 16) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (size >> 24) & 0xFF;
    if(bCheck)
    {
        Uart_ReadImagePage(addr, g_sUartTxFrame.frame + pos, size);
        pos += size;
    }


    crc = Uart_CalCrc(g_sUartTxFrame.frame + 2, pos - 2);
    g_sUartTxFrame.frame[pos++] = crc & 0xFF;
    g_sUartTxFrame.frame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

u32 Uart_RspErase(BOOL b, u16 addr)
{
    u32 pos = 0;
    u16 crc = 0;

    g_sUartTxFrame.frame[pos++] = 0x7E; // frame head first byte
    g_sUartTxFrame.frame[pos++] = 0x55; // frame haed second byte
    g_sUartTxFrame.frame[pos++] = 10;   // command length
    g_sUartTxFrame.frame[pos++] = (addr >> 0) & 0xFF;
    g_sUartTxFrame.frame[pos++] = (addr >> 8) & 0xFF;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = 0x00;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_RESPONSE_FLAG;
    g_sUartTxFrame.frame[pos++] = UART_FRAME_CMD_ERASE;   // computer command serial number
    g_sUartTxFrame.frame[pos++] = 0x00;       // RFU

    g_sUartTxFrame.frame[pos++] = b;

    crc = Uart_CalCrc(g_sUartTxFrame.frame + 2, g_sUartTxFrame.frame[2] - 1);
    g_sUartTxFrame.frame[pos++] = crc & 0xFF;
    g_sUartTxFrame.frame[pos++] = (crc >> 8) & 0xfFF;

    return pos;
}

BOOL Uart_EraseFlash(u8 sector)
{
    FLASH_Status flashStatus = FLASH_BUSY;

    if(!IS_FLASH_SECTOR(sector))
    {
        return FALSE;
    }

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

    flashStatus = FLASH_EraseSector(sector, VoltageRange_3);
    if(flashStatus != FLASH_COMPLETE)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


BOOL Uart_WriteImagePage(u32 addr, u8 *pBuffer, u32 size)
{
    u32 i = 0;
    u32 *p = NULL;
    FLASH_Status flashStatus = FLASH_BUSY;
    BOOL b = TRUE;

    p = (u32 *)pBuffer;

    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

    //如果发生错误就延时等待，并继续写数据，直到写数据成功
    for(i = 0; i < size; i += 4)
    {
        flashStatus = FLASH_ProgramWord(addr + i, p[i >> 2]);
        if(flashStatus != FLASH_COMPLETE)
        {
            b = FALSE;
            break;
        }
    }

    //FLASH_Lock();

    return b;
}

BOOL Uart_ReadImagePage(u32 addr, u8 *pBuffer, u32 size)
{
    u32 i = 0;
    u32 *p1 = NULL;
    u32 *p2 = NULL;
    BOOL b = TRUE;

    p1 = (u32 *)addr;
    p2 = (u32 *)pBuffer;

    FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

    if(size & 0x03)
    {
        size += 4;
    }

    for(i = 0; i < (size >> 2); i++)
    {
        *(p2 + i) = *(p1 + i);
    }

    return b;
}



