#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
#include "libs/text.h"
#include "libs/text_5x3.h"
#include "libs/armmath.h"
#include "libs/rect.h"

static int ticka = 0;
static uint16_t a = 0;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


enum{
	FADE_IN=0,
	SCROLL_CLOCK_IN,
	SCROLL_NAMES_IN,
	WAIT_A,
	SCROLL_OUT,
	FADE_OUT,
	WAIT_B
};
enum{
	FUTUREFLOOR=0,
	SPYFLOOR,
	IGLOO
};

static uint16_t state_a = WAIT_B;
static uint16_t state_b = 0;
static uint16_t state_d = 0;
static uint16_t state_e = 0;


static char *location_name = "    SEKTOR      ";
static char *event_name =    "   SYLVESTER    ";
static char *floor_list[] = {"TEKNOFLOOR","SPYFLOOR","IGLOO"};


#define FLOOR_A_LENGTH 3
#define FLOOR_B_LENGTH 2
#define FLOOR_C_LENGTH 4

static char *floor_a[FLOOR_A_LENGTH*9] = {
	"23:55 - 01:oo "," Ernstesy",     "",
	"01:oo - 02:3o "," 90 Grad",     "",
	"02:3o - 04:oo "," Steffen",     "",
	"04:oo - 05:3o "," P3p3d",      "",
	"05:3o - 07:oo "," Hesed","             (ATM)",
	"07:oo - 08:3o ",""," Andre Triple X",
	"08:3o - 09:3o "," Marvin","",
	"09:3o - 11:oo "," Matte Live","",
	"11:oo - 13:3o "," Maik","",

};
static char *floor_b[FLOOR_B_LENGTH*9] = {
	"23:55 - 02:oo "," Ogrimizer",     "",
	"02:oo - 04:oo "," Knizpel",     "",
	"04:oo - 06:oo "," Meta",     "",
	"06:oo - 08:oo "," Metler",     "",
	"08:oo - 10:oo "," Zwielicht",     "",
	"10:oo - 12:oo "," LSDj 25",     "",
};
static char *floor_c[FLOOR_C_LENGTH*9] = {
	"23:55 - 02:oo "," Gnista",     "",
	"02:oo - 03:oo "," ",     " Dub Positive",
	"03:oo - 04:oo "," Shadi",     "",
	"04:oo - 05:oo "," ",     " Marcus Krassus",
	"05:oo - 07:oo "," Tajo",     "",
	"07:oo - 08:oo "," Marvin",     "",
	"08:oo - 10:oo "," ",     " MRRM & Felix V.",
	"10:oo - 12:oo "," ",     " Lenn. R. & Geronimo",
	"12:oo - 14:oo "," Navigate",     "",
	"14:oo - 16:oo "," ",     " Antoine de Ramena",
	"16:oo - 18:oo "," Ufo Guy",     "",
	" "," ",     "",
};




