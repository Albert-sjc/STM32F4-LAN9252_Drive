/*******************************************************************************
 STM32F407 Hardware Abstraction Layer for LAN9252

  本文件负责把 LAN9252 EtherCAT SSC（Slave Stack Code）要求的硬件接口
  映射到 STM32F407 HAL：
  1. 使用 Cortex-M4 PRIMASK 实现临界区的全局中断保护；
  2. 使用 TIM7 产生 1 ms 周期中断，作为 SSC 软件定时基准；
  3. 将 LAN9252 IRQ、SYNC0 和 SYNC1 信号转交给 SSC 中断服务函数；
  4. 控制连接在 PC2/PC3 上的低电平有效 RUN/ERR 状态指示灯。

  TIM、GPIO 和 EXTI 的底层初始化由 STM32CubeMX 生成，本文件只负责启动、
  分发及协议栈接口适配，避免在平台层重复配置外设。
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

#include "main.h"
#include "tim.h"
#include "9252_HW.h"
#include "ecatappl.h"

/*
 * EtherCAT 软件定时计数器，单位为毫秒。
 *
 * TIM7 每产生一次更新中断就加 1。该变量同时在中断上下文中写入、在主循环
 * 中读取，因此必须声明为 volatile，防止编译器缓存其值。UINT16 溢出后会按
 * 无符号整数规则自然回绕，SSC 通过计数差值判断超时，不要求计数器永久递增。
 */
static volatile UINT16 ecatTimerMs;

/*******************************************************************************
 函数：
    void PDI_Restore_Global_Interrupt(UINT32 interruptState)

 说明：
    恢复进入临界区之前保存的 PRIMASK。直接写回原值可以正确处理嵌套调用：
    如果调用前中断本来就是关闭的，退出本层临界区后仍保持关闭，不能简单
    调用 __enable_irq() 替代该操作。
*******************************************************************************/
void PDI_Restore_Global_Interrupt(UINT32 interruptState)
{
    __set_PRIMASK(interruptState);
}

/*******************************************************************************
 函数：
    void PDI_Enable_Global_interrupt(void)

 说明：
    无条件清除 PRIMASK.I 位，允许所有未被单独屏蔽的可屏蔽中断响应。
*******************************************************************************/
void PDI_Enable_Global_interrupt(void)
{
    __enable_irq();
}

/*******************************************************************************
 函数：
    UINT32 PDI_Disable_Global_Interrupt(void)

 说明：
    进入 SSC 临界区。必须先读取 PRIMASK 再关闭全局中断，并把中断原始状态
    返回给调用者。退出临界区时，应将返回值原样传给
    PDI_Restore_Global_Interrupt()，从而正确恢复调用前的中断状态。
*******************************************************************************/
UINT32 PDI_Disable_Global_Interrupt(void)
{
    UINT32 interruptState = __get_PRIMASK();

    __disable_irq();
    return interruptState;
}

/*******************************************************************************
 函数：
    UINT16 PDI_GetTimer(void)

 说明：
    返回 EtherCAT 软件定时器从最近一次清零开始累计的毫秒数。
*******************************************************************************/
UINT16 PDI_GetTimer(void)
{
    return ecatTimerMs;
}

/*******************************************************************************
 函数：
    void PDI_ClearTimer(void)

 说明：
    清除 SSC 使用的软件毫秒计数器。此函数不会停止 TIM7，也不会修改 TIM7
    的硬件计数值，后续 1 ms 定时中断仍会正常产生。
*******************************************************************************/
void PDI_ClearTimer(void)
{
    ecatTimerMs = 0U;
}

/*******************************************************************************
 函数：
    void PDI_Timer_Interrupt(void)

 说明：
    初始化并启动 EtherCAT 的 1 ms 时间基准。TIM7 的预分频器和自动重装值
    在 CubeMX 生成的 tim.c 中配置，使更新事件周期为 1 ms。启动前清除软件
    计数、硬件计数和遗留的更新标志，避免旧 UIF 标志触发一次伪中断。
*******************************************************************************/
void PDI_Timer_Interrupt(void)
{
    ecatTimerMs = 0U;
    __HAL_TIM_SET_COUNTER(&htim7, 0U);
    __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
    (void)HAL_TIM_Base_Start_IT(&htim7);
}

