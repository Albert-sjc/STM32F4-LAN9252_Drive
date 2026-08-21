/*
* This source file is part of the EtherCAT Slave Stack Code licensed by Beckhoff Automation GmbH & Co KG, 33415 Verl, Germany.
* The corresponding license agreement applies. This hint shall not be removed.
*/

/**
\addtogroup SSC-Device SSC-Device
@{
*/

/**
\file SSC-Device.c
\brief Implementation

\version 1.0.0.11
*/


/*-----------------------------------------------------------------------------------------
------
------    Includes
------
-----------------------------------------------------------------------------------------*/
#include "ecat_def.h"

#include "applInterface.h"
#include "main.h"
#include "STM32PDO.h"

#define _SSC_DEVICE_ 1
#include "SSC-Device.h"
#undef _SSC_DEVICE_
/*--------------------------------------------------------------------------------------
------
------    local types and defines
------
--------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    local variables and constants
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    application specific functions
------
-----------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------
------
------    generic functions
------
-----------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief    The function is called when an error state was acknowledged by the master

*////////////////////////////////////////////////////////////////////////////////////////

void    APPL_AckErrorInd(UINT16 stateTrans)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from INIT to PREOP when
             all general settings were checked to start the mailbox handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
            The return code NOERROR_INWORK can be used, if the application cannot confirm
            the state transition immediately, in that case this function will be called cyclically
            until a value unequal NOERROR_INWORK is returned

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from PREEOP to INIT
             to stop the mailbox handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopMailboxHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \param    pIntMask    pointer to the AL Event Mask which will be written to the AL event Mask
                        register (0x204) when this function is succeeded. The event mask can be adapted
                        in this function
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from PREOP to SAFEOP when
           all general settings were checked to start the input handler. This function
           informs the application about the state transition, the application can refuse
           the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartInputHandler(UINT16 *pIntMask)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from SAFEOP to PREEOP
             to stop the input handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopInputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    AL Status Code (see ecatslv.h ALSTATUSCODE_....)

 \brief    The function is called in the state transition from SAFEOP to OP when
             all general settings were checked to start the output handler. This function
             informs the application about the state transition, the application can refuse
             the state transition when returning an AL Status error code.
           The return code NOERROR_INWORK can be used, if the application cannot confirm
           the state transition immediately, in that case the application need to be complete 
           the transition by calling ECAT_StateChange.
*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StartOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return     0, NOERROR_INWORK

 \brief    The function is called in the state transition from OP to SAFEOP
             to stop the output handler. This functions informs the application
             about the state transition, the application cannot refuse
             the state transition.

*////////////////////////////////////////////////////////////////////////////////////////

