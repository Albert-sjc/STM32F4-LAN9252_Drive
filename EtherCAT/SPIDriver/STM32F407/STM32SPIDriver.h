/*******************************************************************************
 STM32F407 SPI Interface Driver for LAN9252

  Company:
    Microchip Technology Inc.

  File Name:
    STM32SPIDriver.h

  Summary:
    定义 LAN9252 STM32F407 SPI 平台层的数据类型、片选控制和函数接口。

  Description:
	该文件连接通用 LAN9252 SPIDriver 与 STM32CubeMX 生成的 SPI3/GPIO HAL
	接口。上层负责组织完整的 LAN9252 SPI 帧，本层负责单字节收发和 CS 控制。
	
  Change History:
    Version		Changes
	1.3			Initial version.

*******************************************************************************/

#ifndef STM32SPIDRIVER_H
#define STM32SPIDRIVER_H

// DOM-IGNORE-BEGIN
/*******************************************************************************
 Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

 Microchip licenses to you the right to use, modify, copy and distribute
 Software only when embedded on a Microchip microcontroller or digital signal
 controller that is integrated into your product or third party product
 (pursuant to the sublicense terms in the accompanying license agreement).

 You should refer to the license agreement accompanying this Software for
 additional information regarding your rights and obligations.

 SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
 IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
 CONTRACT, NEGLiPMPCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
 OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
 CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
 SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "spi.h"
#include "SPIDriver.h"

#ifdef	__cplusplus
extern "C" {
#endif
	// *****************************************************************************
	// *****************************************************************************
	// Section: File Scope or Global Data Types
	// *****************************************************************************
	// *****************************************************************************
	
	/*
	 * LAN9252 的 SPI 片选为低电平有效：
	 * CSLOW()  开始一笔事务，CSHIGH() 结束一笔事务。
	 * 宏使用 CubeMX 生成的 GPIO 端口和引脚名称，避免在驱动内硬编码端口地址。
	 */
	#define CSLOW()      HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_RESET)
	#define CSHIGH()     HAL_GPIO_WritePin(SPI3_CS_GPIO_Port, SPI3_CS_Pin, GPIO_PIN_SET)


	

	// *****************************************************************************
	// *****************************************************************************
	// Section: File Scope Functions
	// *****************************************************************************
	// *****************************************************************************

	/* 初始化 SPI3，并确保 LAN9252 片选保持高电平。 */
	void SPIOpen(void);

	/* 从 LAN9252 读取一个字节。 */
	UINT8 SPIRead(void);

	/* 向 LAN9252 发送一个字节。 */
	void SPIWrite(UINT8 data);
      
#ifdef	__cplusplus
}
#endif

#endif	/* STM32SPIDRIVER_H */

