#include "Switch.h"
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#define PB0123						(*((volatile uint32_t *)0x4000503C))
#define PB0								(*((volatile uint32_t *)0x40005004))
#define PB1								(*((volatile uint32_t *)0x40005008))
#define PB2								(*((volatile uint32_t *)0x40005010))
#define PB3								(*((volatile uint32_t *)0x40005020))
	


// In handler, read button Data register & assign it to a variable. From then on, read the variable instead of the register,
// 		because reading the register actually clears it.


//**************************************************
// Initializes switches to Port B
// PB0: Edit Switch
// PB1: Cycle Switch
// PB2: Increment Switch
// PB3: Arm Switch
//***************************************************
void SwitchesInit(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
	GPIO_PORTB_DIR_R &= ~0x0F;				// makes PB0,1,2,3 in
	GPIO_PORTB_DEN_R |= 0x0F;					// enable digital I/O on PB0,1,2,3
	GPIO_PORTB_IS_R &= ~0x0F;					// edge-sensitive
	GPIO_PORTB_IBE_R &= ~0x0F;				// not both edges
	GPIO_PORTB_IEV_R &= ~0x0F;				// falling edge
	GPIO_PORTB_ICR_R = 0x0F;					// clear flags 0,1,2,3
	GPIO_PORTB_IM_R |= 0x0F;					// arm interrupt on PB0,1,2,3
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x00004000;		// Port B is priority 2
	NVIC_EN0_R |= 0x02;								// enable interrupt 1 on NVIC
}
