#include "fakes/stm32_hw/main.h"
#include "fakes/stm32_hw/tim.h"
#include "fakes/stm32_hw/ecatappl.h"

GPIO_TypeDef Test_GPIOC;
void *Test_TIM7_Instance = (void *)UINT32_C(7);
TIM_HandleTypeDef htim7;

UINT32 __get_PRIMASK(void)
{
    return 0U;
}

void __disable_irq(void)
{
}

void __enable_irq(void)
{
}

void __set_PRIMASK(UINT32 state)
{
    (void)state;
}

void HAL_GPIO_WritePin(
    GPIO_TypeDef *port,
    UINT16 pin,
    GPIO_PinState state)
{
    (void)port;
    (void)pin;
    (void)state;
}

void HAL_NVIC_ClearPendingIRQ(IRQn_Type interrupt)
{
    (void)interrupt;
}

void HAL_NVIC_EnableIRQ(IRQn_Type interrupt)
{
    (void)interrupt;
}

void Test_HAL_GPIO_EXTI_Clear(UINT16 pin)
{
    (void)pin;
}

void Test_HAL_TIM_SetCounter(TIM_HandleTypeDef *timer, UINT32 value)
{
    (void)timer;
    (void)value;
}

void Test_HAL_TIM_ClearFlag(TIM_HandleTypeDef *timer, UINT32 flag)
{
    (void)timer;
    (void)flag;
}

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *timer)
{
    (void)timer;
    return HAL_OK;
}

void ECAT_CheckTimer(void)
{
}

void PDI_Isr(void)
{
}

void Sync0_Isr(void)
{
}

void Sync1_Isr(void)
{
}

#include "../EtherCAT/SPIDriver/STM32F407/STM32HW.c"
