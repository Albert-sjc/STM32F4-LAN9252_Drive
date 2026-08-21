#include "fakes/stm32_spi_driver_test_shim.h"

SPI_HandleTypeDef hspi3;

HAL_StatusTypeDef Test_HAL_SPI_TransmitReceive(
    SPI_HandleTypeDef *hspi,
    UINT8 *transmitData,
    UINT8 *receiveData,
    uint16_t size,
    uint32_t timeout)
{
    (void)hspi;
    (void)transmitData;
    (void)receiveData;
    (void)size;
    (void)timeout;
    return HAL_OK;
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
}

#include "../EtherCAT/SPIDriver/STM32F407/STM32SPIDriver.c"
