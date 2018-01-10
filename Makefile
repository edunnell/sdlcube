CC=cc
CFLAGS=-lSDL2 -lm

all:
	$(CC) -o sdlcube sdlcube.c $(CFLAGS)

ubuntudeps:
	dpkg -l | grep -w "libsdl2-dev" || sudo apt-get install libsdl2-dev
