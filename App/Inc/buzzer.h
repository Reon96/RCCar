
#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "bsp.h"

#define C4	383
#define D4	340
#define E4	304
#define F4	287
#define G4	255
#define A4	227
#define B4	203
#define C5	191
#define D5	170
#define D5S	161
#define E5	152
#define F5	144

#define sNote 	81 //16분음표
#define eNote 	162 //8분음표
#define qNote	324 //4분음표
#define pqNote  486 //점4분음표
#define hNote 	648 //2분음표
#define phNote 	972 //점2분음표
#define wNote 	1296 //온음표

#define HORN_SYLLABLE_START 310
#define HORN_SYLLABLE_MAX	370

//#define SYLLABLES {MI, REs, MI, REs, MI, SI, RE, DO, LA, DO, MI, LA, SI}
//#define NOTES {E, E, E, E, E, E, E, E, E, E, E, E, QD}

typedef enum {
    BUZZER_IDLE,       // 대기 상태
    BUZZER_PLAYING,    // 음 재생 중
    BUZZER_DELAY       // 음 끈 후 지연 상태
} BUZZER_STATE;

typedef struct
{
	TIM_HandleTypeDef *htim;
	uint32_t		  channel;
	BUZZER_STATE	  state;
}BUZZER;

void BuzzerInit(BUZZER *buzzer, TIM_HandleTypeDef *htim, uint32_t channel);

void CallSong(BUZZER *buzzer);
void CallHorn(BUZZER *buzzer);
void Reset_Buzzer();

#endif /* INC_BUZZER_H_ */
