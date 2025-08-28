#include "hw.h"

static L298N driver;
static uint16_t driver_pins[4] = Driver_Pins;
static ULTRASONIC ultrasonic[Ultrasonic_Size];
static BUZZER buzzer;
static uint8_t prevStep = 0;
static uint8_t rxBuffer[1];
static BUZZER_TYPE buzzer_type = NONE;
static uint8_t auto_flag = 0;
static uint8_t prev_dir = 'R';

void hwInit()
{
	HAL_UART_Receive_DMA(&huart6, rxBuffer, 1);
	L298N_Init(&driver, Driver_Port, driver_pins);
	HAL_TIM_Base_Start(Echo_TIM);
	HCSR04_Init(&ultrasonic[0], Echo_TIM, TIM_CHANNEL_1);
	HCSR04_Init(&ultrasonic[1], Echo_TIM, TIM_CHANNEL_3);
	HCSR04_Init(&ultrasonic[2], Echo_TIM, TIM_CHANNEL_4);
	HCSR04_SetTrigger(Trigger_Port, Trigger_Pin);
	BuzzerInit(&buzzer, &htim5, TIM_CHANNEL_1);
}

void Set_Step(uint8_t step)
{
	if (step > Max_Step)
		return;
	uint16_t speed = (step == 0) ? 0 : (Period / Max_Step * step - 1);
	Set_Speed(LEFT, speed);
	Set_Speed(RIGHT, speed);
}

void Move_Forward()
{
	for (uint8_t i = 0; i < 4; i++)
	{
		L298N_Drive(&driver, i, (i % 2 == 0) ? LOW : HIGH);  // Forward
	}
}

void Move_Backward()
{
	for (uint8_t i = 0; i < 4; i++)
	{
		L298N_Drive(&driver, i, (i % 2 != 0) ? LOW : HIGH);  // Backward
	}
}

void Move_Turn(uint32_t direction)
{
//	uint16_t speed = Get_Speed(direction);
	Set_Speed(direction, 100-1);
}

void Controller(uint8_t data)
{
	buzzer_type = (data == 'B') ? SONG : ((data == 'Y') ? HORN : NONE);
	Set_Step(prevStep);
	switch (data)
	{
	case 'Z':
		auto_flag = !auto_flag;
		if(!auto_flag) Controller('S');
		break;

	case 'S':
		for (uint8_t i = 0; i < 4; i++)
		{
			L298N_Drive(&driver, i, LOW);
		}
		break;

	case 'F':
		Move_Forward();
		break;

	case 'B':
		Move_Backward();
		break;

	case 'L':
		L298N_Drive(&driver, 0, HIGH);
		L298N_Drive(&driver, 1, LOW);
		L298N_Drive(&driver, 2, LOW);
		L298N_Drive(&driver, 3, HIGH);
		prev_dir = 'L';
		break;

	case 'R':
		L298N_Drive(&driver, 0, LOW);
		L298N_Drive(&driver, 1, HIGH);
		L298N_Drive(&driver, 2, HIGH);
		L298N_Drive(&driver, 3, LOW);
		prev_dir = 'R';
		break;

	case 'G':
		Move_Forward();
		Move_Turn(LEFT);
		break;

	case 'H':
		Move_Forward();
		Move_Turn(RIGHT);
		break;

	case 'I':
		Move_Backward();
		Move_Turn(LEFT);
		break;

	case 'J':
		Move_Backward();
		Move_Turn(RIGHT);
		break;
	}
}

void Ultrasonic_Process()
{
	HCSR04_Trigger(ultrasonic, Echo_TIM, Ultrasonic_Size);
	for (uint8_t i = 0; i < Ultrasonic_Size; i++)
	{
		HCSR04_Process(&ultrasonic[i]);
	}
}

void AutoRun_Process()
{
	static uint16_t prev_time = 0;

	uint16_t curr_time = HAL_GetTick();

	uint16_t forward = ultrasonic[0].distance;
	uint16_t left = ultrasonic[1].distance;
	uint16_t right = ultrasonic[2].distance;

	Distance_State forward_state =
			(forward > AUTO_F_Distance) ? SAFE :
			(forward > AUTO_F_Ditection) ? WARNING : DANGER;
	Distance_State left_state = (left > AUTO_T_Distance) ? SAFE :
								(left > AUTO_T_Ditection) ? WARNING : DANGER;
	Distance_State right_state = (right > AUTO_T_Distance) ? SAFE :
									(right > AUTO_T_Ditection) ? WARNING : DANGER;
	if (auto_flag)
	{
		Set_Step(prevStep);
		switch (forward_state)
		{
		case SAFE:
				Controller('F');
			break;
		case WARNING:
			if ((uint16_t)(curr_time - prev_time) > TurningTime)
			{
				prev_time = curr_time;
				if(left_state != SAFE || right_state != SAFE)
				{
					if(left > right) Controller('L');
					else if(right > left) Controller('R');
					else Controller(prev_dir);
				}
			}
			else
			{
				Controller(prev_dir);
			}
			break;
		case DANGER:
				Set_Step(2);
				Controller('B');
			break;
		}
	}
}


uint8_t Hold_Check()
{
	static uint16_t input_time = 0;
	static uint8_t hold_flag = 0;
	static BUZZER_TYPE prev_state = NONE;

	uint16_t curr_time = HAL_GetTick();

	if(prev_state != buzzer_type) hold_flag = 0;
	if (hold_flag)
	{
		if((uint16_t)(curr_time - input_time) > HOLD_TIME) return 1;
	}
	else if (!hold_flag || prev_state != buzzer_type)
	{
		hold_flag = 1;
		input_time = curr_time;
		prev_state = buzzer_type;
	}

	return 0;
}

void Buzzer_Process()
{
	if(Hold_Check())
	{
		switch(buzzer_type)
		{
		case NONE:
			Reset_Buzzer(&buzzer);
			break;
		case SONG:
			CallSong(&buzzer);
			break;
		case HORN:
			CallHorn(&buzzer);
			break;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART6)
	{
		HAL_UART_Transmit(&huart2, rxBuffer, 1, 100);
		if(rxBuffer[0] == '\n' || rxBuffer[0] == '\r')
		{
			HAL_UART_Receive_DMA(&huart6, rxBuffer, 1);
			return;
		}
		if (isdigit(rxBuffer[0]))
		{
			prevStep = (uint8_t) (rxBuffer[0] - '0');
			Set_Step(prevStep);
		}
		else
		{
			Controller(rxBuffer[0]);
		}
		HAL_UART_Receive_DMA(&huart6, rxBuffer, 1);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	for (int i = 0; i < 3; i++)
	{
		if (htim->Instance == ultrasonic[i].htim->Instance)
		{
			HCSR04_CaptureHandler(htim, &ultrasonic[i]);
		}
	}
}
