/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
\addtogroup CoE CAN Application Profile over EtherCAT
@{
*/

/**
\file    sdoserv.c
\author EthercatSSC@beckhoff.com
\brief Implementation
The SDO server handles all sdo and sdo information services

\version 5.12

<br>Changes to version V5.11:<br>
V5.12 COE2: do not clear the object index low byte in case of creating a SDO Abort<br>
V5.12 COE5: updates in case of MBX_16BIT_ACCESS == 0<br>
V5.12 COE6: handle get object length in case of an out of range subindex<br>
V5.12 ECAT2: big endian changes<br>
V5.12 EOE4: handle 16bit only acceess, move ethernet protocol defines and structures to application header files<br>
V5.12 MBX3: handle incomplete mailbox communication<br>
V5.12 TEST3: Send  ping request or invalid mbx data in case of access to idx 0x1009<br>
<br>Changes to version V5.10:<br>
V5.11 ECAT10: change PROTO handling to prevent compiler errors<br>
V5.11 ECAT7: add missing big endian swapping<br>
V5.11 SDO10: add new SDO abort code 0x6010004 (complete access not supported)<br>
V5.11 SDO4: "if insufficient memory is available on complete download access return ""unsupported access"""<br>
<br>Changes to version V5.01:<br>
V5.10 ESC5: Add missing swapping<br>
V5.10 MBX1: Remove multiple swapping of length filed in mailbox header<br>
V5.10 SDO3: Add new SDO Abort Code (0x06090033)<br>
V5.10 SDO4: Block SDO Info services for indices less 0x1000<br>
V5.10 SDO7: Correct mailbox length calculation on segmented or normal SDO upload response<br>
V5.10 SDO8: Fix invalid fragment calculation on SdoInfo list response<br>
V5.10 TEST5: test 0x2020.1 change limit from 10 to 16 Byte <br>
             Add test object 0x3009/0x300A (huge array and record objects)<br>
<br>Changes to version V5.0:<br>
V5.01 MBX1: Allocate always complete 16Bit memory areas<br>
V5.01 SDO2: Prevent invalid memory access in case of 8Bit mailbox memory handling<br>
V5.01 SDO3: Update mailbox data length calculation<br>
V5.01 SDO4: Update length calculation in case of an segmented SDO list response<br>
V5.01 SDO6: Update SDO response interface handling. (used if the object access function returns "ABORTIDX_WORKING" and SDO_RES_INTERFACE is active)<br>
V5.01 TEST2: Change Test 0x2020.1 (force Segmented transfer) to 16Byte<br>
<br>Changes to version V4.40:<br>
V5.0 TEST1: Add test application. see Application Note ET9300 for more details.<br>
V5.0 MBX3: Calculate MBX datagram length independent of SM size.<br>
V5.0 SDO2: SDO toggle bit don not do be cleared for segmented communication.<br>
V5.0 SDO3: Set SDO OpCode in list response.<br>
V5.0 SDO4: Update abort code for "BIG_ENDIAN_16BIT" configuration.<br>
V5.0 SDO5: Object code was calculated wrong for SDO Info service.<br>
V5.0 SDO6: Handle SDO segmented transfer if only 16Bit MBX memory access is allowed. <br>
<br>Changes to version V4.20:<br>
V4.40 SDO1: change size calculation for SDO services<br>
V4.40 MBX1: Prevent accessing odd address<br>
V4.40 COE1: Abort code is set in OBJ_GetObjectList()<br>
<br>Changes to version V4.11:<br>
V4.20 PIC24: Add EL9800_4 (PIC24) required source code<br>
V4.20 SDO 2: SDO mask value bug<br>
V4.20 SDO 1: check zero size object length<br>
<br>Changes to version V4.10:<br>
V4.11 SDO 1: fixed calculation of frame fragments during a object dictionary array list request<br>
V4.11 SDO 2-3: fixed size of entry description<br>
V4.11 SDO 4-7: add STRUCT_PACKED defines<br>
<br>Changes to version V4.08:\a<br>
V4.10 SDO 1: fixed zero size SDO comparison<br>
V4.10 SDO 2: set SdoService_CommandOffset to 0<br>
V4.10 SDO 3: fixed zero size SDO comparison<br>
V4.10 SDO 4: fixed struct_packed definition<br>
<br>Changes to version V4.07:<br>
V4.08 SDO 1: For an upload segment response the toggle bit was overwritten<br>
V4.08 SDO 2: For a segmented response the command was wrong in the response<br>
<br>Changes to version V4.06:<br>
V4.07 SDO 1: In SdoRes the command specifier was not set correctly in case of an abort<br>
V4.07 ECAT 1: The sources for SPI and MCI were merged (in ecat_def.h<br>
                   set the switch MCI_HW to 1 when using the MCI,<br>
                   set the switch SPI_HW to 1 when using the SPI<br>
<br>Changes to version V4.05:<br>
V4.06 SDO 1: The variable dataSize was used wrong in function SdoRes<br>
<br>Changes to version V4.03:<br>
V4.04 SDO 1: The SDO interface was changed in that way that a SDO response<br>
             could be sent by the application to a later time. In that case<br>
                 the functions OBJ_Read and OBJ_Write shall return the value<br>
                 ABORTIDX_WORKING. To send the SDO response the new function SDOS_SdoRes<br>
                 has to be called by the application. While waiting for the call<br>
                 of SDOS_SdoRes the SDO interface will answer to another SDO request<br>
                 with the error MBXERR_SERVICEINWORK in the mailbox protocol <br>
<br>Changes to version V3.20:<br>
V4.00 SDO 1: The size of the written data in case of a SDO Download will be<br>
             in the function OBJ_Write to be more flexible<br>
V4.00 SDO 2: The object lists will be generated in the functions OBJ_GetNoOfObjects<br>
             and OBJ_GetObjectList in objdef.c to decouple the SDO services from<br>
             the implementation of the object dictionary<br>
V4.00 SDO 3: The name of an object or entry description will only be transmitted<br>
             if it fits in the mailbox because the fragmentation is not supported in the sample code.<br>
V4.00 SDO 4: SDOs with size greater than 65535 were not handled correctly, that is fixed now
*/

/*---------------------------------------------------------------------------------------
------
------    Includes
------
---------------------------------------------------------------------------------------*/

#include "ecat_def.h"

#include "ecatslv.h"


#define _SDOSERV_ 1
#include "objdef.h"
#undef  _SDOSERV_
/*remove definition of _SDOSERV_ (#ifdef is used in objdef.h)*/





extern OBJCONST TOBJECT OBJMEM asObjDef[];
extern UINT16 OBJ_GetDesc( UINT16 index, UINT8 subindex, OBJCONST TOBJECT OBJMEM * pObjEntry, UINT16 MBXMEM * pData );
extern OBJCONST TSDOINFOENTRYDESC OBJMEM * OBJ_GetEntryDesc(OBJCONST TOBJECT OBJMEM * pObjEntry, UINT8 Subindex);
extern OBJCONST TSDOINFOOBJDESC OBJMEM * OBJ_GetObjDesc(OBJCONST TOBJECT OBJMEM * pObjEntry);

/*---------------------------------------------------------------------------------------
------
------    Modulintern variable definitions
------
---------------------------------------------------------------------------------------*/

const UINT32 MBXMEM cAbortCode[] =
{
    ABORT_NOERROR,
    ABORT_TOGGLE_BIT_NOT_CHANGED,
    ABORT_SDO_PROTOCOL_TIMEOUT,
    ABORT_COMMAND_SPECIFIER_UNKNOWN,
    ABORT_OUT_OF_MEMORY,
    ABORT_UNSUPPORTED_ACCESS,
    ABORT_WRITE_ONLY_ENTRY,
    ABORT_READ_ONLY_ENTRY,
    ABORT_OBJECT_NOT_EXISTING,
    ABORT_OBJECT_CANT_BE_PDOMAPPED,
    ABORT_MAPPED_OBJECTS_EXCEED_PDO,
    ABORT_PARAM_IS_INCOMPATIBLE,
    ABORT_INTERNAL_DEVICE_INCOMPATIBILITY,
    ABORT_HARDWARE_ERROR,
    ABORT_PARAM_LENGTH_ERROR,
    ABORT_PARAM_LENGTH_TOO_LONG,
    ABORT_PARAM_LENGTH_TOO_SHORT,
    ABORT_SUBINDEX_NOT_EXISTING,
    ABORT_VALUE_EXCEEDED,
    ABORT_VALUE_TOO_GREAT,
    ABORT_VALUE_TOO_SMALL,
    ABORT_MODULE_ID_LIST_NOT_MATCH,
    ABORT_MAX_VALUE_IS_LESS_THAN_MIN_VALUE,
    ABORT_GENERAL_ERROR,
    ABORT_DATA_CANNOT_BE_READ_OR_STORED,
    ABORT_DATA_CANNOT_BE_READ_OR_STORED_BECAUSE_OF_LOCAL_CONTROL,
    ABORT_DATA_CANNOT_BE_READ_OR_STORED_IN_THIS_STATE,
    ABORT_NO_OBJECT_DICTIONARY_IS_PRESENT,
    ABORT_ENTRY_CANT_BE_WRITTEN_SI0_NOT_0,
    ABORT_COMPLETE_ACCESS_NOT_SUPPORTED
};

UINT16 VARMEM                            nSdoInfoIndex;
OBJCONST TOBJECT OBJMEM * VARMEM        pSdoInfoObjEntry;


/*---------------------------------------------------------------------------------------
------
------    module internal function declarations
------
---------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------
------
------    Functions
------
---------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    abort                 Result of the SDO access
 \param    command		         SDO command index 
 \param    completeAccess        Indicates if complete access was requested
 \param    dataSize              Available data buffer in the response
 \param    objLength             Complete size of the object
 \param     pSdoRes                Pointer to the mailbox buffer

 \brief    This function is called when a SDO response shall be sent
*////////////////////////////////////////////////////////////////////////////////////////

void SdoRes(UINT8 abort, UINT8 command, UINT8 completeAccess, UINT16 dataSize, UINT32 objLength, TINITSDOMBX MBXMEM *pSdoRes)
{
    /* for an upload segment response the toggle bit was overwritten */
    if ((command != SDOSERVICE_UPLOADSEGMENTREQ) && (command != SDOSERVICE_DOWNLOADSEGMENTREQ))
    {
        pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET] &= 0xFF00;
    }
    if ( abort == 0 )
    {
        /* SDO-Download or SDO-Upload was successful, generate the SDO- and CoE-Header */
        pSdoRes->CoeHeader &= ~COEHEADER_COESERVICEMASK;
        pSdoRes->CoeHeader |= ((UINT16)COESERVICE_SDORESPONSE) << COEHEADER_COESERVICESHIFT;
        if ( command == SDOSERVICE_INITIATEUPLOADREQ )
        {
            // HBu 06.02.06: Complete Access Bit in the SDO-Upload-Response too */
            if ( (objLength <= 4) && (objLength > 0) )
            {
                /* Expedited Upload Response */
                pSdoRes->MbxHeader.Length             =         EXPEDITED_FRAME_SIZE;
/* ECATCHANGE_START(V5.12) ECAT2*/
                pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET]    |=     SWAPWORD(SDOHEADER_SIZEINDICATOR     |
                                                                                        SDOHEADER_TRANSFERTYPE        |
                                                                                        completeAccess |
                                                                                        ((MAX_EXPEDITED_DATA - ((UINT8)objLength)) << SDOHEADERSHIFT_DATASETSIZE) |
                                                                                        SDOSERVICE_INITIATEUPLOADRES);
/* ECATCHANGE_END(V5.12) ECAT2*/
            }
            else
            {
                /* Normal or Segmented Upload Response */
                {
                    pSdoRes->MbxHeader.Length         =         UPLOAD_NORM_RES_SIZE+((UINT16)objLength);
                }
                   ((TINITSDOUPLOADNORMRESMBX MBXMEM *) pSdoRes)->CompleteSize[0] = SWAPWORD((UINT16)objLength);
                   ((TINITSDOUPLOADNORMRESMBX MBXMEM *) pSdoRes)->CompleteSize[1] = SWAPWORD((UINT16)(objLength>>16));
/* ECATCHANGE_START(V5.12) ECAT2*/
                pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET]    |=     SWAPWORD(SDOHEADER_SIZEINDICATOR     |
                                                                                        completeAccess |
                                                                                        SDOSERVICE_INITIATEUPLOADRES);
/* ECATCHANGE_END(V5.12) ECAT2*/

            }
        }
        /* for a segmented response the command was wrong in the response */
        else if ( command == SDOSERVICE_DOWNLOADSEGMENTREQ )
        {
            /* Download segmented response */
            pSdoRes->MbxHeader.Length         = DOWNLOAD_NORM_RES_SIZE;
/* ECATCHANGE_START(V5.12) ECAT2*/
            pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET]    |= SWAPWORD(SDOSERVICE_DOWNLOADSEGMENTRES);
