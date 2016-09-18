#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"




void LCDArrayInit(char *Time, char *Alarm, uint32_t *Cursor);
void LCDDisplayInit(char *Time, char *Alarm, uint32_t *Cursor);
void DrawHands(uint32_t MinuteX[], uint32_t MinuteY[], uint32_t HourX[], uint32_t HourY[]);
