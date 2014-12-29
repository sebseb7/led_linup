#include "rect.h"
#include "mcugui.h"


void draw_filledRect(
	uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
	uint8_t r,
	uint8_t g,
	uint8_t b )
{

	int hx = h-1;
	int wx = w-1;

	uint16_t h2 = hx;

	for(;wx>=0;wx--)
	{
		for(hx=h2;hx>=0;hx--)
		{
			setLedXY(x+wx, y+hx, r,g,b);
		}
	}
}


void draw_rect(
	uint16_t x, uint16_t y, uint16_t w, uint16_t h, 
	uint8_t t,
	uint8_t r,
	uint8_t g,
	uint8_t b )
{

	uint16_t h2 = h;
	uint16_t w2 = w;

	for(;w>0;w--)
	{
		for(h=h2;h>0;h--)
		{
			if(	(w <= t)  || (w > w2-t) || (h <= t) || (h > h2-t ))
			{
				setLedXY(x+w, y+h, r,g,b);
			}
		}
	}
}