/* ECATCHANGE_END(V5.12) ECAT2*/
      }
        else if ( command != SDOSERVICE_UPLOADSEGMENTREQ )
        {
            /* Download response */
            pSdoRes->MbxHeader.Length         = DOWNLOAD_NORM_RES_SIZE;
/* ECATCHANGE_START(V5.12) ECAT2*/
            pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET]    |= SWAPWORD(SDOSERVICE_INITIATEDOWNLOADRES);
/* ECATCHANGE_END(V5.12) ECAT2*/
        }
    }
    else
    {
        /* generate a SDO-Abort-Request */
        pSdoRes->MbxHeader.Length         = ABORT_NORM_RES_SIZE;
        pSdoRes->CoeHeader &= ~COEHEADER_COESERVICEMASK;
        pSdoRes->CoeHeader |= ((UINT16)COESERVICE_SDOREQUEST) << COEHEADER_COESERVICESHIFT;
        /* ECATCHANGE_START(V5.12) COE2*/
/* ECATCHANGE_START(V5.12) ECAT2*/
        pSdoRes->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET]    |= SWAPWORD(SDOSERVICE_ABORTTRANSFER);
/* ECATCHANGE_END(V5.12) ECAT2*/
        /* ECATCHANGE_END(V5.12) COE2*/

        ((TABORTSDOTRANSFERREQMBX MBXMEM *) pSdoRes)->AbortCode = SWAPDWORD(cAbortCode[abort]);
    }

    // HBu 02.05.06: if the CoE-response could not be sent because the
    //               send mailbox is full it should be stored
    if (MBX_MailboxSendReq((TMBX MBXMEM *) pSdoRes, COE_SERVICE) != 0)
    {
        /* we store the CoE mailbox service to send it later (in COE_ContinueInd) when the mailbox is read */
        pCoeSendStored = (TMBX MBXMEM *) pSdoRes;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param     pSdoInd    Pointer to the received mailbox data from the master.

 \return    Indicates if an error occurred while the operation ( good = 0 ).

 \brief    This function is called when a SDO request service
            is received from the master and calls depending from
            the command the concerning function.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 SDOS_SdoInd(TINITSDOMBX MBXMEM *pSdoInd)
{
    UINT8 abort = 0;
    UINT8 sdoHeader = (pSdoInd->SdoHeader.Sdo[SDOHEADER_COMMANDOFFSET] & SDOHEADER_COMMANDMASK) >> SDOHEADER_COMMANDSHIFT;
    /* the SDO-command is in bit 5-7 of the first SDO-Byte */
    UINT8 command = (sdoHeader & SDOHEADER_COMMAND);
    /* mbxSize contains the size of the mailbox (CoE-Header (2 Bytes) + SDO-Header (8 Bytes) + SDO-Data (if the data length is greater than 4)) */
    UINT16 mbxSize = SWAPWORD(pSdoInd->MbxHeader.Length);
    UINT16 index;
    UINT8 subindex;
    OBJCONST TOBJECT OBJMEM * pObjEntry;
    /* this variable contains the information, if all entries of an object will be read (bCompleteAccess > 0) or a single entry */
    UINT8 bCompleteAccess = 0;
    UINT32 objLength = 0;
    UINT32 dataSize = 0;



    switch (command)
    {
    case SDOSERVICE_INITIATEDOWNLOADREQ:
    case SDOSERVICE_INITIATEUPLOADREQ:
        /* the variable index contains the requested index of the SDO service */
        index = pSdoInd->SdoHeader.Sdo[SDOHEADER_INDEXHIOFFSET] & SDOHEADER_INDEXHIMASK;
/* ECATCHANGE_START(V5.12) ECAT2*/
        index <<= 8;
        index += (pSdoInd->SdoHeader.Sdo[SDOHEADER_INDEXLOOFFSET] >> SDOHEADER_INDEXLOSHIFT) & SDOHEADER_INDEXLOMASK;
        /* the variable subindex contains the requested subindex of the SDO service */
        subindex = (pSdoInd->SdoHeader.Sdo[SDOHEADER_SUBINDEXOFFSET] >> SDOHEADER_SUBINDEXSHIFT) & SDOHEADER_SUBINDEXMASK;

/* ECATCHANGE_END(V5.12) ECAT2*/




        /* OBJ_GetObjectHandle checks if the requested index is defined in the object dictionary */
        pObjEntry = OBJ_GetObjectHandle( index );

        if ( pObjEntry )
        {
            /* transferType contains the information if the SDO Download Request or the SDO Upload Response
               can be an expedited service (SDO data length <= 4, that means the data is stored in the
                SDO-Header completely */
            UINT8 bTransferType = 0;
            /* pData is the pointer to the received (SDO-Download) or sent (SDO-Upload) SDO data in the mailbox */
            UINT16 MBXMEM * pData = NULL;

            {
/*ECATCHANGE_START(V5.12) COE6*/
                UINT8 maxSubindex = (pObjEntry->ObjDesc.ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;

                if (subindex > maxSubindex)
                {
                    abort = ABORTIDX_SUBINDEX_NOT_EXISTING;
                }
                else
                {
                    dataSize = objLength = OBJ_GetObjectLength(index, subindex, pObjEntry, (UINT8)(sdoHeader & SDOHEADER_COMPLETEACCESS));
                }

                if( abort == 0)
/*ECATCHANGE_END(V5.12) COE6*/
                {
                    if (command == SDOSERVICE_INITIATEUPLOADREQ)
                    {
                            /* SDO Upload */
                            if (mbxSize != EXPEDITED_FRAME_SIZE)
                            {
                                    /* a SDO Upload request has always a fixed size (2 Byte CoE-Header plus 8 Byte SDO-Header) */
                                    return MBXERR_INVALIDSIZE;
                            }
                        /* distinguish between expedited and normal upload response within the length of the response data */
                        if ((objLength <= MAX_EXPEDITED_DATA) && objLength != 0)
                        {
                                /* Expedited Upload */
                                bTransferType = 1;
                            /* pData is the pointer where the object data has to be copied for the response */
    /* ECATCHANGE_START(V5.12)*/
                            pData = (UINT16 MBXMEM *) ((TINITSDOUPLOADEXPRESMBX MBXMEM *) pSdoInd)->Data;
    /* ECATCHANGE_END(V5.12)*/
                                                    /* initialize the 4 data bytes of the SDO upload response because the requested object data
                                                       could be less than 4 */
                            pData[0] = 0;
                            pData[1] = 0;
                        }
                        else
                        {
                                /* Normal Upload */
                                /* pData is the pointer where the object data has to be copied for the response */
                                pData = ((TINITSDOUPLOADNORMRESMBX MBXMEM *) pSdoInd)->Data;
                                }
                        }
                    else
                    {
                        /* SDO-Download: store if the request is a expedited or normal request  */
                        bTransferType = sdoHeader & SDOHEADER_TRANSFERTYPE;
                    }
                 }
            }

/*ECATCHANGE_START(V5.12) COE6*/
            if ((abort == 0 ) && (command == SDOSERVICE_INITIATEDOWNLOADREQ ))
/*ECATCHANGE_END(V5.12) COE6*/
            {
                /* SDO Download */
                if ( bTransferType )
                {
                    /* Expedited Download */
                    if ( mbxSize != EXPEDITED_FRAME_SIZE )
                    {
                        /* an Expedited SDO Download request has always a fixed size (2 Byte CoE-Header plus 8 Byte SDO-Header) */
                        return MBXERR_INVALIDSIZE;
                    }
                    /* dataSize gets the real size of the downloaded object data (1,2,3 or 4) */
                    dataSize = MAX_EXPEDITED_DATA - ((sdoHeader & SDOHEADER_DATASETSIZE) >> SDOHEADERSHIFT_DATASETSIZE);
                    /* pData is the pointer to the downloaded object data */
                    pData = (UINT16 MBXMEM *) &pSdoInd[1];
                }
                else
                {
                    /* Normal Download */
                    /* downloadSize gets the real size of the downloaded data */
                    /* '&' operator was too much */

                    UINT32 downloadSize = ((UINT32)(SWAPWORD(((TINITSDODOWNLOADNORMREQMBX MBXMEM *) pSdoInd)->CompleteSize[1]))<<16) + (SWAPWORD(((TINITSDODOWNLOADNORMREQMBX MBXMEM *) pSdoInd)->CompleteSize[0]));

                    /* HBu 29.03.06: if it is a segmented download the mbxSize has to be the complete mailbox size */
                    if ( (MBX_HEADER_SIZE+EXPEDITED_FRAME_SIZE+downloadSize) > u16ReceiveMbxSize )
                    {
                        if ( mbxSize != (u16ReceiveMbxSize-MBX_HEADER_SIZE) )
                        {
                            return MBXERR_INVALIDSIZE;
                        }
                    }
                    else
                    {
                        if ( mbxSize != (EXPEDITED_FRAME_SIZE+downloadSize) )
                        {
                            /* the mbxSize and the downloadSize are not consistent (mbxSize = downloadSize + 2 byte CoE-Header + 8 byte SDO Header */
                            return MBXERR_INVALIDSIZE;
                        }
                    }

                    /* pData is the pointer to the downloaded object data */
                    pData = (UINT16 MBXMEM *) ((TINITSDODOWNLOADNORMREQMBX MBXMEM *) pSdoInd)->Data;
                    /* the received dataSize will be checked in the object specific functions called from
                       OBJ_Write (in objdef.c) */
                    dataSize = downloadSize;
                    if ( dataSize > (UINT32)(mbxSize - DOWNLOAD_NORM_REQ_SIZE) )
                    {
                        abort = ABORTIDX_PARAM_LENGTH_ERROR;
                    }
                }
            }

/*ECATCHANGE_START(V5.12) COE6*/
            if ((abort == 0) && (sdoHeader & SDOHEADER_COMPLETEACCESS ))
/*ECATCHANGE_END(V5.12) COE6*/
            {
                abort = ABORTIDX_UNSUPPORTED_ACCESS;
            }

            if ( abort == 0 )
            {
                {
                    if ( command == SDOSERVICE_INITIATEUPLOADREQ )
                    {
                        /* Expedited or Normal Upload */
                        abort = OBJ_Read( index, subindex, objLength, pObjEntry, pData, bCompleteAccess );
                    }
                    else
                    {
                        /* Expedited or Normal Download */
                        abort = OBJ_Write( index, subindex, dataSize, pObjEntry, pData, bCompleteAccess );
                    }
                } /* else if ( objLength == 0 ) */
            } /* if ( abort == 0 ) */

        } //if(pObjEntry) (Object handle found)
        else
        {
            abort = ABORTIDX_OBJECT_NOT_EXISTING;
        }
        break;

    default:
        abort = ABORTIDX_COMMAND_SPECIFIER_UNKNOWN;
        break;
    }

    {
        /*  type cast was added because of warning */
        SdoRes(abort, command, (UINT8) (sdoHeader & SDOHEADER_COMPLETEACCESS), (UINT16) dataSize, objLength, pSdoInd);
    }

    return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pSdoInfoInd      Pointer to the received mailbox data from the master.

 \return    Indicates if an error occurred while the operation ( good = 0 ).

 \brief    This function is called when a SDO-Info request service
            is received from the master and calls depending from
            the opcode the concerning function.
*////////////////////////////////////////////////////////////////////////////////////////

UINT8 SDOS_SdoInfoInd( TSDOINFORMATION MBXMEM *pSdoInfoInd )
{
    UINT8 abort = 0;
    /* the variable opCode contains the requested SDO Information type */
    UINT8 opCode = (UINT8) ((pSdoInfoInd->SdoHeader.InfoHead & INFOHEAD_OPCODE_MASK) >> INFOHEAD_OPCODE_SHIFT);
    OBJCONST TOBJECT OBJMEM * pObjEntry;
    UINT16 index;
    UINT8 flags = COE_SERVICE;

    /* it has to be checked if the mailbox protocol is correct, the sent mailbox data length has to
       great enough for the service header of the requested SDO Information type */
    if ( opCode == SDOINFOSERVICE_ENTRYDESCRIPTION_Q )
    {
        if ( pSdoInfoInd->MbxHeader.Length < SIZEOF_SDOINFOENTRYREQSTRUCT )
        {
            return MBXERR_SIZETOOSHORT;
        }
    }
    else
    {
        if ( pSdoInfoInd->MbxHeader.Length < SIZEOF_SDOINFOLISTSTRUCT )
        {
            return MBXERR_SIZETOOSHORT;
        }
    }

    switch (opCode)
    {
    case SDOINFOSERVICE_OBJDICTIONARYLIST_Q:
        /* an object list is requested, check if the list type is supported */
        if (SWAPWORD(pSdoInfoInd->SdoHeader.Data.List.ListType) <= INFO_LIST_TYPE_MAX)
        {
            UINT16 size = 0;
            /* the variable listType contains the requested listType */
            UINT8 listType = (UINT8)SWAPWORD(pSdoInfoInd->SdoHeader.Data.List.ListType);

            /* the SDO Information Header has to be stored because this function will be
               called again if the response could not be sent with one mailbox service, the
               variable nSdoInfoFragmentsLeft is 0 zero for the first call and unequal 0
               for the following calls */
            MBXMEMCPY(aSdoInfoHeader, pSdoInfoInd, SDO_INFO_HEADER_BYTE_SIZE);
            if (listType-- == 0)
            {
                /* List-Type 0: length of the lists */
                UINT8 i;

                /* the needed mailbox size for List-Type 0 response is just 24 bytes, the mailbox has always
                   to be at least 24 bytes to support the SDO Information service */
                for (i = 0; i < INFO_LIST_TYPE_MAX; i++)
                {
                    UINT16 n = OBJ_GetNoOfObjects(i);

                    /* copy the number of objects of the list type in the SDO Information response */
                    ((UINT16 MBXMEM *) &pSdoInfoInd->CoeHeader)[(SIZEOF_SDOINFOLISTSTRUCT >> 1) + i] = SWAPWORD(n);
                }

                /* size of the mailbox service response */
                size = (INFO_LIST_TYPE_MAX << 1) + SIZEOF_SDOINFOLISTSTRUCT;
            }
            else
            {
                /* object list with indexes is requested */
                UINT16 MBXMEM * pData;
                UINT16 n = 0;

                {
                    /* the first fragment of the SDO Information response has to be sent */
                    /* get the number of objects of the requested object list */
                    n = OBJ_GetNoOfObjects(listType);
                    /* we start with index 0x1000 */
                    index = 0x1000;
                    /* initialize size with the maximum size fits into one mailbox service */
                    {
                        size = u16SendMbxSize - SIZEOF_SDOINFOLISTSTRUCT - MBX_HEADER_SIZE;
                    }
                    /* initialize pData with the pointer where the fragment has to be copied */
                    pData = &((UINT16 MBXMEM *) &pSdoInfoInd->CoeHeader)[SIZEOF_SDOINFOLISTSTRUCT >> 1];
                    /*Check if List need to be send in fragments*/
                    if ((n << 1) > size)
                    {
                        abort = ABORTIDX_UNSUPPORTED_ACCESS;
                    }
                }

                /* get the next part of the requested object list */
                size = OBJ_GetObjectList(listType, &index, size, pData, &abort);

                /* store index for next fragment */
                nSdoInfoIndex = index;
                /* size contains before the instruction the size still available in the mailbox buffer
                    and shall contain the size of the mailbox response data after the next instruction */
                {
                    size = u16SendMbxSize - size - MBX_HEADER_SIZE;
                }
            }

            /* size of the mailbox response data */
            pSdoInfoInd->MbxHeader.Length = size;

            if (abort == 0)
            {
                pSdoInfoInd->SdoHeader.InfoHead &= ~INFOHEAD_OPCODE_MASK;
                pSdoInfoInd->SdoHeader.InfoHead |= (UINT16)(SDOINFOSERVICE_OBJDICTIONARYLIST_S << INFOHEAD_OPCODE_SHIFT);
                pSdoInfoInd->SdoHeader.FragmentsLeft = 0;
            }
        }
        break;

    case SDOINFOSERVICE_OBJDESCRIPTION_Q:
    case SDOINFOSERVICE_ENTRYDESCRIPTION_Q:
        /* get the requested index */
        index = SWAPWORD(pSdoInfoInd->SdoHeader.Data.Obj.Index);



        if(index < 0x1000)
        {
            /*SDO Info access is only allowed for objects >= 0x1000*/
            abort = ABORTIDX_UNSUPPORTED_ACCESS;
        }
        else
        {
            /* get the object handle of the requested index */
            pObjEntry = OBJ_GetObjectHandle( index );

            if ( pObjEntry )
            {
                /* object exists */
                UINT16 size = 0;
                
                if ( opCode == SDOINFOSERVICE_OBJDESCRIPTION_Q )
                {
                    /* object description is requested */
                    OBJTOMBXMEMCPY(&pSdoInfoInd->SdoHeader.Data.Obj.Res, OBJ_GetObjDesc(pObjEntry), SDO_INFO_OBJ_DESC_SIZE);


                    /* the mailbox should be big enough that the maximum object description
                    fits in the mailbox (the fragmentation is not done in the sample code),
                    so it will be checked only if the object description fits */
                    size = OBJ_GetDesc(index, 0, pObjEntry, NULL) + SIZEOF_SDOINFOOBJSTRUCT;

                    if ( size > (u16SendMbxSize - MBX_HEADER_SIZE) )
                    {
                        /* size of the object description does not fit in the mailbox,
                        the object description will be sent without the name */
                        size = SIZEOF_SDOINFOOBJSTRUCT;
                    }
                    else
                    {
                        /* object description fits in the mailbox, get the name of the object */
                        size = OBJ_GetDesc(index, 0, pObjEntry, ((UINT16 MBXMEM *) &(&pSdoInfoInd->SdoHeader.Data.Obj.Res)[1])) + SIZEOF_SDOINFOOBJSTRUCT;
                    }
                }
                else
                {
                    /* entry description is requested,
                    get the requested subindex */
                    UINT8 subindex = (UINT8) ((pSdoInfoInd->SdoHeader.Data.Entry.Info & ENTRY_MASK_SUBINDEX) >> ENTRY_SUBINDEX_SHIFT);

                    /* get the maximum subindex */
                    UINT8 maxSubindex = (OBJ_GetObjDesc(pObjEntry)->ObjFlags & OBJFLAGS_MAXSUBINDEXMASK) >> OBJFLAGS_MAXSUBINDEXSHIFT;

                    if ( subindex <= maxSubindex )
                    {
                        UINT16 ObjectFlags;
                        /* requested subindex is not too great */
                        /* get the entry description of the requested entry */
                        OBJTOMBXMEMCPY(&pSdoInfoInd->SdoHeader.Data.Entry.Res, OBJ_GetEntryDesc(pObjEntry, subindex), SIZEOF(TSDOINFOENTRYDESC));

                        /* the transmission of the value info is not supported yet of the sample code */
                        pSdoInfoInd->SdoHeader.Data.Entry.Info &= ~ENTRY_MASK_VALUEINFO;
                        ObjectFlags = OBJ_GetObjDesc(pObjEntry)->ObjFlags;
                        ObjectFlags = (ObjectFlags & OBJFLAGS_OBJCODEMASK) >> OBJFLAGS_OBJCODESHIFT;

                        if(((ObjectFlags == OBJCODE_ARR) || (ObjectFlags == OBJCODE_REC)) && (subindex == 0) )
                        {
                            OBJTOMBXSTRCPY( ((UINT16 MBXMEM *) &(&pSdoInfoInd->SdoHeader.Data.Entry.Res)[1]), aSubindexDesc, SIZEOF(aSubindexDesc) );
                            size = 12 + SIZEOF_SDOINFO + SIZEOF(TSDOINFOENTRY); // 12: Length of "SubIndex 000"
                        }
                        else
                        {
                            /* the mailbox should be big enough that the maximum entry description
                            fits in the mailbox (the fragmentation is not done in the sample code),
                            so it will be checked only if the entry description fits */
                            size = OBJ_GetDesc(index, subindex, pObjEntry, NULL) + SIZEOF_SDOINFO + SIZEOF(TSDOINFOENTRY);
                            if ( size > (u16SendMbxSize - MBX_HEADER_SIZE) )
                            {
                                /* size of the object description does not fit in the mailbox,
                                the object description will be sent without the name */
                                size =  SIZEOF_SDOINFO + SIZEOF(TSDOINFOENTRY);
                            }
                            else
                            {
                                /* object description fits in the mailbox, get the name of the entry */
                                size = OBJ_GetDesc(index, subindex, pObjEntry, ((UINT16 MBXMEM *) &(&pSdoInfoInd->SdoHeader.Data.Entry.Res)[1])) + SIZEOF_SDOINFO + SIZEOF(TSDOINFOENTRY);
                            }
                        }
                    }
                    else
                    {
                        abort = ABORTIDX_SUBINDEX_NOT_EXISTING;
                    }
                }

                if ( abort == 0 )
                {
                    {
                        /* for the object and entry description the sample code does not support the fragmentation,
                        the mailbox has to be big enough */
                        pSdoInfoInd->SdoHeader.FragmentsLeft = 0;
                        /* store the size of the mailbox data in the mailbox buffer */
                        pSdoInfoInd->MbxHeader.Length = size;
                        /* set the opCode of the SDO Information response */
                        pSdoInfoInd->SdoHeader.InfoHead &= ~INFOHEAD_OPCODE_MASK;
                        pSdoInfoInd->SdoHeader.InfoHead |= (UINT16)((opCode + 1) << INFOHEAD_OPCODE_SHIFT);
                    }
                }
            }
            else
            {
                abort = ABORTIDX_OBJECT_NOT_EXISTING;
            }
        }
        break;
    default:
        abort = ABORTIDX_COMMAND_SPECIFIER_UNKNOWN;
    }

    if ( abort )
    {
        /* send a SDO Information Error response */
        pSdoInfoInd->MbxHeader.Length = SIZEOF_SDOINFOERRORSTRUCT;

        pSdoInfoInd->SdoHeader.InfoHead &= ~INFOHEAD_OPCODE_MASK;
        pSdoInfoInd->SdoHeader.InfoHead |= (UINT16) ((SDOINFOSERVICE_ERROR_Q) << INFOHEAD_OPCODE_SHIFT);

        pSdoInfoInd->SdoHeader.FragmentsLeft = 0;
        pSdoInfoInd->SdoHeader.Data.Error.ErrorCode = SWAPDWORD(cAbortCode[abort]);

    }

    if (MBX_MailboxSendReq((TMBX MBXMEM *) pSdoInfoInd, flags) != 0)
    {
        /* if the mailbox response could not be sent (or stored), the response will be
           stored in the variable pCoeSendStored and will be sent automatically
            from the mailbox handler (COE_ContinueInd) when the send mailbox will be read
            the next time from the master */
        pCoeSendStored = (TMBX MBXMEM *) pSdoInfoInd;
    }

    return 0;
}

/** @} */


