#ifndef STM32_SPI_DRIVER_TEST_SHIM_H
#define STM32_SPI_DRIVER_TEST_SHIM_H

#define STM32SPIDRIVER_H

#include <stdint.h>

typedef uint8_t UINT8;
typedef struct
{
    int unused;
} SPI_HandleTypeDef;

typedef enum
{
    HAL_OK = 0,
    HAL_ERROR = 1
} HAL_StatusTypeDef;

typedef enum
{
    HAL_SPI_STATE_RESET = 0,
    HAL_SPI_STATE_READY = 1
} HAL_SPI_StateTypeDef;

extern SPI_HandleTypeDef hspi3;

#define HAL_MAX_DELAY UINT32_C(0xFFFFFFFF)

HAL_StatusTypeDef Test_HAL_SPI_TransmitReceive(
    SPI_HandleTypeDef *hspi,
    UINT8 *transmitData,
    UINT8 *receiveData,
    uint16_t size,
    uint32_t timeout);

#define HAL_SPI_TransmitReceive(hspi, transmitData, receiveData, size, timeout) \
    ((void)sizeof(char[((size) == 1U) ? 1 : -1]),                        \
     (void)sizeof(char[((timeout) == HAL_MAX_DELAY) ? 1 : -1]),          \
     Test_HAL_SPI_TransmitReceive(                                       \
         (hspi), (transmitData), (receiveData), (size), (timeout)))

HAL_SPI_StateTypeDef Test_HAL_SPI_GetState(const SPI_HandleTypeDef *hspi);
#define HAL_SPI_GetState(hspi) Test_HAL_SPI_GetState(hspi)

void MX_SPI3_Init(void);
void Test_CSHigh(void);
#define CSHIGH() Test_CSHigh()

#endif
