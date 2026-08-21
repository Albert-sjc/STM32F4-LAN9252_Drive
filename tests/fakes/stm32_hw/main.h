#ifndef TEST_STM32_HW_MAIN_H
#define TEST_STM32_HW_MAIN_H

#include <stdint.h>

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;

typedef struct
{
    int unused;
} GPIO_TypeDef;

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
} GPIO_PinState;

typedef int32_t IRQn_Type;

extern GPIO_TypeDef Test_GPIOC;

#define GPIOC (&Test_GPIOC)
#define SYS_RUN_GPIO_Port GPIOC
#define SYS_RUN_Pin UINT16_C(0x0004)
#define SYS_ERR_GPIO_Port GPIOC
#define SYS_ERR_Pin UINT16_C(0x0008)
#define L9252_ISR_Pin UINT16_C(0x0008)
#define L9252_SYN0_Pin UINT16_C(0x0010)
#define L9252_SYN1_Pin UINT16_C(0x0080)
#define L9252_ISR_EXTI_IRQn ((IRQn_Type)9)
#define L9252_SYN0_EXTI_IRQn ((IRQn_Type)10)
#define L9252_SYN1_EXTI_IRQn ((IRQn_Type)23)

UINT32 __get_PRIMASK(void);
void __disable_irq(void);
void __enable_irq(void);
void __set_PRIMASK(UINT32 state);
void HAL_GPIO_WritePin(
    GPIO_TypeDef *port,
    UINT16 pin,
    GPIO_PinState state);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type interrupt);
void HAL_NVIC_EnableIRQ(IRQn_Type interrupt);
void Test_HAL_GPIO_EXTI_Clear(UINT16 pin);

#define __HAL_GPIO_EXTI_CLEAR_IT(pin) Test_HAL_GPIO_EXTI_Clear(pin)

#endif