static uint8_t tick(void) {
	ticka++;

	clear_display();

	int color = sini(ticka<<9);
	int color2 = sini(ticka<<10);

	print_5x3_at (4,0,location_name,color>>12);
	print_5x3_at (4,0,event_name,15-(color>>12));


	if(state_a == FADE_IN)
	{
		state_b+=4;
		if(state_b>=100)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_CLOCK_IN)
	{
		state_b+=4;
		if(state_b>=72)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_NAMES_IN)
	{
		state_b+=4;
		if(state_b>=72)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == WAIT_A)
	{
		state_b+=2;
		if(state_b>=150)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_OUT)
	{
		state_b+=4;
		if(state_b>=72)
		{
			state_a=SCROLL_CLOCK_IN;
			state_b=0;

			state_e++;
			if(state_d == FUTUREFLOOR)
			{
				if(state_e==FLOOR_A_LENGTH)
				{
					state_e=0;
					state_a=FADE_OUT;
				}
			}
			if(state_d == SPYFLOOR)
			{
				if(state_e==FLOOR_B_LENGTH)
				{
					state_e=0;
					state_a=FADE_OUT;
				}
			}
			if(state_d == IGLOO)
			{
				if(state_e==FLOOR_C_LENGTH)
				{
					state_e=0;
					state_a=FADE_OUT;
				}
			}
		}
	}
	else if(state_a == FADE_OUT)
	{
		state_b++;
		if(state_b>=100)
		{
			state_a++;
			state_b=0;
			if(state_d == FUTUREFLOOR)
			{
				state_d=SPYFLOOR;
			}
			else if(state_d == SPYFLOOR)
			{
				state_d=IGLOO;
			}
			else if(state_d == IGLOO)
			{
				state_d=FUTUREFLOOR;
			}
		}
	}
	else if(state_a == WAIT_B)
	{
		state_b++;
		if(state_b>=50)
		{
			state_a=FADE_IN;
			state_b=0;
		}
	}

	char* floor=floor_list[state_d];
	
	char**strings;
	
	if(state_d == FUTUREFLOOR)
		strings=floor_a;
	if(state_d == SPYFLOOR)
		strings=floor_b;
	if(state_d == IGLOO)
		strings=floor_c;
		
	char* dj1=strings[state_e*9+1];
	char* dj2=strings[state_e*9+1+3];
	char* dj3=strings[state_e*9+1+6];
	char* dj1_sm=strings[state_e*9+2];
	char* dj2_sm=strings[state_e*9+2+3];
	char* dj3_sm=strings[state_e*9+2+6];
	char* time1=strings[state_e*9];
	char* time2=strings[state_e*9+3];
	char* time3=strings[state_e*9+6];


	print_5x3_at (0,9,floor,10);
	draw_filledRect(0,13+2,72,7,0,11,0);
	draw_filledRect(0,30+2,72,7,0,11,0);
	draw_filledRect(0,47+2,72,7,0,11,0);


	if(state_a==SCROLL_CLOCK_IN)
	{
		print_5x3_at (1+(72-state_b),14+2,time1,16+(color2>>15));
		print_5x3_at (1+(72-state_b),31+2,time2,16+(color2>>15));
		print_5x3_at (1+(72-state_b),48+2,time3,16+(color2>>15));
	}
	if(state_a==SCROLL_NAMES_IN)
	{
		print_5x3_at (1,14+2,time1,16+(color2>>15));
		print_5x3_at (1,31+2,time2,16+(color2>>15));
		print_5x3_at (1,48+2,time3,16+(color2>>15));
		draw_text_8x6 (0+(72-state_b),17+2,dj1,0,15,0);
		draw_text_8x6 (0+(72-state_b),34+2,dj2,0,15,0);
		draw_text_8x6 (0+(72-state_b),51+2,dj3,0,15,0);
		print_5x3_at (2+(72-state_b),17+6+2,dj1_sm,15);
		print_5x3_at (2+(72-state_b),34+6+2,dj2_sm,15);
		print_5x3_at (2+(72-state_b),51+6+2,dj3_sm,15);
	}
	if(state_a==WAIT_A)
	{
		print_5x3_at (1,14+2,time1,16+(color2>>15));
		print_5x3_at (1,31+2,time2,16+(color2>>15));
		print_5x3_at (1,48+2,time3,16+(color2>>15));
		draw_text_8x6 (0,17+2,dj1,0,15,0);
		draw_text_8x6 (0,34+2,dj2,0,15,0);
		draw_text_8x6 (0,51+2,dj3,0,15,0);
		print_5x3_at (0+2,17+6+2,dj1_sm,15);
		print_5x3_at (0+2,34+6+2,dj2_sm,15);
		print_5x3_at (0+2,51+6+2,dj3_sm,15);
	}
	if(state_a==SCROLL_OUT)
	{
		print_5x3_at (1-state_b,14+2,time1,16+(color2>>15));
		print_5x3_at (1-state_b,31+2,time2,16+(color2>>15));
		print_5x3_at (1-state_b,48+2,time3,16+(color2>>15));
		draw_text_8x6 (0-state_b,17+2,dj1,0,15,0);
		draw_text_8x6 (0-state_b,34+2,dj2,0,15,0);
		draw_text_8x6 (0-state_b,51+2,dj3,0,15,0);
		print_5x3_at (2-state_b,17+6+2,dj1_sm,15);
		print_5x3_at (2-state_b,34+6+2,dj2_sm,15);
		print_5x3_at (2-state_b,51+6+2,dj3_sm,15);
	}
	if(state_a==WAIT_B)
	{
	}




	{

		float x0 = sini(a*25)/910.0f; 
		float y0 = sini((a*40)+0x1000)/910.0f;
		float x1 = sini(a*50)/1024.0f;
		float y1 = sini((a*80)+0x2000)/1024.0f;
		uint8_t x, y;


		for(y = 8; y < LCD_HEIGHT; y++) 
		{
			for(x = 0; x < LCD_WIDTH; x++) 
			{
				int dist = sini(pythagorasf(x0-x,y0-y)*1024.0f)>>9;
				int dist2 = sini(pythagorasf(x1-x,y1-y)*1024.0f)>>9 ;

				uint8_t red=0;
				uint8_t green=0;
				uint8_t blue=0;

				getLedXY(x,y,&red,&green,&blue);

				uint8_t factor=0;

				if(state_a==FADE_IN)
				{
					factor = 100-state_b;
				}
				else if(state_a==FADE_OUT)
				{
					factor = state_b;
				}
				else if(state_a==WAIT_B)
				{
					factor=100;
				}
				else
				{
					factor=0;
				}
				uint8_t new_green = MIN(15,(green*((100-factor)/100.0f))+((((factor)/85.0f)+0.15f)*( sini((dist*dist2)+a*300)>>12)));

				setLedXY(x,y,0,new_green,0);
			}
		}
		a+=2;
		if(a==0x4000)
		{
			a=0;
		}
	}
	return 0;
}

static void init(void) {
}

static void deinit(void) {
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 22, 2000);
}
