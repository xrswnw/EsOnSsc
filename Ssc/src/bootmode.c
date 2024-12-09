/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
\addtogroup ESM EtherCAT State Machine
@{
*/

/**
\file bootmode.c
\author EthercatSSC@beckhoff.com
\brief Implementation

\version 5.12

<br>Changes to version V4.20:<br>
V5.12 BOOT2: call BL_Start() from Init to Boot<br>
<br>Changes to version - :<br>
V4.20: File created
*/

/*--------------------------------------------------------------------------------------
------
------    Includes
------
--------------------------------------------------------------------------------------*/
#include "ecat_def.h"


#define    _BOOTMODE_ 1
#include "bootmode.h"
#undef _BOOTMODE_

#include "ecatfoe.h"

#include "Application.h"


extern void SoftReset(void);
/*ECATCHANGE_START(V5.12) BOOT2*/
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    State        Current state

 \brief Called from INIT to BOOT
*////////////////////////////////////////////////////////////////////////////////////////
void BL_Start( UINT8 State)
{
}
/*ECATCHANGE_END(V5.12) BOOT2*/

/////////////////////////////////////////////////////////////////////////////////////////
/**

\brief Called in the state transition from BOOT to Init
*////////////////////////////////////////////////////////////////////////////////////////
void BL_Stop(void)
{
	u32 FirmwareCnt = 0;
	//u32 NewFlag = 1;
	FirmwareCnt = FirmEndFlashAddr - FirmStartFlashAddr;
	if(FirmwareCnt !=0 )  //如果没有接收到固件，则只复位不进行其他操作
	{
		//iap_write_appbin (NewFirmFlagAddr, (u8*)&NewFlag, 4); //更新FLASH代码
		//iap_write_appbin (NewFirmCntAddr, (u8*)&FirmwareCnt, 4); //更新FLASH代码
	}
//	SoftReset();
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    password    download password

 \brief Dummy BL_StartDownload function
*////////////////////////////////////////////////////////////////////////////////////////
void BL_StartDownload(UINT32 password)
{
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pData    Data pointer
 \param    Size    Data Length


 \return    FoE error code

 \brief Dummy BL_Data function
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 BL_Data(UINT16 *pData,UINT16 Size)
{
	//iap_write_appbin (FirmEndFlashAddr, (u8*)pData, Size); //更新FLASH代码
	//FirmEndFlashAddr = FirmEndFlashAddr + Size;
    return 0;
}
/** @} */
