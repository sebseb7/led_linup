#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

enum {
	DISPLAY_WIDTH = 72,
	DISPLAY_HEIGHT = 64,
	LCD_WIDTH = 72,
	LCD_HEIGHT = 64,
	ZOOM = 9
};

#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

void orLedXY(uint16_t x,uint16_t y, uint8_t r,uint8_t g,uint8_t b);
void setLedXY(uint16_t x,uint16_t y, uint8_t r,uint8_t g,uint8_t b);
void getLedXY(uint16_t x, uint16_t y, uint8_t* r,uint8_t* g, uint8_t* b);
void invLedXY(uint16_t x, uint16_t y);
void clear_display();

typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);
void registerAnimation(init_fun init,tick_fun tick,deinit_fun deinit, uint16_t t, uint16_t duration);

#endif
