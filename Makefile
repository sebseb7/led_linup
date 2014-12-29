UNAME := $(shell uname)


SRC = $(wildcard *.c)
SRC+= sdl_draw/SDL_draw.c
SRC+= $(wildcard libs/*.c)

FLAGS= --std=gnu99 -Wall -funsigned-char -Wundef -Wsign-compare -pedantic  -Wstrict-prototypes -lftdi1


ifeq ($(UNAME), Darwin)
	COMPILER = clang
	FLAGS   += -Qunused-arguments
	
	FLAGS   += `sdl-config --cflags` 
	LDFLAGS += `sdl-config --libs`  
endif

ifeq ($(UNAME), Linux)
    FLAGS +=  -lSDL
endif


all:
	clang $(FLAGS) $(SRC) -o lineup -I. $(LDFLAGS)

clean:
	rm -f lineup

