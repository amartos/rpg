CPP=gcc
CFLAGS=-O3
LDFLAGS=-lSDL -lSDL_mixer -lSDL_image -lSDL_gfx
FILES=*

all: compile clean

compile: libs
	@${CPP} $(CFLAGS) -o exec *.o ${LDFLAGS}

libs:
	@${CPP} $(CFLAGS) -c $(FILES).c

clean:	
	@rm -rf *.o

nuke: clean
	@rm -rf exec
