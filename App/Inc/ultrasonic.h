
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "main.h"
#include "tim.h"
#include "delay.h"

#define FILTERCOUNT 10
#define NOTFILTERVALUE 10
#define FILTERVALUE 80

typedef struct{
	TIM_HandleTypeDef	*htim;
	uint32_t			channel;
	uint16_t			IC_Value[2];
	uint16_t			distance;
	uint8_t				captureFlag;
	uint8_t 			distanceFlag;
	uint8_t				prev_distance;
	uint8_t				repet_count;
}ULTRASONIC;

void HCSR04_Init(ULTRASONIC *ultrasonic, TIM_HandleTypeDef *htim, uint32_t channel);
void HCSR04_SetTrigger(GPIO_TypeDef *port, uint16_t pin);
void HCSR04_Trigger(ULTRASONIC *ultrasonic, TIM_HandleTypeDef *htim, uint8_t size);
void HCSR04_Process(ULTRASONIC *ultrasonic);
void HCSR04_CaptureHandler(TIM_HandleTypeDef *htim, ULTRASONIC *ultrasonic);
void Distance_Filter(ULTRASONIC *ultrasonic, uint16_t distance);

#endif /* ULTRASONIC_H_ */
