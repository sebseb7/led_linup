#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#ifdef sdl_support
#include <SDL/SDL.h>
#include "sdl_draw/SDL_draw.h"
#endif

#ifdef serial
#include "libftdi1/ftdi.h"
static struct ftdi_context *ftdi;
#endif

#include "main.h"
#include "libs/text.h"
#include "libs/text_5x3.h"
#include "libs/armmath.h"
#include "libs/rect.h"


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


enum { ZOOM = 17 };


static unsigned char	display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
static int				rerender = 1;

static int tick = 0;
static uint16_t a = 0;


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

static uint16_t state_a = 0;
static uint16_t state_b = 0;
static uint16_t state_d = 0;
static uint16_t state_e = 0;


static char *location_name = "    SEKTOR      ";
static char *event_name =    "   SYLVESTER    ";
static char *floor_list[] = {"TEKNOFLOOR","SPYFLOOR","IGLOO"};


#define FLOOR_A_LENGTH 3
#define FLOOR_B_LENGTH 2
#define FLOOR_C_LENGTH 2

static char *floor_a[FLOOR_A_LENGTH*9] = {
					"23:55 - 01:oo "," Ernstesy",     "",
					"01:oo - 02:3o "," 90 Grad",     "",
					"02:3o - 04:oo "," P3p3d",     "",
					"04:oo - 05:3o "," Steffen",      "",
					"05:3o - 07:oo "," Hesed","             (ATM)",
					"07:oo - 08:3o ",""," Andre Triple X",
					"08:3o - 09:3o "," Marvin","",
					"09:3o - 11:oo "," Matte Live","",
					"11:oo - 13:3o "," Maik","",

				};
static char *floor_b[FLOOR_B_LENGTH*9] = {
					"23:55 - 02:oo "," Ogrimizer",     "",
					"02:oo - 04:oo "," Meta",     "",
					"04:oo - 06:oo "," Knizpel",     "",
					"06:oo - 08:oo "," Metler",     "",
					"08:oo - 10:oo "," Zwielicht",     "",
					"10:oo - 12:oo "," LSDj 25",     "",
				};
static char *floor_c[FLOOR_C_LENGTH*9] = {
					"23:55 - 02:oo "," ?",     "",
					"02:oo - 04:oo "," ?",     "",
					"04:oo - 06:oo "," ?",     "",
					"06:oo - 07:oo "," ?",     "",
					"08:oo - 10:oo "," ?",     "",
					"10:oo - 12:oo "," ?",     "",
				};




void display_highscore()
{
	tick++;

	int color = sini(tick<<9);
	int color2 = sini(tick<<10);

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
	char* time1 ="";
	char* time2 ="";
	char* time3 ="";

	char* dj1 = "";
	char* dj2 = "";
	char* dj3 = "";
	char* dj1_sm = "";
	char* dj2_sm = "";
	char* dj3_sm = "";

	char* floor=floor_list[state_d];

	if(state_d == FUTUREFLOOR)
	{
		dj1=floor_a[state_e*9+1];
		dj2=floor_a[state_e*9+1+3];
		dj3=floor_a[state_e*9+1+6];
		dj1_sm=floor_a[state_e*9+2];
		dj2_sm=floor_a[state_e*9+2+3];
		dj3_sm=floor_a[state_e*9+2+6];
		time1=floor_a[state_e*9];
		time2=floor_a[state_e*9+3];
		time3=floor_a[state_e*9+6];
	}
	if(state_d == SPYFLOOR)
	{
		dj1=floor_b[state_e*9+1];
		dj2=floor_b[state_e*9+1+3];
		dj3=floor_b[state_e*9+1+6];
		dj1_sm=floor_b[state_e*9+2];
		dj2_sm=floor_b[state_e*9+2+3];
		dj3_sm=floor_b[state_e*9+2+6];
		time1=floor_b[state_e*9];
		time2=floor_b[state_e*9+3];
		time3=floor_b[state_e*9+6];
	}
	if(state_d == IGLOO)
	{
		dj1=floor_c[state_e*9+1];
		dj2=floor_c[state_e*9+1+3];
		dj3=floor_c[state_e*9+1+6];
		dj1_sm=floor_c[state_e*9+2];
		dj2_sm=floor_c[state_e*9+2+3];
		dj3_sm=floor_c[state_e*9+2+6];
		time1=floor_c[state_e*9];
		time2=floor_c[state_e*9+3];
		time3=floor_c[state_e*9+6];
	}






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
			int dist = sini(pythagorasf(x0-x,y0-y)*1024)>>9;
			int dist2 = sini(pythagorasf(x1-x,y1-y)*1024)>>9 ;
				// sini((dist*dist2*dist2)+a*300)>>8, -> noise


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
			//new_green = sini((dist*dist2)+a*300)>>12;

			setLedXY(x,y,0,new_green,0);
		}
	}
	a+=2;
	if(a==0x4000)
	{
		a=0;
	}
	}
}


void setLedXY(uint16_t x, uint16_t y, uint8_t red,uint8_t green, uint8_t blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	red=blue;
	blue=red;
	display[y][x] = green;
	rerender = 1;
}

void orLedXY(uint16_t x, uint16_t y, uint8_t red,uint8_t green, uint8_t blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	red=blue;
	blue=green;
	display[y][x] |= green;
	rerender = 1;
}

void invLedXY(uint16_t x, uint16_t y) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	display[y][x] = 16 - display[y][x];
	rerender = 1;
}
void getLedXY(uint16_t x, uint16_t y, uint8_t* red,uint8_t* green, uint8_t* blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	*red = display[y][x];
	*green = display[y][x];
	*blue = display[y][x];
}




