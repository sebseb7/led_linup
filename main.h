#ifndef MAIN_H_
#define MAIN_H_

enum {
	DISPLAY_WIDTH = 72,
	DISPLAY_HEIGHT = 64,
	LCD_WIDTH = 72,
	LCD_HEIGHT = 64
};

void orLedXY(uint16_t x,uint16_t y, uint8_t r,uint8_t g,uint8_t b);
void setLedXY(uint16_t x,uint16_t y, uint8_t r,uint8_t g,uint8_t b);
void getLedXY(uint16_t x, uint16_t y, uint8_t* r,uint8_t* g, uint8_t* b);
void invLedXY(uint16_t x, uint16_t y);
#endif
