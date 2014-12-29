#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "font8x6.h"
#include "font16pt.h"
#include "mcugui.h"

void draw_char_inv_8x6(uint16_t x,uint16_t y, char text)
{
	text-=32;
	int i;
	for (i = 0; i < 6; i++)
	{
		char ch =                font8x6[(int)text][i];

		int j;
		for (j = 0; j < 8; j++)
		{
			if(ch & (1<<j))
			{
				invLedXY(x+i,y+j+4);
			}
		}
	}
}
void fill_8x6(uint16_t x, uint16_t y, uint8_t count,uint8_t r, uint8_t g , int8_t b)
{
	int i;
	for (i = 0; i < 6*count; i++)
	{
		int j;
		for (j = 0; j < 8; j++)
		{
			setLedXY(x+i,y+j+4,r,g,b);
		}
	}
}
void draw_char_8x6(uint16_t x,uint16_t y, char text, uint8_t r,uint8_t g, uint8_t b)
{
	text-=32;
	int i;
	for (i = 0; i < 6; i++)
	{
		char ch =                font8x6[(int)text][i];

		int j;
		for (j = 0; j < 8; j++)
		{
			if(ch & (1<<j))
			{
				setLedXY(x+i,y+j+4,r,g,b);
			}
		}
	}
}


void draw_char_16pt(uint16_t x,uint16_t y, uint8_t text, uint8_t r,uint8_t g, uint8_t b)
{
	text-=32;
	uint8_t i,j;

	uint16_t start = startIdx_16pt[text]; 
	uint8_t width = charWidth_16pt[text];

	uint8_t r2=0;
	uint8_t g2=0;
	uint8_t b2=0;

	for (j = 0; j < width; j ++)
	{

		for (i = 0; i < 22; i++)
		{
			char pix =    charMap_16pt[start + j + i*width];

			
			getLedXY(x+j,y+i,&r2,&g2,&b2);


			setLedXY(x+j,y+i,
				((pix*r)>>8)+(((255-pix)*r2)>>8),
				((pix*g)>>8)+(((255-pix)*g2)>>8),
				((pix*b)>>8)+(((255-pix)*b2)>>8)
			);
		}
	}
}
void draw_char_inv_16pt(uint16_t x,uint16_t y, uint8_t text)
{
	text-=32;
	uint8_t i,j;

	uint16_t start = startIdx_16pt[text]; 
	uint8_t width = charWidth_16pt[text];

	uint8_t r2=0;
	uint8_t g2=0;
	uint8_t b2=0;

	for (j = 0; j < width; j ++)
	{

		for (i = 0; i < 22; i++)
		{
			char pix =    charMap_16pt[start + j + i*width];

			
			getLedXY(x+j,y+i,&r2,&g2,&b2);



			setLedXY(x+j,y+i,
				(((255-r2)*(pix))>>8)+(((r2)*(255-pix))>>8),
				(((255-g2)*(pix))>>8)+(((g2)*(255-pix))>>8),
				(((255-b2)*(pix))>>8)+(((b2)*(255-pix))>>8)
			);
		}
	}
}
void draw_text_inv_8x6(uint16_t x, uint16_t y, const char *text)
{
	while (*text)
	{
		draw_char_inv_8x6(x,y,*text);
		x+=6;
		text++;
	}

}
void draw_text_8x6(uint16_t x, uint16_t y, const char *text, uint8_t r,uint8_t g,uint8_t b)
{
	while (*text)
	{
		draw_char_8x6(x,y,*text,r,g,b);
		x+=6;
		text++;
	}

}

void draw_text_16pt(uint16_t x, uint16_t y, const char *text, uint8_t r,uint8_t g,uint8_t b)
{
	while (*text)
	{
		draw_char_16pt(x,y,*text,r,g,b);
		x+=charWidth_16pt[(*text)-32]+2;
		text++;
	}
}

void draw_text_inv_16pt(uint16_t x, uint16_t y, const char *text)
{
	while (*text)
	{
		draw_char_inv_16pt(x,y,*text);
		x+=charWidth_16pt[(*text)-32]+2;
		text++;
	}
}

uint16_t get_text_width_16pt(const char *text)
{
	uint16_t width = 0;
	while (*text)
	{
		width+=charWidth_16pt[(*text)-32]+2;
		text++;
	}
	width-=2;
	return width;
}


void draw_number_8x6(uint16_t x, uint16_t y, int32_t number, uint8_t length, uint8_t pad, uint8_t r, uint8_t g , uint8_t b)
{

	char s[10];
#ifdef SIMULATOR
	snprintf(s,10, "%i", number);
#else
	snprintf(s,10, "%i", (int)number);
#endif

	int len = strlen(s);

	if (length < len) {
		int i;
		for (i = 0; i < length; i++) {
			draw_char_8x6(x, y, '*', r,g,b);
			x+=6;
		}
		return;
	}
	int i;
	for (i = 0; i < length - len; i++) {
		draw_char_8x6(x, y, pad, r,g,b);
		x+=6;
	}
	draw_text_8x6(x, y, (char*)s, r,g,b);

}

void draw_number_inv_8x6(uint16_t x, uint16_t y, int32_t number, uint8_t length, uint8_t pad)
{

	char s[10];
#ifdef SIMULATOR
	snprintf(s,10, "%i", number);
#else
	snprintf(s,10, "%i", (int)number);
#endif

	int len = strlen(s);

	if (length < len) {
		int i;
		for (i = 0; i < length; i++) {
			draw_char_inv_8x6(x, y, '*');
			x+=6;
		}
		return;
	}
	int i;
	for (i = 0; i < length - len; i++) {
		draw_char_inv_8x6(x, y, pad);
		x+=6;
	}
	draw_text_inv_8x6(x, y, (char*)s);

}

