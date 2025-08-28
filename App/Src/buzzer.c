
#include "buzzer.h"

static uint16_t syllables[] = {E5, D5S, E5, D5S, E5, B4, D5, C5, A4, F4, A4,
  		B4, G4, B4, C5, E5, D5S, E5, D5S, E5, B4, D5, C5, A4, F4, A4,
			B4, G4, B4, C5, B4, A4, G4, B4, C5, D5, E5, E5, F5, E5, D5, D5,
			E5, D5, C5, C5, D5, C5, B4, E5, D5S, E5, D5S, E5, B4, D5, C5,
			A4, F4, A4, B4, G4, B4, C5, B4, A4};
static uint16_t notes[] = {eNote, eNote, eNote, eNote, eNote, eNote, eNote, eNote,
		hNote, eNote, eNote, hNote, eNote, eNote, hNote, eNote, eNote, eNote, eNote,
		eNote, eNote, eNote, eNote, hNote, eNote, eNote, qNote, eNote, eNote, eNote,
		eNote, qNote, eNote, eNote, eNote, eNote, pqNote, eNote, eNote, eNote, pqNote,
		eNote, eNote, eNote, pqNote, eNote, eNote, eNote, hNote,
		eNote, eNote, eNote, eNote, eNote, eNote, eNote, eNote,
		hNote, eNote, eNote, qNote, eNote, eNote, eNote, eNote, hNote};

static uint16_t prev_time = 0;
static uint8_t idx = 0;

void BuzzerInit(BUZZER *buzzer, TIM_HandleTypeDef *htim, uint32_t channel)
{
	buzzer->htim = htim;
	buzzer->channel = channel;
	buzzer->state = BUZZER_IDLE;

	HAL_TIM_PWM_Start(htim, channel);
//	__HAL_TIM_SET_COMPARE(htim, channel, 0);
}

uint8_t non_blocking_delay(uint16_t interval)
{
	uint16_t curr_time = HAL_GetTick();
	if((uint16_t)(curr_time - prev_time) >= interval)
	{
		prev_time = curr_time;
		return 1;
	}
	return 0;
}

void CallBuzzer(BUZZER *buzzer, uint16_t syllable, uint16_t note)
{
	switch (buzzer->state)
	{
	case BUZZER_IDLE:
		__HAL_TIM_SET_PRESCALER(buzzer->htim, syllable);		   // ==  TIM4->PSC = song[i];
		__HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, 500); // ==  TIM4->CCR1 = 500;
		buzzer->state = BUZZER_PLAYING;
		prev_time = HAL_GetTick();
		break;

	case BUZZER_PLAYING:
		if(non_blocking_delay(note))
		{
			__HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, 0);
			buzzer->state = BUZZER_DELAY;
			prev_time = HAL_GetTick();
		}
		break;

	case BUZZER_DELAY:
		if(non_blocking_delay(50))
		{
			buzzer->state = BUZZER_IDLE;
			idx++;
		}
		break;
	}
}

void CallSong(BUZZER *buzzer)
{
	if (idx < sizeof(syllables) / sizeof(syllables[0]))
	{
		CallBuzzer(buzzer, syllables[idx], notes[idx]);
	}
	else
	{
		Reset_Buzzer();
	}
}

void CallHorn(BUZZER *buzzer)
{
	uint16_t horning = HORN_SYLLABLE_START;
    if (buzzer->state != BUZZER_PLAYING)
    {
    	if(horning < HORN_SYLLABLE_MAX)
    	{
        __HAL_TIM_SET_PRESCALER(buzzer->htim, horning++);
    	}
        __HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, 500);
        buzzer->state = BUZZER_PLAYING;
    }
}

void Reset_Buzzer(BUZZER *buzzer)
{
	idx = 0;
	buzzer->state = BUZZER_IDLE;
	__HAL_TIM_SET_COMPARE(buzzer->htim, buzzer->channel, 0);
}
