#include <stdint.h>
#include <stdio.h>
#include "PLL.h"
#include "Switch.h"
#include "LCD.h"
#include "Sound.h"
#include "Timer.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

extern uint32_t MinuteX[];
extern uint32_t MinuteY[];
extern uint32_t HourX[];
extern uint32_t HourY[];


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
char TimeArray[9];
char AlarmArray[9];
uint32_t CursorArray[8];			// Cursor[0,1] = Time coordinates
															// Cursor[2,3] = Alarm coordinates
															// Cursor[4,5] = Alarm Status ON  coordinates
															// Cursor[6,7] = Alarm Status OFF coordinates
uint32_t SecondCounter = 0;

void TimerHandler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	SecondCounter++;
	if(SecondCounter == 60){
		SecondCounter = 0;
		
	}
	
	ST7735_OutString(TimeArray);
	DrawHands(
	
}

int main(void){
//		DisableInterrupts();
//	  PLL_Init(Bus80MHz);                   // 80 MHz
//		TimerInit();
//		EditSwitchInit();
//		CycleSwitchInit();
//		IncrementSwitchInit();
//		ArmSwitchInit();
//		SpeakerInit();
		LCDArrayInit(TimeArray, AlarmArray, CursorArray);
		LCDDisplayInit(TimeArray, AlarmArray, CursorArray);
//		EnableInterrupts();
		while(1){
			

		
		
		
		}
}
