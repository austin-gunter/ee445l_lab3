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
int32_t SecondCounter = 0;
uint32_t MinuteCounter = 0;
uint32_t HourCounter = 12;
uint32_t PreviousMinute = 0;
uint32_t PreviousHour = 12;
uint32_t AlarmStatus = 0; 			// 0 by default. 1 = enabled
uint32_t TimeCycleIndex = 0;		// to move through the Time Array when Cycling
uint32_t ScreenCycleIndex = 0;	// to move along the screen when Cycling



void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	SecondCounter++;
	if(SecondCounter == 60){
		PreviousHour = HourCounter;
		SecondCounter = 0;
		TimeArray[4] += 1;
		PreviousMinute = MinuteCounter;
		MinuteCounter += 1;
		if(TimeArray[4] == 0x3A){					// minutes rollover
			TimeArray[4] = 0x30;
			TimeArray[3] += 1;
			if(TimeArray[3] == 0x36){				// hours rollover
				TimeArray[3] = 0x30;
				TimeArray[1] += 1;
				MinuteCounter = 0;
				PreviousHour = HourCounter;
				HourCounter += 1;																			//***************Special Cases****************//
				if(TimeArray[1] == 0x3A && TimeArray[0] == 0x30){			// 9:59 to 10:00
					TimeArray[1] = 0x30;
					TimeArray[0] += 1;
				}
				if(TimeArray[1] == 0x32 && TimeArray[0] == 0x31){				// 11:59 to 12:00, am to pm or pm to am
					if(TimeArray[6] == 0x61){
						TimeArray[6] = 0x70;
					}
					else{
						TimeArray[6] = 0x61;
					}
				}
				if(TimeArray[1] == 0x33 && TimeArray[0] == 0x31){				// 12:59 to 1:00
					TimeArray[1] = 0x31;
					TimeArray[0] = 0x30;
					HourCounter = 1;
					}
				}
			}
			ST7735_SetCursor(CursorArray[0], CursorArray[1]);
			ST7735_OutString(TimeArray);
			DrawHands(MinuteX, MinuteY, HourX, HourY, HourCounter, MinuteCounter, PreviousHour, PreviousMinute);
		}	
	}

void GPIOPortB_Handler(void){
	if(GPIO_PORTB_RIS_R&0x08){						// Arm/Disarm Alarm Button
		GPIO_PORTB_ICR_R = 0x08;
		if(AlarmStatus == 0){
			AlarmStatus ^= 1;
			ST7735_SetCursor(17, 4);
			ST7735_SetTextColor(ST7735_BLACK);
			ST7735_OutString("OFF");
			ST7735_SetTextColor(ST7735_YELLOW);
			ST7735_SetCursor(15, 4);
			ST7735_OutString("ON");
		}
		else{
			AlarmStatus ^= 1;
			ST7735_SetCursor(15, 4);
			ST7735_SetTextColor(ST7735_BLACK);
			ST7735_OutString("ON");
			ST7735_SetTextColor(ST7735_YELLOW);
			ST7735_SetCursor(17, 4);
			ST7735_OutString("OFF");
		}
	}
	else if(GPIO_PORTB_RIS_R&0x01){					// Edit Button
		GPIO_PORTB_ICR_R = 0x01;
		PreviousHour = HourCounter;
		PreviousMinute = MinuteCounter;
		while((GPIO_PORTB_RIS_R&0x01) == 0){
			if(GPIO_PORTB_RIS_R&0x08){
				GPIO_PORTB_ICR_R = 0x08;
				if(AlarmStatus == 0){
					AlarmStatus ^= 1;
					ST7735_SetCursor(17, 4);
					ST7735_SetTextColor(ST7735_BLACK);
					ST7735_OutString("OFF");
					ST7735_SetTextColor(ST7735_YELLOW);
					ST7735_SetCursor(15, 4);
					ST7735_OutString("ON");
				}
				else{
					AlarmStatus ^= 1;
					ST7735_SetCursor(15, 4);
					ST7735_SetTextColor(ST7735_BLACK);
					ST7735_OutString("ON");
					ST7735_SetTextColor(ST7735_YELLOW);
					ST7735_SetCursor(17, 4);
					ST7735_OutString("OFF");
				}
			}
			if(GPIO_PORTB_RIS_R&0x04){					// Increment Button
				if(TimeCycleIndex < 5){						// If Index is in the TimeArray
					TimeArray[TimeCycleIndex] += 1;
					if(TimeArray[0] == 0x32 || (TimeArray[1] > 0x32 && TimeArray[0] == 0x31)){		// If trying to increment 1st digit past 2
						TimeArray[0] = 0x30;																												// Or if trying to increment 1st digit to 1 when first digit is > 2
					}
					if(TimeArray[1] > 0x32){				// If trying to increment 2nd digit past 2 when 1st digit is 0
						if(TimeArray[0] == 0x31){
							TimeArray[1] = 0x30;
						}
					}
					if(TimeArray[1] == 0x3A){				// Carry over when second digit hits 9
						TimeArray[1] = 0x31;
					}
					if(TimeArray[3] == 6){
						///////////////////////////////////////////////////////////////////////continue working
					}
				}
				if(TimeCycleIndex == 6){					// If Index is on the a/p of "am" or "pm"
					if(TimeArray[6] == 0x61){
						TimeArray[6] = 0x70;
					}
					else{
						TimeArray[6] = 0x61;
					}
				}
				if(TimeCycleIndex > 6 && TimeCycleIndex < 13){		// If Index is in the AlarmArray
					AlarmArray[TimeCycleIndex - 7] += 1;
				}
			}
			if(GPIO_PORTB_RIS_R&0x02){					// Cycle Button
				TimeCycleIndex += 1;							// Cycle 0-6 for Time Array, skipping indices 2 and 5 (colon and space)
				ScreenCycleIndex += 1;						// Cycle 7-13 for Alarm Array, skipping 9 and 12 (colon and space).
				if(TimeCycleIndex == 2 || TimeCycleIndex == 5 || TimeCycleIndex == 9 || TimeCycleIndex == 12){
					TimeCycleIndex += 1;
					ScreenCycleIndex += 1;
				}
				if(TimeCycleIndex == 14){					// If index hits 14, restart at index 0
					TimeCycleIndex = 0;
					ScreenCycleIndex += 0;
				}								
				
			}
			
			
			
			
			
			
			
			
		}
		GPIO_PORTB_ICR_R = 0x01;
		HourCounter = (TimeArray[0]*10) + TimeArray[1];
		MinuteCounter = (TimeArray[3]*10) + TimeArray[4];
		DrawHands(MinuteX, MinuteY, HourX, HourY, HourCounter, MinuteCounter, PreviousHour, PreviousMinute);
		TimeCycleIndex = 0;				// reset cycle indices for next edit mode
		ScreenCycleIndex = 8;
		SecondCounter = -1;		// -1 because SecondCounter will immediately increment when leaving this interrupt
	}
}

int main(void){
		DisableInterrupts();
	  PLL_Init(Bus80MHz);                   // 80 MHz
		TimerInit();
		SwitchesInit();
//	  SpeakerInit();
		LCDArrayInit(TimeArray, AlarmArray, CursorArray);
		LCDDisplayInit(TimeArray, AlarmArray, CursorArray);
		EnableInterrupts();
		while(1){}
}
