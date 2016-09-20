#include "Sound.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define PB4								(*((volatile uint32_t *)0x40005040))



void SpeakerInit(void){	// set speaker output to PB4
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
                                    // 2) no need to unlock GPIO Port B
  GPIO_PORTB_AMSEL_R &= ~0x10;      // 3) disable analog on PB4
  GPIO_PORTB_PCTL_R &= ~0x000F0000; // 4) PCTL GPIO on PB4
  GPIO_PORTB_DIR_R &= ~0x10;        // 5) direction PB4 output
  GPIO_PORTB_AFSEL_R &= ~0x10;      // 6) PB4 regular port function
  GPIO_PORTB_DEN_R |= 0x10;         // 7) enable PB4 digital port




}
