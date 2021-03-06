#include "LCD.h"
#include <stdint.h>
#include "ST7735.h"
#include "LineDrawer.h"

extern const unsigned short ClockFace_24bit[];


uint32_t TimeX   			=	0;
uint32_t TimeY  		  =	1;
uint32_t AlarmX  			=	2;
uint32_t AlarmY  			=	3;
uint32_t StatusX_ON 	= 4;
uint32_t StatusY_ON	  = 5;
uint32_t StatusX_OFF  = 6;
uint32_t StatusY_OFF  = 7;



void LCDArrayInit(char Time[], char Alarm[], uint32_t Cursor[]){
	// Initialize Time and Alarm Arrays to be "12:00 am"
	Time[0] = 0x31;						// 1
	Alarm[0] = 0x31;
	Time[1] = 0x32;						// 2
	Alarm[1] = 0x32;
	Time[2] = 0x3A;						// colon
	Alarm[2] = 0x3A;
	Time[3] = 0x30;						// 0
	Alarm[3] = 0x30;
	Time[4] = 0x30;						// 0
	Alarm[4] = 0x30;
	Time[5] = 0x20;						// space
	Alarm[5] = 0x20;
	Time[6] = 0x61;						// a or p (a = 0x61, p = 0x70)
	Alarm[6] = 0x61;
	Time[7] = 0x6D;						// m
	Alarm[7] = 0x6D;
	Time[8] = 0;							// null terminated
	Alarm[8] = 0;
	
	Cursor[TimeX] = 8;							// Initialize Coordinates Array
	Cursor[TimeY] = 0;
	Cursor[AlarmX] = 8;
	Cursor[AlarmY] = 2;
	Cursor[StatusX_ON] = 15;
	Cursor[StatusY_ON] = 4;
	Cursor[StatusX_OFF] = 17;
	Cursor[StatusY_OFF] = 4;
	
	ST7735_InitR(INITR_REDTAB);					// Set up display
	ST7735_SetCursor(0, 0);
	ST7735_OutString("Time:");
	ST7735_SetCursor(0, 2);
	ST7735_OutString("Alarm:");
	ST7735_SetCursor(0, 4);
	ST7735_OutString("Alarm Status:");
}

void LCDDisplayInit(char Time[], char Alarm[], uint32_t Cursor[]){
	ST7735_SetCursor(Cursor[TimeX], Cursor[TimeY]);
	ST7735_OutString(Time);
	ST7735_SetCursor(Cursor[AlarmX], Cursor[AlarmY]);
	ST7735_OutString(Alarm);
	ST7735_SetCursor(Cursor[StatusX_OFF], Cursor[StatusY_OFF]);
	ST7735_OutString("OFF");
	
	ST7735_DrawBitmap(14, 155, ClockFace_24bit, 100, 100);
	ST7735_Line(64, 106, 64, 73, ST7735_GREEN);
  ST7735_Line(64, 106, 64, 90, ST7735_RED);
	
}

void DrawHands(uint32_t MinuteX[], uint32_t MinuteY[], uint32_t HourX[], uint32_t HourY[], uint32_t HourCounter, uint32_t MinuteCounter, uint32_t PreviousHour, uint32_t PreviousMinute){
	ST7735_Line(64, 106, MinuteX[PreviousMinute], MinuteY[PreviousMinute], ST7735_BLACK);
	ST7735_Line(64, 106, HourX[4*(PreviousHour%12)+(PreviousMinute/15)], HourY[4*(PreviousHour%12)+(PreviousMinute/15)], ST7735_BLACK);
	
	ST7735_Line(64, 106, MinuteX[MinuteCounter], MinuteY[MinuteCounter], ST7735_GREEN);
	ST7735_Line(64, 106, HourX[4*(HourCounter%12)+(MinuteCounter/15)], HourY[4*(HourCounter%12)+(MinuteCounter/15)], ST7735_RED);





}
