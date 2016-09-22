#include "Sound.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define PB4								(*((volatile uint32_t *)0x40005040))



void SpeakerInit(void){	// set speaker output to PB4
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000004;     // 1) activate clock for Port C
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTC_DIR_R |= 0x10;        // 5) direction PC4 output
  GPIO_PORTC_AFSEL_R &= ~0x10;      // 6) PC4 regular port function
  GPIO_PORTC_DEN_R |= 0x10;         // 7) enable PC4 digital port
}
