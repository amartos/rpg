CPP=gcc
CFLAGS=-O3
LDFLAGS=-lSDL -lSDL_mixer -lSDL_image -lSDL_gfx
BIN=bin
FILES=*
EXEC=$(BIN)/exec

all: init compile clean

compile: libs
	@${CPP} $(CFLAGS) -o $(EXEC) *.o ${LDFLAGS}

libs:
	@${CPP} $(CFLAGS) -g -c $(FILES).c

init:
	@mkdir -p bin

clean:	
	@rm -rf *.o

nuke: clean
	@rm -rf bin

#################
# Test units
#################

PATHFINDING=pathfinding.c coord.c errors.c
PFEXEC=$(BIN)/pathfinding_test

pathfinding: init
	@${CPP} $(CFLAGS) -o $(PFEXEC) $(PATHFINDING) ${LDFLAGS}
