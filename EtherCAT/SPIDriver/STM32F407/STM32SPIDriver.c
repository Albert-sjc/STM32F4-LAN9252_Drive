/*******************************************************************************
 STM32F407 SPI Interface Driver for LAN9252

  Company:
    Microchip Technology Inc.

  File Name:
    STM32SPIDriver.c

  Summary:
    使用 STM32 HAL SPI3 实现 LAN9252 的单字节 SPI 访问接口。

  Description:
	LAN9252 的命令、地址和数据由上层 SPIDriver.c 按字节组织，本文件只负责
	完成一个字节的全双工传输。片选信号由上层在完整事务开始和结束时控制，
	因此 SPIRead()/SPIWrite() 内部不改变 CS 电平。
	
  Change History:
    Version		Changes
	1.3			Initial version.
*******************************************************************************/

/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
 

#include "STM32SPIDriver.h"
#include "9252_HW.h"

/*******************************************************************************
 函数：
    void SPIWrite(UINT8 data)

 说明：
    通过 SPI3 向 LAN9252 发送一个字节。

    STM32 SPI 主机在发送数据的同时也会接收一个字节。写操作不需要返回接收值，
    但 HAL_SPI_TransmitReceive() 仍要求提供接收缓冲区，因此使用 receivedData
    接收并丢弃该字节。HAL_MAX_DELAY 表示阻塞等待本次单字节传输完成，确保
    上层可以紧接着发送下一个命令、地址或数据字节。
*******************************************************************************/
void SPIWrite(UINT8 data)
{
    UINT8 receivedData = 0U;

    (void)HAL_SPI_TransmitReceive(
        &hspi3,
        &data,
        &receivedData,
        1U,
        HAL_MAX_DELAY);
}

/*******************************************************************************
 函数：
    UINT8 SPIRead(void)

 说明：
    通过 SPI3 从 LAN9252 读取一个字节。

    SPI 是全双工总线，主机只有产生时钟才能接收数据。因此读取时必须同时发送
    一个 Dummy Byte（此处为 0x00），LAN9252 在相同的 8 个时钟周期内返回有效
    数据。片选在调用前已由上层拉低，并在完整事务结束后由上层拉高。
*******************************************************************************/
UINT8 SPIRead(void)
{
    UINT8 transmitData = 0U;
    UINT8 receivedData = 0U;

    (void)HAL_SPI_TransmitReceive(
        &hspi3,
        &transmitData,
        &receivedData,
        1U,
        HAL_MAX_DELAY);

    return receivedData;
}

/*******************************************************************************
 函数：
    void SPIOpen(void)

 说明：
    确保 SPI3 已初始化，并把 LAN9252 片选置为非选中状态。

    正常情况下 MX_SPI3_Init() 已由 main() 调用。为了让驱动也能在 SPI3 尚处于
    HAL_SPI_STATE_RESET 时独立初始化，这里仅在 RESET 状态下再次调用初始化
    函数，避免重复配置正在工作的外设。最后将 CS 拉高，保证 LAN9252 不会把
    初始化期间的时钟或电平变化解释成一笔 SPI 事务。
*******************************************************************************/
void SPIOpen(void)
{
    if (HAL_SPI_GetState(&hspi3) == HAL_SPI_STATE_RESET)
    {
        MX_SPI3_Init();
    }

    CSHIGH();
}

