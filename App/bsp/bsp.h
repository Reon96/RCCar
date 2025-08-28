
#ifndef BSP_BSP_H_
#define BSP_BSP_H_

#include "def.h"
#include "stm32f4xx_hal.h"

void GPIOInit(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint32_t pull);
uint32_t Get_TIM_IT_CC(uint32_t channel);
uint32_t Get_Active_Channel(uint32_t channel);

#endif /* BSP_BSP_H_ */
