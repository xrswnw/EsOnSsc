#include "AnyID_FRam_App.h"

BOOL FRam_ReadBuffer(u16 addr, u16 len, u8 *pBuffer)
{
    BOOL b = TRUE;
    u16 i = 0;

    if(len)
    {
        //��ʼ
        if(!FRam_I2CStart())
        {
            b = FALSE;
        }


        //�豸��ַ�����ݸߵ�ַ
        if(b)
        {
            FRam_I2CSendByte(FRAM_ADDR_WRITE | ((addr >> 7) & FRAM_ADDR_MASK));
            if(!FRam_I2CWaitAck())
            {
                b = FALSE;
            }
        }

        //���ݵ͵�ַ
        if(b)
        {
            FRam_I2CSendByte((addr >> 0) & 0xFF);
            b = FRam_I2CWaitAck();
        }


        //��ʼ
    	if(b)
    	{
    		if(!FRam_I2CStart())
    		{
    			b = FALSE;
            }
    	}

        //�豸��ַ
        if(b)
        {
            FRam_I2CSendByte(FRAM_ADDR_READ | ((addr >> 7) & FRAM_ADDR_MASK));
            if(!FRam_I2CWaitAck())
            {
                b = FALSE;
            }
        }

        //�����������ݣ����һ������Ҫ��NOACK��β
    	len--;		
    	if(b)
    	{
    		for(i = 0; i < len; i++)
    		{
    			pBuffer[i] = FRam_I2CReceiveByte();
    			FRam_I2CAck();
    		}
    		
    		pBuffer[i] = FRam_I2CReceiveByte();
    		FRam_I2CNoAck();
    	}
    	
        FRam_I2CStop();
    }

    return b;
}

BOOL FRam_WriteBuffer(u16 addr, u16 len, u8 *pBuffer)
{
    u16 i = 0;
    BOOL b = TRUE;

    if(len)
    {
        //��ʼ
        if(!FRam_I2CStart())
        {
            b = FALSE;
        }

        //�豸��ַ�����ݸߵ�ַ
        if(b)
        {
            FRam_I2CSendByte(FRAM_ADDR_WRITE | ((addr >> 7) & FRAM_ADDR_MASK));
            if(!FRam_I2CWaitAck())
            {
                b = FALSE;
            }
        }

        //���ݵ͵�ַ
        if(b)
        {
            FRam_I2CSendByte((addr >> 0) & 0xFF);
            b = FRam_I2CWaitAck();
        }

        //д������
        for(i = 0; i < len; i++)
        {
            if(b)
            {
                FRam_I2CSendByte(pBuffer[i]);
                b = FRam_I2CWaitAck();
            }
        }

        //ֹͣ
        FRam_I2CStop();
    }
    return b;
}

