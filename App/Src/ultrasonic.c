#include "ultrasonic.h"

static GPIO_TypeDef *port = 0;
static uint16_t pin = 0;

void HCSR04_Init(ULTRASONIC *ultrasonic, TIM_HandleTypeDef *htim,
		uint32_t channel)
{
	*ultrasonic = (ULTRASONIC
			)
			{ 0 };

	ultrasonic->htim = htim;
	ultrasonic->channel = channel;

	HAL_TIM_IC_Start_IT(htim, channel);
}

void HCSR04_SetTrigger(GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
	port = gpio_port;
	pin = gpio_pin;
	GPIOInit(port, pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
}

void HCSR04_Trigger(ULTRASONIC *ultrasonic, TIM_HandleTypeDef *htim, uint8_t size)
{
	uint8_t flag = 1;
	uint32_t IT_Mask = 0;

	if (size > 4)
		return;
	for (uint8_t i = 0; i < size; i++)
	{
		if (ultrasonic[i].captureFlag == 0 && ultrasonic[i].distanceFlag == 0)
		{
			IT_Mask |= Get_TIM_IT_CC(ultrasonic[i].channel);
		}
		else
		{
			flag = 0;
			break;
		}
	}
	if (flag)
	{
		HAL_GPIO_WritePin(port, pin, RESET);
		delay_us(1);
		HAL_GPIO_WritePin(port, pin, SET);
		delay_us(10);
		HAL_GPIO_WritePin(port, pin, RESET);

		__HAL_TIM_SET_COUNTER(htim, 0);
		__HAL_TIM_ENABLE_IT(htim, IT_Mask);
	}
}

void HCSR04_Process(ULTRASONIC *ultrasonic)
{
	if (ultrasonic->distanceFlag == 1)
	{
		uint16_t echoTime = 0;

		if (ultrasonic->IC_Value[1] > ultrasonic->IC_Value[0])
		{
			echoTime = ultrasonic->IC_Value[1] - ultrasonic->IC_Value[0];
		}
		else if (ultrasonic->IC_Value[1] < ultrasonic->IC_Value[0])
		{
			echoTime = (0xffff - ultrasonic->IC_Value[0])
					+ ultrasonic->IC_Value[1];
		}

		uint16_t distance = echoTime / 58;

		Distance_Filter(ultrasonic, distance);

		ultrasonic->IC_Value[0] = 0;
		ultrasonic->IC_Value[1] = 0;
		ultrasonic->distanceFlag = 0;
	}
}

void HCSR04_CaptureHandler(TIM_HandleTypeDef *htim, ULTRASONIC *ultrasonic)
{
	if (htim->Channel == Get_Active_Channel(ultrasonic->channel))
	{
		if (ultrasonic->captureFlag == 0)	// if it hasn't been captured yet
		{
			// IC_Value1 in Count value when Capture Interrupt
			ultrasonic->IC_Value[0] = HAL_TIM_ReadCapturedValue(
					ultrasonic->htim, ultrasonic->channel);
			ultrasonic->captureFlag = 1;	// Flag it as captured

			__HAL_TIM_SET_CAPTUREPOLARITY(ultrasonic->htim, ultrasonic->channel,
					TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if (ultrasonic->captureFlag == 1)	// if one time of capture
		{

			ultrasonic->IC_Value[1] = HAL_TIM_ReadCapturedValue(
					ultrasonic->htim, ultrasonic->channel);
			ultrasonic->captureFlag = 0;
			ultrasonic->distanceFlag = 1;

			__HAL_TIM_SET_CAPTUREPOLARITY(ultrasonic->htim, ultrasonic->channel,
					TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(ultrasonic->htim,
					Get_TIM_IT_CC(ultrasonic->channel));
		}
	}
}

void Distance_Filter(ULTRASONIC *ultrasonic, uint16_t distance)
{
	if(distance < NOTFILTERVALUE)
	{
		ultrasonic->distance = distance;
		ultrasonic->prev_distance = distance;
		ultrasonic->repet_count = 0;
		return;
	}
	uint16_t difference = (distance > ultrasonic->prev_distance) ?
						 (distance - ultrasonic->prev_distance) :
						 (ultrasonic->prev_distance - distance);
	if (difference == 0)
	{
		ultrasonic->repet_count++;
		// 반복 횟수가 기준을 넘으면 필터링 완료
		if (ultrasonic->repet_count > FILTERCOUNT)
		{
			ultrasonic->repet_count = 0;  // 반복 횟수 초기화
			ultrasonic->distance = ultrasonic->prev_distance;
		}
		return;
	}
	else if (difference > FILTERVALUE)
	{
		ultrasonic->distance = ultrasonic->prev_distance;
		ultrasonic->repet_count = 0;  // 값이 튈 때는 반복 횟수 초기화
	}
	else
	{
		// 정상적인 변화일 경우 이전 값 갱신
		ultrasonic->distance = distance;
		ultrasonic->prev_distance = ultrasonic->distance;
		ultrasonic->repet_count = 0;  // 새로운 값이므로 반복 횟수 초기화
	}
}
