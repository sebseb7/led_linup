#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#ifdef sdl_support
#include <SDL/SDL.h>
#include "sdl_draw/SDL_draw.h"
#endif
#include "main.h"

#define serial
#ifdef serial
#include "libftdi1/ftdi.h"
static struct ftdi_context *ftdi;
#endif




static unsigned char	display[DISPLAY_HEIGHT][DISPLAY_WIDTH];
static int				rerender = 1;



#define MAX_ANIMATIONS 200

int animationcount = 0;

struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	int duration;
	int min_delay;
} animations[MAX_ANIMATIONS];


void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].min_delay = t;

	animationcount++;

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
	unsigned char buf[DISPLAY_HEIGHT*DISPLAY_WIDTH*2+1];

	uint32_t pixel=0;
				
	buf[pixel++] = 0x23;

	for(uint8_t y = 0;y<DISPLAY_HEIGHT;y++)
	{
		for(uint8_t x = 0;x<DISPLAY_WIDTH/2;x++)
		{
			uint8_t y2=y;
			if(y2>=32){y2-=32;} else{ y2+=32;};

			int pix = display[y2][x*2+1]*16+display[y2][x*2];

			if(pix == 0x23)
			{
				buf[pixel++] = 0x65;
				buf[pixel++] = 1;
			}
			else if(pix == 0x42)
			{
				buf[pixel++] = 0x65;
				buf[pixel++] = 2;
			}
			else if(pix == 0x65)
			{
				buf[pixel++] = 0x65;
				buf[pixel++] = 3;
			}
			else if(pix == 0x66)
			{
				buf[pixel++] = 0x65;
				buf[pixel++] = 4;
			}
			else
			{
				buf[pixel++] = pix;
			}
		}
	}

	int ret = ftdi_write_data(ftdi, buf, pixel);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
	usleep(200);

}
void write_frame_old(void)
{
	unsigned char c=0x23;
	int ret = ftdi_write_data(ftdi, &c,1);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}

	static unsigned char buf[DISPLAY_HEIGHT*DISPLAY_WIDTH*2];

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


	int current_animation = 0;
	animations[current_animation].init_fp();

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

		animations[current_animation].tick_fp();

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


