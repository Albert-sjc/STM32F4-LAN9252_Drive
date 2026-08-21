#include <assert.h>
#include "fakes/stm32_spi_driver_test_shim.h"

SPI_HandleTypeDef hspi3;
static HAL_StatusTypeDef s_transfer_status = HAL_OK;
static UINT8 s_received_data = UINT8_C(0xA5);
static uint32_t s_last_timeout;
static uint32_t s_transfer_count;
static uint32_t s_cs_high_count;

HAL_StatusTypeDef Test_HAL_SPI_TransmitReceive(
    SPI_HandleTypeDef *hspi,
    UINT8 *transmitData,
    UINT8 *receiveData,
    uint16_t size,
    uint32_t timeout)
{
    (void)hspi;
    (void)transmitData;
    assert(size == 1U);
    *receiveData = s_received_data;
    s_last_timeout = timeout;
    s_transfer_count++;
    return s_transfer_status;
}

HAL_SPI_StateTypeDef Test_HAL_SPI_GetState(const SPI_HandleTypeDef *hspi)
{
    (void)hspi;
    return HAL_SPI_STATE_RESET;
}

void MX_SPI3_Init(void)
{
}

void Test_CSHigh(void)
{
    s_cs_high_count++;
}

#include "../EtherCAT/SPIDriver/STM32F407/STM32SPIDriver.c"

int main(void)
{
    STM32_SPI_ClearError();
    s_transfer_status = HAL_OK;
    SPIWrite(UINT8_C(0x5A));
    assert(STM32_SPI_HasError() == 0U);
    assert(s_transfer_count == 1U);
    assert(s_last_timeout != HAL_MAX_DELAY);

    assert(SPIRead() == UINT8_C(0xA5));
    assert(STM32_SPI_HasError() == 0U);

    s_transfer_status = HAL_ERROR;
    SPIWrite(UINT8_C(0x11));
    assert(STM32_SPI_HasError() != 0U);
    assert(s_cs_high_count == 1U);

    s_transfer_status = HAL_OK;
    s_transfer_count = 0U;
    SPIWrite(UINT8_C(0x22));
    assert(s_transfer_count == 0U);

    STM32_SPI_ClearError();
    SPIWrite(UINT8_C(0x33));
    assert(s_transfer_count == 1U);
    assert(STM32_SPI_HasError() == 0U);

    return 0;
}
