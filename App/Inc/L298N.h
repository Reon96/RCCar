#ifndef INC_L298N_H_
#define INC_L298N_H_

#include "bsp.h"
#include "tim.h"

#define Period 1000

typedef struct{
	GPIO_TypeDef	*port;
	uint16_t		*pins;
}L298N;

uint8_t L298N_Init(L298N *driver, GPIO_TypeDef *port, uint16_t *pins);
void L298N_Drive(L298N *driver, uint8_t num, GPIO_PinState state);
void Set_Speed(uint32_t channel, uint16_t speed);
uint16_t Get_Speed(uint32_t channel);

#endif /* INC_L298N_H_ */
