#include <stdint.h>
#include <stdio.h>
#include "Switch.h"
#include "LCD.h"
#include "Sound.h"
#include "Timer.h"
#include "../inc/tm4c123gh6pm.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts






int main(void){
		DisableInterrupts();
		TimerInit(); // Timer W init?
		EditSwitchInit();
		CycleSwitchInit();
		IncrementSwitchInit();
		ArmSwitchInit();
		SpeakerInit();
		LCDInit();
		EnableInterrupts();
		while(1){
		
		
		
		}
}
