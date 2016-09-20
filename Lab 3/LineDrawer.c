#include <stdint.h>
#include <stdio.h>
#include "LineDrawer.h"
#include "ST7735.h"
#include <stdlib.h>


//*************ST7735_Line********************************************
//  Draws one line on the ST7735 color LCD
//  Inputs: (x1,y1) is the start point
//          (x2,y2) is the end point
// x1,x2 arehorizontal positions, columns from the left edge
//               must be less than 128
//               0 is on the left, 126 is near the right
// y1,y2 arevertical positions, rows from the top edge
//               must be less than 160	
//               159 is near the wires, 0 is the side opposite the wires
//        color 16-bit color, which can be produced by ST7735_Color565()
// Output: none
void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color){
	if(x1 == x2){															// If line is a vertical line
		if(y1 > y2){
			uint32_t temp = y1;
			y1 = y2;
			y2 = temp;
		}
			for(int i = y1; i <= y2; i++){
					ST7735_DrawPixel(x1, i, color);
			}
			return;
	}	
	if(y1 == y2){															// If line is a horizontal line
		if(x1 > x2){
			uint32_t temp = x1;
			x1 = x2;
			x2 = temp;
		}
			for(int i = x1; i <= x2; i++){
					ST7735_DrawPixel(i, y1, color);
			}
			return;
	}	
	
	int32_t deltaX = (x2 - x1);													// Otherwise, draw line with slope
	int32_t deltaY = (y2 - y1);
	int32_t currentY = y1;
	int32_t currentX = x1;
	if(abs(deltaX) <= abs(deltaY) && (y1 > y2)){				// Steep positive slope/Steep negative slope near 12
		uint32_t temp = x1;
		x1 = x2;
		x2 = temp;
		uint32_t temp2 = y1;
		y1 = y2;
		y2 = temp2;
		for(int i = y1; i <= y2; i++){
			currentX = x1 + deltaX*(i - y1)/deltaY;
			ST7735_DrawPixel(currentX, i, color);
			}
	}
	else if(abs(deltaX) >= abs(deltaY) && (x1 < x2)){		// Shallow positive slope/Shallow negative slope near 3
		for(int i = x1; i <= x2; i++){
				currentY = y1 + deltaY*(i - x1)/deltaX;
				ST7735_DrawPixel(i, currentY, color);
		}
	}
	else if(abs(deltaX) <= abs(deltaY) && (y1 < y2)){				// Steep negative slope/Steep positive slope near 6
		for(int i = y1; i <= y2; i++){
			currentX = x1 + deltaX*(i - y1)/deltaY;
			ST7735_DrawPixel(currentX, i, color);
		}
	}
	else if(abs(deltaX) >= abs(deltaY) && (x1 > x2)){				// Shallow positive slope/Shallow negative slope near 9
		uint32_t temp = x1;
		x1 = x2;
		x2 = temp;
		uint32_t temp2 = y1;
		y1 = y2;
		y2 = temp2;
		for(int i = x1; i <= x2; i++){
				currentY = y1 + deltaY*(i - x1)/deltaX;
				ST7735_DrawPixel(i, currentY, color);
		}
	}
}
