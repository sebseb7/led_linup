#ifndef _TEXT_H
#define _TEXT_H

#include "mcugui.h"

void fill_8x6(uint16_t x, uint16_t y, uint8_t count,uint8_t r, uint8_t g , int8_t b);
void draw_text_8x6(uint16_t x, uint16_t y, const char *text, uint8_t r,uint8_t g,uint8_t b);
void draw_text_inv_8x6(uint16_t x, uint16_t y, const char *text);
void draw_number_8x6(uint16_t x, uint16_t y, int32_t number, uint8_t length, uint8_t pad, uint8_t r, uint8_t g , uint8_t b);
void draw_number_inv_8x6(uint16_t x, uint16_t y, int32_t number, uint8_t length, uint8_t pad);

uint16_t get_text_width_16pt(const char *text);
void draw_text_16pt(uint16_t x, uint16_t y, const char *text, uint8_t r,uint8_t g,uint8_t b);
void draw_text_inv_16pt(uint16_t x, uint16_t y, const char *text);

#endif
