#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "PLL.h"
#include "Switch.h"
#include "LCD.h"
#include "Sound.h"
#include "Timer.h"
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

#define PB4								(*((volatile uint32_t *)0x40005040))

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
uint32_t CycleIndex = 0;		// to move through the Time Array when Cycling


void waitDelay(void){
	for(int i = 0; i < 4000000; i++){} // wait 10ms to not bounce on switches
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	SecondCounter++;
	if(SecondCounter == 1){
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
				if(TimeArray[1] == 0x32 && TimeArray[0] == 0x31){			// 11:59 to 12:00, am to pm or pm to am
					if(TimeArray[6] == 0x61){
						TimeArray[6] = 0x70;
					}
					else{
						TimeArray[6] = 0x61;
					}
				}
				if(TimeArray[1] == 0x33 && TimeArray[0] == 0x31){			// 12:59 to 1:00
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
/////////////////////////////////////////////////////////////////////////////////
//	Arm/Disarm Buttom
//	Can be pushed at any time to arm or disarm the alarm
//	Can be pushed when the alarm is sounding to turn the alarm off
/////////////////////////////////////////////////////////////////////////////////
	if(GPIO_PORTB_RIS_R&0x08){
		GPIO_PORTB_IM_R &= ~0x08;
		GPIO_PORTB_ICR_R = 0x08;
		if(AlarmStatus == 0){
			AlarmStatus = 1;
			ST7735_SetCursor(17, 4);
			ST7735_OutString("   ");
			ST7735_SetCursor(15, 4);
			ST7735_OutString("ON");
		}
		else{
			AlarmStatus = 0;
			ST7735_SetCursor(15, 4);
			ST7735_OutString("  ");
			ST7735_SetCursor(17, 4);
			ST7735_OutString("OFF");
		}
		waitDelay();
		GPIO_PORTB_IM_R |= 0x08;
		return;
	}
/////////////////////////////////////////////////////////////////////////////////
//	Edit Buttom
//	Can be pushed to enter or exit edit mode
//	While edit mode is active, the Cycle/Increment buttons should be working
/////////////////////////////////////////////////////////////////////////////////
	else if(GPIO_PORTB_RIS_R&0x01){
		GPIO_PORTB_IM_R &= ~0x01;
		GPIO_PORTB_ICR_R = 0x01;
		PreviousHour = HourCounter;
		PreviousMinute = MinuteCounter;
		if(CycleIndex < 7){
			ST7735_SetCursor((CursorArray[0] + CycleIndex), 1);						// print a ^ where the cursor is currently at.
			ST7735_OutChar(0x5E);																
		}
		else{
			ST7735_SetCursor((CursorArray[2] + CycleIndex - 7), 3);
			ST7735_OutChar(0x5E);	
		}
		waitDelay();
		GPIO_PORTB_IM_R |= 0x01;
		while((GPIO_PORTB_RIS_R&0x01) == 0){
			if(GPIO_PORTB_RIS_R&0x08){
				GPIO_PORTB_IM_R &= ~0x08;
				GPIO_PORTB_ICR_R = 0x08;
				if(AlarmStatus == 0){
					AlarmStatus ^= 1;
					ST7735_SetCursor(17, 4);
					ST7735_OutString("   ");
					ST7735_SetCursor(15, 4);
					ST7735_OutString("ON");
				}
				else{
					AlarmStatus ^= 1;
					ST7735_SetCursor(15, 4);
					ST7735_OutString("  ");
					ST7735_SetCursor(17, 4);
					ST7735_OutString("OFF");
				}
				waitDelay();
				GPIO_PORTB_IM_R |= 0x08;
			}
/////////////////////////////////////////////////////////////////////////////////
//	Increment Buttom
//	Used to increment values in Time/Alarm arrays and on the screen
//	Should only do anything when in edit mode
/////////////////////////////////////////////////////////////////////////////////
			if(GPIO_PORTB_RIS_R&0x04){
				GPIO_PORTB_IM_R &= ~0x04;
				GPIO_PORTB_ICR_R = 0x04;
				if(CycleIndex < 5){								// If Index is in the TimeArray
					TimeArray[CycleIndex] += 1;
					if((TimeArray[0] >= 0x32) || ((TimeArray[1] > 0x32) && (TimeArray[0] == 0x31))){		// If trying to increment 1st digit past 2
						TimeArray[0] = 0x30;																												// Or if trying to increment 1st digit to 1 when first digit is > 2
					}
					if(TimeArray[1] > 0x32){				// If trying to increment 2nd digit past 2 when 1st digit is 1
						if(TimeArray[0] >= 0x31){
							TimeArray[1] = 0x30;
						}
					}
					if(TimeArray[1] >= 0x3A){				// Carry over when second digit passes 9
						TimeArray[1] = 0x31;
					}
					if(TimeArray[3] >= 0x36){					// Carry over when third digit passes 5
						TimeArray[3] = 0x30;
					}
					if(TimeArray[4] >= 0x3A){
						TimeArray[4] = 0x30;
					}
				}
				if(CycleIndex == 6){								// If Index is on the a/p of "am" or "pm"
					if(TimeArray[6] == 0x61){
						TimeArray[6] = 0x70;
					}
					else{
						TimeArray[6] = 0x61;
					}
				}
				if(CycleIndex > 6 && CycleIndex < 13){		// If Index is in the AlarmArray
					AlarmArray[CycleIndex - 7] += 1;
					if(AlarmArray[0] >= 0x32 || (AlarmArray[1] > 0x32 && AlarmArray[0] == 0x31)){	// If trying to increment 1st digit past 2
						AlarmArray[0] = 0x30;																												// Or if trying to increment 1st digit to 1 when first digit is > 2
					}
					if(AlarmArray[1] > 0x32){					// If trying to increment 2nd digit past 2 when 1st digit is 0
						if(AlarmArray[0] >= 0x31){
							AlarmArray[1] = 0x30;
						}
					}
					if(AlarmArray[1] >= 0x3A){				// Carry over when second digit passes 9
						AlarmArray[1] = 0x31;
					}
					if(AlarmArray[3] >= 0x36){				// Carry over when third digit passes 5
						AlarmArray[3] = 0x30;
					}
					if(AlarmArray[4] >= 0x3A){
						AlarmArray[4] = 0x30;
					}
				}
				if(CycleIndex == 13){								// If Index is on the "a/p" of Alarm
					if(AlarmArray[6] == 0x61){
						AlarmArray[6] = 0x70;
					}
					else{
						AlarmArray[6] = 0x61;
					}
				}
				
				if(CycleIndex < 7){															// Finished checking which button incremented. Now print on screen
					ST7735_SetCursor((CursorArray[0] + CycleIndex), 0);
					ST7735_OutChar(TimeArray[CycleIndex]);
				}
				else{
					ST7735_SetCursor((CursorArray[2] + CycleIndex - 7), 2);
					ST7735_OutChar(AlarmArray[CycleIndex - 7]);
				}
				waitDelay();
				GPIO_PORTB_IM_R |= 0x04;
			}
/////////////////////////////////////////////////////////////////////////////////
//	Cycle Buttom
//	Used to switch between places on the screen while indexing through the arrays
//	Should only do anything when in edit mode
/////////////////////////////////////////////////////////////////////////////////
			if(GPIO_PORTB_RIS_R&0x02){					// Cycle Button
				GPIO_PORTB_IM_R &= ~ 0x02;				
				GPIO_PORTB_ICR_R = 0x02;
				if(CycleIndex < 7){
					ST7735_SetCursor((CursorArray[0] + CycleIndex), 1);						// clear the current ^
					ST7735_OutChar(0x20);																
				}
				else{
					ST7735_SetCursor((CursorArray[2] + CycleIndex - 7), 3);
					ST7735_OutChar(0x20);	
				}
				
				CycleIndex += 1;							// Cycle 0-6 for Time Array, 7-13 for Alarm Array									
				if(CycleIndex == 2 || CycleIndex == 5 || CycleIndex == 9 || CycleIndex == 12){		// skip indices 2, 5, 9, and 12 because colons/spaces
					CycleIndex += 1;
				}
				if(CycleIndex == 14){					// If index hits 14, restart at index 0
					CycleIndex = 0;
				}		
				if(CycleIndex < 7){
					ST7735_SetCursor((CursorArray[0] + CycleIndex), 1);						// print a ^ where the cursor is currently at.
					ST7735_OutChar(0x5E);																
				}
				else{
					ST7735_SetCursor((CursorArray[2] + CycleIndex - 7), 3);
					ST7735_OutChar(0x5E);	
				}
				waitDelay();
				GPIO_PORTB_IM_R |= 0x02;
			}		
		}
//////////////////////////////////////////////////////////////////////////////////
// Once left Edit Mode
// Clear carrot, reset time data
//////////////////////////////////////////////////////////////////////////////////
		GPIO_PORTB_IM_R &= ~0x01;
		GPIO_PORTB_ICR_R = 0x01;
		HourCounter = ((TimeArray[0] - 0x30)*10) + (TimeArray[1] - 0x30);		// Time array has ascii values, so -0x30 to get to decimal
		MinuteCounter = ((TimeArray[3] - 0x30)*10) + (TimeArray[4] - 0x30);
		DrawHands(MinuteX, MinuteY, HourX, HourY, HourCounter, MinuteCounter, PreviousHour, PreviousMinute);
		if(CycleIndex < 7){
			ST7735_SetCursor((CursorArray[0] + CycleIndex), 1);						// clear the ^
			ST7735_OutChar(0x20);																
		}
		else{
			ST7735_SetCursor((CursorArray[2] + CycleIndex - 7), 3);
			ST7735_OutChar(0x20);	
		}
		CycleIndex = 0;				// reset cycle indices for next edit mode
		SecondCounter = -1;		// -1 because SecondCounter will immediately increment when leaving this interrupt
		waitDelay();
		GPIO_PORTB_IM_R |= 0x01;
	}
}

int main(void){
		DisableInterrupts();
	  PLL_Init(Bus80MHz);                   // 80 MHz
		TimerInit();
		SwitchesInit();
		SpeakerInit();
		LCDArrayInit(TimeArray, AlarmArray, CursorArray);
		LCDDisplayInit(TimeArray, AlarmArray, CursorArray);
		EnableInterrupts();
		while(1){
			if((strcmp(TimeArray, AlarmArray) == 0) && (AlarmStatus == 1)){
				ST7735_SetCursor(0, 3);
				ST7735_OutString("Alarm!!");
				while(AlarmStatus == 1){PB4 ^= 0x10;} // Wait until alarm turns off
				ST7735_SetCursor(0, 3);
				ST7735_OutString("       ");
			}
		}
}
