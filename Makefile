all:
	cc -o sdlcube sdlcube.c -lSDL2 -lm

ubuntudeps:
	dpkg -l | grep -w "libsdl2-dev" || sudo apt-get install libsdl2-dev
