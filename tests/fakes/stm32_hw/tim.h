#ifndef TEST_STM32_HW_TIM_H
#define TEST_STM32_HW_TIM_H

#include "main.h"

typedef struct
{
    void *Instance;
} TIM_HandleTypeDef;

typedef enum
{
    HAL_OK = 0,
    HAL_ERROR = 1
} HAL_StatusTypeDef;

extern TIM_HandleTypeDef htim7;
extern void *Test_TIM7_Instance;

#define TIM7 Test_TIM7_Instance
#define TIM_FLAG_UPDATE UINT32_C(1)

void Test_HAL_TIM_SetCounter(TIM_HandleTypeDef *timer, UINT32 value);
void Test_HAL_TIM_ClearFlag(TIM_HandleTypeDef *timer, UINT32 flag);
HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *timer);

#define __HAL_TIM_SET_COUNTER(timer, value) \
    Test_HAL_TIM_SetCounter((timer), (value))
#define __HAL_TIM_CLEAR_FLAG(timer, flag) \
    Test_HAL_TIM_ClearFlag((timer), (flag))

#endif
