#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#define sdl_support
#ifdef sdl_support
#include <SDL/SDL.h>
#include "sdl_draw/SDL_draw.h"
#endif

//#define serial

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


enum { ZOOM = 7 };


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


void display_highscore()
{
	tick++;

	int color = sini(tick<<7);
	int color2 = sini(tick<<9);

	print_5x3_at (4,0,"SEKTOR EVOLUTION",color>>12);
	print_5x3_at (4,0,"   SYLVESTER    ",15-(color>>12));


	if(state_a == FADE_IN)
	{
		state_b++;
		if(state_b>=100)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_CLOCK_IN)
	{
		state_b++;
		if(state_b>=72)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_NAMES_IN)
	{
		state_b++;
		if(state_b>=72)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == WAIT_A)
	{
		state_b++;
		if(state_b>=500)
		{
			state_a++;
			state_b=0;
		}
	}
	else if(state_a == SCROLL_OUT)
	{
		state_b++;
		if(state_b>=72)
		{
			state_a=SCROLL_CLOCK_IN;
			state_b=0;
			
				state_e++;
				if(state_d == FUTUREFLOOR)
				{
					if(state_e==2)
					{
						state_e=0;
						state_a=FADE_OUT;
					}
				}
				if(state_d == SPYFLOOR)
				{
					if(state_e==1)
					{
						state_e=0;
						state_a=FADE_OUT;
					}
				}
				if(state_d == IGLOO)
				{
					if(state_e==1)
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
	char* floor="";

	char* time1 ="";
	char* time2 ="";
	char* time3 ="";

	char* dj1 = "";
	char* dj2 = "";
	char* dj3 = "";
	char* dj1_sm = "";
	char* dj2_sm = "";
	char* dj3_sm = "";


	if(state_d == FUTUREFLOOR)
	{
		floor="Futurefloor:";
		if(state_e == 0)
		{
			dj1=" slot a";
			dj2=" slot b";
			dj3=" slot c";
			time1 ="00:oo - 01:oo ";
			time2 ="01:oo - 02:oo ";
			time3 ="02:oo - 03:oo ";
		}
		if(state_e == 1)
		{
			dj1=" Hesed";
			dj1_sm="             (ATM)";
			dj2=" Tim vs. S10";
			dj3=" P3p3d";
			time1 ="03:oo - 09:3o ";
			time2 ="09:3o - 09:31 ";
			time3 ="09:31 - 19:45 ";
		}
	}
	if(state_d == SPYFLOOR)
	{
		floor="Spyfloor:";
		if(state_e == 0)
		{
			dj1=" slot 1";
			dj2=" slot 2";
			dj3=" slot 3";
			time1 ="00:oo - 02:oo ";
			time2 ="02:oo - 04:oo ";
			time3 ="04:oo - 06:oo ";
		}
	}
	if(state_d == IGLOO)
	{
		floor="Igloo:";
		if(state_e == 0)
		{
			dj1=" slot x";
			dj2=" slot y";
			dj3=" slot z";
			time1 ="00:oo - 02:oo ";
			time2 ="02:oo - 04:oo ";
			time3 ="04:oo - 06:oo ";
		}
	}






	print_5x3_at (0,7,floor,10);
	draw_filledRect(0,13,72,7,0,11,0);
	draw_filledRect(0,30,72,7,0,11,0);
	draw_filledRect(0,47,72,7,0,11,0);


	if(state_a==SCROLL_CLOCK_IN)
	{
		print_5x3_at (1+(72-state_b),14,time1,16+(color2>>13));
		print_5x3_at (1+(72-state_b),31,time2,16+(color2>>13));
		print_5x3_at (1+(72-state_b),48,time3,16+(color2>>13));
	}
	if(state_a==SCROLL_NAMES_IN)
	{
		print_5x3_at (1,14,time1,16+(color2>>13));
		print_5x3_at (1,31,time2,16+(color2>>13));
		print_5x3_at (1,48,time3,16+(color2>>13));
		draw_text_8x6 (0+(72-state_b),17,dj1,0,15,0);
		draw_text_8x6 (0+(72-state_b),34,dj2,0,15,0);
		draw_text_8x6 (0+(72-state_b),51,dj3,0,15,0);
		print_5x3_at (2+(72-state_b),17+6,dj1_sm,15);
		print_5x3_at (2+(72-state_b),34+6,dj2_sm,15);
		print_5x3_at (2+(72-state_b),51+6,dj3_sm,15);
	}
	if(state_a==WAIT_A)
	{
		print_5x3_at (1,14,time1,16+(color2>>13));
		print_5x3_at (1,31,time2,16+(color2>>13));
		print_5x3_at (1,48,time3,16+(color2>>13));
		draw_text_8x6 (0,17,dj1,0,15,0);
		draw_text_8x6 (0,34,dj2,0,15,0);
		draw_text_8x6 (0,51,dj3,0,15,0);
		print_5x3_at (0+2,17+6,dj1_sm,15);
		print_5x3_at (0+2,34+6,dj2_sm,15);
		print_5x3_at (0+2,51+6,dj3_sm,15);
	}
	if(state_a==SCROLL_OUT)
	{
		print_5x3_at (1-state_b,14,time1,16+(color2>>13));
		print_5x3_at (1-state_b,31,time2,16+(color2>>13));
		print_5x3_at (1-state_b,48,time3,16+(color2>>13));
		draw_text_8x6 (0-state_b,17,dj1,0,15,0);
		draw_text_8x6 (0-state_b,34,dj2,0,15,0);
		draw_text_8x6 (0-state_b,51,dj3,0,15,0);
		print_5x3_at (2-state_b,17+6,dj1_sm,15);
		print_5x3_at (2-state_b,34+6,dj2_sm,15);
		print_5x3_at (2-state_b,51+6,dj3_sm,15);
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
	

	for(y = 0; y < LCD_HEIGHT; y++) 
	{
		for(x = 0; x < LCD_WIDTH; x++) 
		{
			float dist = _sinf(pythagorasf(x0-x,y0-y));
			float dist2 = _sinf(pythagorasf(x1-x,y1-y));
				// sini((dist*dist2*dist2)+a*300)>>8, -> noise


			uint8_t red=0;
			uint8_t green=0;
			uint8_t blue=0;

			getLedXY(x,y,&red,&green,&blue);

			uint8_t new_green=0;

			if(state_a==FADE_IN)
			{
				new_green = MIN(15,(green*(state_b/100.0f))+((((100-state_b)/75.0f)+0.25f)*(sini((dist*dist2*64000)+a*200)>>12)));
			}
			else if(state_a==FADE_OUT)
			{
				new_green = MIN(15,(green*((100-state_b)/100.0f))+((((state_b)/75.0f)+0.25f)*(sini((dist*dist2*64000)+a*200)>>12)));
			}
			else if(state_a==WAIT_B)
			{
				new_green = MIN(15,(green*0.0f)+(1.00f*(sini((dist*dist2*64000)+a*200)>>12)));
			}
			else
			{
				new_green = MIN(15,(green*1.0f)+(0.25f*(sini((dist*dist2*64000)+a*200)>>12)));
			}

			setLedXY(x,y,0,new_green,0);
		}
	}
	a+=1;
	if(a==0x4000)
	{
		a=0;
	}
	}
}


void setLedXY(uint16_t x, uint16_t y, uint8_t red,uint8_t green, uint8_t blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
	display[y][x] = green;
	rerender = 1;
}

void orLedXY(uint16_t x, uint16_t y, uint8_t red,uint8_t green, uint8_t blue) {
	if (x >= LCD_WIDTH) return;
	if (y >= LCD_HEIGHT) return;
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
	*green = display[y][x];
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
			buf[pixel] = display[y2][x*2+1]*16+display[y2][x*2];
			pixel++;
		}
	}


		
	ret = ftdi_write_data(ftdi, buf, 2304);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
	usleep(2000);

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


int main(int argc, char *argv[]) {
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
		SDL_Delay(10);
#else
		usleep(20000);
#endif
	}

#ifdef sdl_support
	SDL_Quit();
#endif
	return 0;
}


