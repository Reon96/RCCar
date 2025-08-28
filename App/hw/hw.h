#ifndef HW_HW_H_
#define HW_HW_H_

#include "L298N.h"
#include "usart.h"
#include "ultrasonic.h"
#include "buzzer.h"

#define Max_Step 4

#define Driver_Port GPIOB
#define Driver_Pins {GPIO_PIN_3, GPIO_PIN_5, GPIO_PIN_10, GPIO_PIN_4}

#define Trigger_Port GPIOC
#define Trigger_Pin  GPIO_PIN_7

#define Echo_TIM	&htim4

#define RIGHT 	TIM_CHANNEL_1
#define LEFT 	TIM_CHANNEL_2

#define Ultrasonic_Size 3

#define AUTO_F_Distance	 30
#define AUTO_T_Distance	 40
#define AUTO_F_Ditection 15
#define AUTO_T_Ditection 20

#define TurningTime		750

#define HOLD_TIME 		50

typedef enum{
	NONE, SONG, HORN
}BUZZER_TYPE;

typedef enum{
	SAFE, WARNING, DANGER
}Distance_State;

typedef enum{
	LOW, HIGH
}Signal;

void hwInit();
void Controller(uint8_t data);
void Ultrasonic_Process();
void AutoRun_Process();
void Buzzer_Process();

#endif /* HW_HW_H_ */
