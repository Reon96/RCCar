
#include <bsp.h>

void GPIOInit(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint32_t pull)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = mode;
	GPIO_InitStruct.Pull = pull;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(port, &GPIO_InitStruct);

	if(mode == GPIO_MODE_OUTPUT_OD || mode == GPIO_MODE_OUTPUT_PP)
	{
		HAL_GPIO_WritePin(port, pin, RESET);
	}
}

uint32_t Get_TIM_IT_CC(uint32_t channel) {
    switch (channel) {
        case TIM_CHANNEL_1: return TIM_IT_CC1;
        case TIM_CHANNEL_2: return TIM_IT_CC2;
        case TIM_CHANNEL_3: return TIM_IT_CC3;
        case TIM_CHANNEL_4: return TIM_IT_CC4;
        default: return 0;
    }
}

uint32_t Get_Active_Channel(uint32_t channel) {
    switch (channel) {
        case TIM_CHANNEL_1: return HAL_TIM_ACTIVE_CHANNEL_1;
        case TIM_CHANNEL_2: return HAL_TIM_ACTIVE_CHANNEL_2;
        case TIM_CHANNEL_3: return HAL_TIM_ACTIVE_CHANNEL_3;
        case TIM_CHANNEL_4: return HAL_TIM_ACTIVE_CHANNEL_4;
        default: return 0;
    }
}
