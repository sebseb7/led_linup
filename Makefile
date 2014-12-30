UNAME := $(shell uname)


SRC = $(wildcard *.c)
SRC+= $(wildcard libs/*.c)

FLAGS= --std=gnu99 -Wall -Wextra -Wall -funsigned-char -Wundef -Wsign-compare -pedantic  -Wstrict-prototypes -lftdi1 -lm


ifeq ($(UNAME), Darwin)
	SRC+= sdl_draw/SDL_draw.c
	COMPILER = clang
	FLAGS   += -Qunused-arguments
	
	FLAGS   += `sdl-config --cflags` 
	FLAGS   += -Dsdl_support 
	LDFLAGS += `sdl-config --libs`  
endif

ifeq ($(UNAME), Linux)
	FLAGS   += -Dserial 
#    FLAGS +=  -lSDL
endif


all:
	gcc $(FLAGS) $(SRC) -o lineup -I. $(LDFLAGS)

clean:
	rm -f lineup