UINT16 APPL_StopOutputHandler(void)
{
    return ALSTATUSCODE_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\return     0(ALSTATUSCODE_NOERROR), NOERROR_INWORK
\param      pInputSize  pointer to save the input process data length
\param      pOutputSize  pointer to save the output process data length

\brief    This function calculates the process data sizes from the actual SM-PDO-Assign
            and PDO mapping
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GenerateMapping(UINT16 *pInputSize,UINT16 *pOutputSize)
{
    UINT16 result = ALSTATUSCODE_NOERROR;
    UINT16 InputSize = 0;
    UINT16 OutputSize = 0;

#if COE_SUPPORTED
    UINT16 PDOAssignEntryCnt = 0;
    OBJCONST TOBJECT OBJMEM * pPDO = NULL;
    UINT16 PDOSubindex0 = 0;
    UINT32 *pPDOEntry = NULL;
    UINT16 PDOEntryCnt = 0;
   
    /*Scan object 0x1C12 RXPDO assign*/
    for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sRxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
    {
        pPDO = OBJ_GetObjectHandle(sRxPDOassign.aEntries[PDOAssignEntryCnt]);
        if(pPDO != NULL)
        {
            PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
            for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
            {
                pPDOEntry = (UINT32 *)((UINT16 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3)/2);    //goto PDO entry
                // we increment the expected output size depending on the mapped Entry
                OutputSize += (UINT16) ((*pPDOEntry) & 0xFF);
            }
        }
        else
        {
            /*assigned PDO was not found in object dictionary. return invalid mapping*/
            OutputSize = 0;
            result = ALSTATUSCODE_INVALIDOUTPUTMAPPING;
            break;
        }
    }

    OutputSize = (OutputSize + 7) >> 3;

    if(result == 0)
    {
        /*Scan Object 0x1C13 TXPDO assign*/
        for(PDOAssignEntryCnt = 0; PDOAssignEntryCnt < sTxPDOassign.u16SubIndex0; PDOAssignEntryCnt++)
        {
            pPDO = OBJ_GetObjectHandle(sTxPDOassign.aEntries[PDOAssignEntryCnt]);
            if(pPDO != NULL)
            {
                PDOSubindex0 = *((UINT16 *)pPDO->pVarPtr);
                for(PDOEntryCnt = 0; PDOEntryCnt < PDOSubindex0; PDOEntryCnt++)
                {
                    pPDOEntry = (UINT32 *)((UINT16 *)pPDO->pVarPtr + (OBJ_GetEntryOffset((PDOEntryCnt+1),pPDO)>>3)/2);    //goto PDO entry
                    // we increment the expected output size depending on the mapped Entry
                    InputSize += (UINT16) ((*pPDOEntry) & 0xFF);
                }
            }
            else
            {
                /*assigned PDO was not found in object dictionary. return invalid mapping*/
                InputSize = 0;
                result = ALSTATUSCODE_INVALIDINPUTMAPPING;
                break;
            }
        }
    }
    InputSize = (InputSize + 7) >> 3;

#else
#if _WIN32
   #pragma message ("Warning: Define 'InputSize' and 'OutputSize'.")
#else
    #warning "Define 'InputSize' and 'OutputSize'."
#endif
#endif

    *pInputSize = InputSize;
    *pOutputSize = OutputSize;
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to input process data

\brief      This function will copies the inputs from the local memory to the ESC memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_InputMapping(UINT16* pData)
{
    UINT8 *pInputData = (UINT8 *)pData;

    /*
     * Object 0x1A00 maps 0x6000:01-08 to one TxPDO byte.
     * DI0 occupies bit 0 and DI7 occupies bit 7, matching the ESI file.
     */
    *pInputData = STM32_PDO_PACK_8_BITS(
        DigitalIn0x6000.DI0,
        DigitalIn0x6000.DI1,
        DigitalIn0x6000.DI2,
        DigitalIn0x6000.DI3,
        DigitalIn0x6000.DI4,
        DigitalIn0x6000.DI5,
        DigitalIn0x6000.DI6,
        DigitalIn0x6000.DI7);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\param      pData  pointer to output process data

\brief    This function will copies the outputs from the ESC memory to the local memory
            to the hardware
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_OutputMapping(UINT16* pData)
{
    const UINT8 outputData = *((UINT8 *)pData);

    /* Object 0x1600 maps the received RxPDO byte to 0x7000:01-08. */
    DigitalOut0x7000.DO0 = STM32_PDO_GET_BIT(outputData, 0U);
    DigitalOut0x7000.DO1 = STM32_PDO_GET_BIT(outputData, 1U);
    DigitalOut0x7000.DO2 = STM32_PDO_GET_BIT(outputData, 2U);
    DigitalOut0x7000.DO3 = STM32_PDO_GET_BIT(outputData, 3U);
    DigitalOut0x7000.DO4 = STM32_PDO_GET_BIT(outputData, 4U);
    DigitalOut0x7000.DO5 = STM32_PDO_GET_BIT(outputData, 5U);
    DigitalOut0x7000.DO6 = STM32_PDO_GET_BIT(outputData, 6U);
    DigitalOut0x7000.DO7 = STM32_PDO_GET_BIT(outputData, 7U);
}

/////////////////////////////////////////////////////////////////////////////////////////
/**
\brief    This function will called from the synchronisation ISR 
            or from the mainloop if no synchronisation is supported
*////////////////////////////////////////////////////////////////////////////////////////
void APPL_Application(void)
{
    /*
     * PE8-PE15 use pull-up resistors. Convert the active-low key levels to
     * EtherCAT logical values where 1 means pressed.
     */
    DigitalIn0x6000.DI0 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin));
    DigitalIn0x6000.DI1 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin));
    DigitalIn0x6000.DI2 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin));
    DigitalIn0x6000.DI3 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin));
    DigitalIn0x6000.DI4 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin));
    DigitalIn0x6000.DI5 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin));
    DigitalIn0x6000.DI6 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin));
    DigitalIn0x6000.DI7 = STM32_ACTIVE_LOW_TO_LOGICAL(
        HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin));

    /* PE0-PE7 LEDs are active-high: PDO value 1 turns the LED on. */
    HAL_GPIO_WritePin(
        LED0_GPIO_Port,
        LED0_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO0)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED1_GPIO_Port,
        LED1_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO1)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED2_GPIO_Port,
        LED2_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO2)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED3_GPIO_Port,
        LED3_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO3)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED4_GPIO_Port,
        LED4_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO4)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED5_GPIO_Port,
        LED5_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO5)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED6_GPIO_Port,
        LED6_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO6)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(
        LED7_GPIO_Port,
        LED7_Pin,
        STM32_ACTIVE_HIGH_FROM_LOGICAL(DigitalOut0x7000.DO7)
            ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

#if EXPLICIT_DEVICE_ID
/////////////////////////////////////////////////////////////////////////////////////////
/**
 \return    The Explicit Device ID of the EtherCAT slave

 \brief     Calculate the Explicit Device ID
*////////////////////////////////////////////////////////////////////////////////////////
UINT16 APPL_GetDeviceID()
{
#if _WIN32
   #pragma message ("Warning: Implement explicit Device ID latching")
#else
    #warning "Implement explicit Device ID latching"
#endif
    /* Explicit Device 5 is expected by Explicit Device ID conformance tests*/
    return 0x5;
}
#endif



#if USE_DEFAULT_MAIN
/////////////////////////////////////////////////////////////////////////////////////////
/**

 \brief    This is the main function

*////////////////////////////////////////////////////////////////////////////////////////
#if _PIC24
int main(void)
#else
void main(void)
#endif
{
    /* initialize the Hardware and the EtherCAT Slave Controller */
#if FC1100_HW
    if(HW_Init())
    {
        HW_Release();
        return;
    }
#else
    HW_Init();
#endif
    MainInit();

    bRunApplication = TRUE;
    do
    {
        MainLoop();
        
    } while (bRunApplication == TRUE);

    HW_Release();
#if _PIC24
    return 0;
#endif
}
#endif //#if USE_DEFAULT_MAIN
/** @} */