#ifdef serial
void write_frame(void)
{
	unsigned char c=0x23;
	int ret = ftdi_write_data(ftdi, &c,1);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}

	static unsigned char buf[DISPLAY_HEIGHT*DISPLAY_WIDTH];

	uint32_t pixel=0;

	for(uint8_t y = 0;y<DISPLAY_HEIGHT;y++)
	{
		for(uint8_t x = 0;x<DISPLAY_WIDTH/2;x++)
		{
			uint8_t y2=y;
			if(y2>=32){y2-=32;} else{ y2+=32;};
		
			int pix = display[y2][x*2+1]*16+display[y2][x*2];

			if(pix == 0x23)
				pix = 0x24;
			if(pix == 0x42)
				pix = 0x43;
			if(pix == 0x65)
				pix = 0x67;
			if(pix == 0x66)
				pix = 0x67;
/*void write_escaped(uint8_t byte)
{
	if(byte == 0x23)
	{
		uint8_t c = 0x65;
		write(tty_fd,&c,1);
		c = 1;
		write(tty_fd,&c,1);
	}
	else if(byte == 0x42)
	{
		uint8_t c = 0x65;
		write(tty_fd,&c,1);
		c = 2;
		write(tty_fd,&c,1);
	}
	else if(byte == 0x65)
	{
		uint8_t c = 0x65;
		write(tty_fd,&c,1);
		c = 3;
		write(tty_fd,&c,1);
	}
	else if(byte == 0x66)
	{
		uint8_t c = 0x65;
		write(tty_fd,&c,1);
		c = 4;
		write(tty_fd,&c,1);
	}
	else
	{
		unsigned char c = byte;
		write(tty_fd,&c,1);
	}
}
*/
		
			buf[pixel] = pix;
			pixel++;
		}
	}


		
	ret = ftdi_write_data(ftdi, buf, 2304);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
	usleep(200);

}
#endif
	
	
void clear_display()
{
	for(uint8_t y = 0;y<DISPLAY_HEIGHT;y++)
	{
		for(uint8_t x = 0;x<DISPLAY_WIDTH;x++)
		{
			display[y][x]=0;
		}
	}
}


int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
#ifdef sdl_support
	srand(SDL_GetTicks());
#endif

#ifdef serial

	
	
	
	int ret;
	struct ftdi_version_info version;
	if ((ftdi = ftdi_new()) == 0)
	{
		fprintf(stderr, "ftdi_new failed\n");
		return EXIT_FAILURE;
	}
	version = ftdi_get_library_version();
	printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
			version.version_str, version.major, version.minor, version.micro,
			version.snapshot_str);
	if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6001)) < 0)
	{
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		ftdi_free(ftdi);
		return EXIT_FAILURE;
	}
	// Read out FTDIChip-ID of R type chips
	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;
		printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
		printf("FTDI chipid: %X\n", chipid);
	}
	ret = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set line parameters: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
	ret = ftdi_set_baudrate(ftdi, 500000);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set baudrate: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
	
		
	unsigned char c=66;
	ret = ftdi_write_data(ftdi, &c,1);
	c=0;
	ret = ftdi_write_data(ftdi, &c,1);
	ret = ftdi_write_data(ftdi, &c,1);
	ret = ftdi_write_data(ftdi, &c,1);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
	usleep(200);
#endif

#ifdef sdl_support
	SDL_Surface* screen = SDL_SetVideoMode(
			DISPLAY_WIDTH * ZOOM,
			DISPLAY_HEIGHT * ZOOM,
			32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	const unsigned int COLORS[] = {
		SDL_MapRGB(screen->format, 0x00,0x10,0x00),
		SDL_MapRGB(screen->format, 0x00,0x20,0x00),
		SDL_MapRGB(screen->format, 0x00,0x30,0x00),
		SDL_MapRGB(screen->format, 0x00,0x40,0x00),
		SDL_MapRGB(screen->format, 0x00,0x50,0x00),
		SDL_MapRGB(screen->format, 0x00,0x60,0x00),
		SDL_MapRGB(screen->format, 0x00,0x70,0x00),
		SDL_MapRGB(screen->format, 0x00,0x80,0x00),
		SDL_MapRGB(screen->format, 0x00,0x90,0x00),
		SDL_MapRGB(screen->format, 0x00,0xa0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xb0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xc0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xd0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xe0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xf0,0x00),
		SDL_MapRGB(screen->format, 0x00,0xff,0x00)
	};
#endif

	int running = 1;
		
				

	while(running) {



#ifdef sdl_support
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {

			switch(ev.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN:

					if(ev.type == SDL_KEYUP) break;

					switch(ev.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;

						default: break;
					}
				default: break;
			}
		}
#endif

		clear_display();
		display_highscore();

		if(rerender) {
			rerender = 0;
#ifdef sdl_support
			for(int x = 0; x < DISPLAY_WIDTH; x++)
				for(int y = 0; y < DISPLAY_HEIGHT; y++)
					Draw_FillCircle(screen, ZOOM * x + ZOOM / 2,
							ZOOM * y + ZOOM / 2, ZOOM * 0.45, COLORS[display[y][x]]);
			SDL_Flip(screen);
#endif
#ifdef serial
			write_frame();
#endif
		}
#ifdef sdl_support
		SDL_Delay(42);
#endif
	}

#ifdef sdl_support
	SDL_Quit();
#endif
	return 0;
}