/*******************************************************************************
 函数：
    void PDI_IRQ_Interrupt(void)

 说明：
    配置并启用 LAN9252 PDI IRQ 对应的 EXTI 中断。在打开 NVIC 通道之前清除
    GPIO EXTI 标志和 NVIC pending 位，防止上电或初始化阶段残留的边沿被
    误认为 LAN9252 新产生的中断。
*******************************************************************************/
void PDI_IRQ_Interrupt(void)
{
    __HAL_GPIO_EXTI_CLEAR_IT(L9252_ISR_Pin);
    HAL_NVIC_ClearPendingIRQ(L9252_ISR_EXTI_IRQn);
    HAL_NVIC_EnableIRQ(L9252_ISR_EXTI_IRQn);
}

/*******************************************************************************
 函数：
    void PDI_Init_SYNC_Interrupts(void)

 说明：
    配置并启用 LAN9252 的 SYNC0 和 SYNC1 外部中断。只有 SSC 工程启用
    DC_SUPPORTED 时才清除中断标志并开放对应的 NVIC 通道；未启用分布式
    时钟（Distributed Clocks）时，编译器会移除相关代码。
*******************************************************************************/
void PDI_Init_SYNC_Interrupts(void)
{
#if DC_SUPPORTED
    __HAL_GPIO_EXTI_CLEAR_IT(L9252_SYN0_Pin);
    __HAL_GPIO_EXTI_CLEAR_IT(L9252_SYN1_Pin);
    HAL_NVIC_ClearPendingIRQ(L9252_SYN0_EXTI_IRQn);
    HAL_NVIC_ClearPendingIRQ(L9252_SYN1_EXTI_IRQn);
    HAL_NVIC_EnableIRQ(L9252_SYN0_EXTI_IRQn);
    HAL_NVIC_EnableIRQ(L9252_SYN1_EXTI_IRQn);
#endif
}

/*******************************************************************************
 函数：
    void HW_SetLed(UINT8 RunLed, UINT8 ErrLed)

 说明：
    更新 EtherCAT RUN/ERR 状态指示灯。RUNLED 连接 PC2，ERRLED 连接 PC3，
    两颗 LED 均为低电平点亮、高电平熄灭。SSC 参数非 0 表示点亮，因此写入
    GPIO 时需要把软件状态转换为相反的输出电平。
*******************************************************************************/
void HW_SetLed(UINT8 RunLed, UINT8 ErrLed)
{
    HAL_GPIO_WritePin(
        SYS_RUN_GPIO_Port,
        SYS_RUN_Pin,
        (RunLed != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(
        SYS_ERR_GPIO_Port,
        SYS_ERR_Pin,
        (ErrLed != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/*******************************************************************************
 函数：
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)

 说明：
    STM32 HAL 的统一 GPIO 外部中断回调。此函数根据触发中断的引脚，将 LAN9252
    IRQ 分发给 PDI_Isr()，将 SYNC0/SYNC1 分别分发给 Sync0_Isr() 和
    Sync1_Isr()。其他 GPIO 的 EXTI 事件不在本模块中处理。
*******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == L9252_ISR_Pin)
    {
        PDI_Isr();
    }
#if DC_SUPPORTED
    else if (GPIO_Pin == L9252_SYN0_Pin)
    {
        Sync0_Isr();
    }
    else if (GPIO_Pin == L9252_SYN1_Pin)
    {
        Sync1_Isr();
    }
#endif
    else
    {
        /* 该回调可能被其他 GPIO 共用；非 EtherCAT 引脚不在本模块处理。 */
    }
}

/*******************************************************************************
 函数：
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)

 说明：
    STM32 HAL 的定时器周期完成回调。系统中的多个定时器可能共用此函数，因此
    必须先检查定时器实例。每次 TIM7 更新代表经过 1 ms：累加软件毫秒计数器，
    然后调用 ECAT_CheckTimer() 处理 EtherCAT 协议栈的定时和超时事件。
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM7)
    {
        ecatTimerMs++;
        ECAT_CheckTimer();
    }
}
