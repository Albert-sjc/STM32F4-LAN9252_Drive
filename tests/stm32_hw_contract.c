#include <assert.h>
#include "fakes/stm32_hw/main.h"
#include "fakes/stm32_hw/tim.h"
#include "fakes/stm32_hw/ecatappl.h"

#define STM32SPIDRIVER_H
#define LAN9252_PLATFORM_OK                    0U
#define LAN9252_PLATFORM_SPI_ERROR             1U
#define LAN9252_PLATFORM_BYTE_TEST_TIMEOUT     2U
#define LAN9252_PLATFORM_READY_TIMEOUT         3U
#define LAN9252_PLATFORM_TIMER_ERROR           4U

GPIO_TypeDef Test_GPIOC;
GPIO_TypeDef Test_GPIOD;
void *Test_TIM7_Instance = (void *)UINT32_C(7);
TIM_HandleTypeDef htim7;

static UINT32 s_tick;
static UINT32 s_byte_test_value;
static UINT32 s_hw_cfg_value;
static UINT32 s_reset_low_count;
static UINT32 s_reset_high_count;
static UINT32 s_delay_total_ms;
static UINT32 s_irq_enable_count;
static UINT32 s_irq_disable_count;
static UINT32 s_timer_start_count;
static UINT32 s_timer_stop_count;
static UINT32 s_pdi_isr_count;
static HAL_StatusTypeDef s_timer_start_status = HAL_OK;
static UINT8 s_spi_error;
static UINT8 s_fail_spi_on_read;
static GPIO_PinState s_pdi_irq_state = GPIO_PIN_SET;

#define TEST_BYTE_TEST_ADDRESS UINT16_C(0x0064)
#define TEST_HW_CFG_ADDRESS UINT16_C(0x0074)
#define TEST_BYTE_TEST_VALUE UINT32_C(0x87654321)
#define TEST_HW_CFG_READY_MASK UINT32_C(0x08000000)

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
    if ((port == L9252_RST_GPIO_Port) && (pin == L9252_RST_Pin))
    {
        if (state == GPIO_PIN_RESET)
        {
            s_reset_low_count++;
        }
        else
        {
            s_reset_high_count++;
        }
    }
}

void HAL_NVIC_ClearPendingIRQ(IRQn_Type interrupt)
{
    (void)interrupt;
}

void HAL_NVIC_EnableIRQ(IRQn_Type interrupt)
{
    (void)interrupt;
    s_irq_enable_count++;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port, UINT16 pin)
{
    (void)port;
    (void)pin;
    return s_pdi_irq_state;
}

void HAL_NVIC_DisableIRQ(IRQn_Type interrupt)
{
    (void)interrupt;
    s_irq_disable_count++;
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
    s_timer_start_count++;
    return s_timer_start_status;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *timer)
{
    (void)timer;
    s_timer_stop_count++;
    return HAL_OK;
}

void HAL_Delay(UINT32 delay_ms)
{
    s_delay_total_ms += delay_ms;
    s_tick += delay_ms;
}

UINT32 HAL_GetTick(void)
{
    return s_tick++;
}

void ECAT_CheckTimer(void)
{
}

void PDI_Isr(void)
{
    s_pdi_isr_count++;
}

void Sync0_Isr(void)
{
}

void Sync1_Isr(void)
{
}

UINT32 PDIReadLAN9252DirectReg(UINT16 address)
{
    if (s_fail_spi_on_read != 0U)
    {
        s_spi_error = 1U;
        return 0U;
    }

    if (address == TEST_BYTE_TEST_ADDRESS)
    {
        return s_byte_test_value;
    }
    if (address == TEST_HW_CFG_ADDRESS)
    {
        return s_hw_cfg_value;
    }

    return 0U;
}

void STM32_SPI_ClearError(void)
{
    s_spi_error = 0U;
}

UINT8 STM32_SPI_HasError(void)
{
    return s_spi_error;
}

#include "../EtherCAT/SPIDriver/STM32F407/STM32HW.c"

static void reset_test_state(void)
{
    s_tick = 0U;
    s_byte_test_value = TEST_BYTE_TEST_VALUE;
    s_hw_cfg_value = TEST_HW_CFG_READY_MASK;
    s_reset_low_count = 0U;
    s_reset_high_count = 0U;
    s_delay_total_ms = 0U;
    s_irq_enable_count = 0U;
    s_irq_disable_count = 0U;
    s_timer_start_count = 0U;
    s_timer_stop_count = 0U;
    s_pdi_isr_count = 0U;
    s_timer_start_status = HAL_OK;
    s_spi_error = 0U;
    s_fail_spi_on_read = 0U;
    s_pdi_irq_state = GPIO_PIN_SET;
}

int main(void)
{
    UINT8 status;

    reset_test_state();
    status = LAN9252_PlatformInit();
    assert(status == LAN9252_PLATFORM_OK);
    assert(s_reset_low_count == 1U);
    assert(s_reset_high_count == 1U);
    assert(s_delay_total_ms >= 26U);

    reset_test_state();
    s_byte_test_value = 0U;
    status = LAN9252_PlatformInit();
    assert(status == LAN9252_PLATFORM_BYTE_TEST_TIMEOUT);

    reset_test_state();
    s_hw_cfg_value = 0U;
    status = LAN9252_PlatformInit();
    assert(status == LAN9252_PLATFORM_READY_TIMEOUT);

    reset_test_state();
    s_fail_spi_on_read = 1U;
    status = LAN9252_PlatformInit();
    assert(status == LAN9252_PLATFORM_SPI_ERROR);

    reset_test_state();
    PDI_Timer_Interrupt();
    PDI_IRQ_Interrupt();
    PDI_Init_SYNC_Interrupts();
    assert(s_timer_start_count == 0U);
    assert(s_irq_enable_count == 0U);
    assert(s_irq_disable_count >= 3U);

    status = LAN9252_PlatformStart();
    assert(status == LAN9252_PLATFORM_OK);
    assert(s_timer_start_count == 1U);
    assert(s_irq_enable_count == 4U);

    LAN9252_PlatformStop();
    s_pdi_irq_state = GPIO_PIN_RESET;
    status = LAN9252_PlatformStart();
    assert(status == LAN9252_PLATFORM_OK);
    assert(s_pdi_isr_count == 1U);

    LAN9252_PlatformStop();
    assert(s_timer_stop_count == 2U);

    reset_test_state();
    s_timer_start_status = HAL_ERROR;
    status = LAN9252_PlatformStart();
    assert(status == LAN9252_PLATFORM_TIMER_ERROR);
    assert(s_irq_enable_count == 0U);

    return 0;
}
