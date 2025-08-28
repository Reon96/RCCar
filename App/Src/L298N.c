#include "L298N.h"

uint8_t L298N_Init(L298N *driver, GPIO_TypeDef *port, uint16_t *pins)
{
	driver->port = port;
	driver->pins = pins;

	if(driver == NULL)
	{
		printf("Error to Initializing of L298N");
		return 0;
	}

	for(uint8_t i = 0; i < 4; i++)
	{
		GPIOInit(port, pins[i], GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
	}

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

	return 1;
}

void L298N_Drive(L298N *driver, uint8_t num, GPIO_PinState state)
{
	if(num > 4) return;
	HAL_GPIO_WritePin(driver->port, driver->pins[num], state);
}

void Set_Speed(uint32_t channel, uint16_t speed)
{
	__HAL_TIM_SET_COMPARE(&htim3, channel, speed);
}

uint16_t Get_Speed(uint32_t channel)
{
	return __HAL_TIM_GET_COMPARE(&htim3, channel);
}
