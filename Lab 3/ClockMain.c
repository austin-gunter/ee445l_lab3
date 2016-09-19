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
uint32_t MinuteCounter = 0;
uint32_t HourCounter = 12;
uint32_t PreviousMinute = 0;
uint32_t PreviousHour = 12;
uint32_t AlarmStatus = 0; 		// 0 by default. 1 = enabled

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	SecondCounter++;
	if(SecondCounter == 1){
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
				HourCounter += 1;																	//***************Special Cases****************//
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
	else if(GPIO_PORTB_RIS_R&0x01){
		GPIO_PORTB_ICR_R = 0x01;
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
			if(GPIO_PORTB_RIS_R&0x04){
			
			}
			if(GPIO_PORTB_RIS_R&0x02){
			
			}
			
			
			
			
			
			
			
			
		}
		GPIO_PORTB_ICR_R = 0x01;
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
		while(1){
			

		
		
		
		}
}
