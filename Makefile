CPP=gcc
CFLAGS=-O3 -no-pie
LDFLAGS=-lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_gfx -lm
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
TEST=test
PATHFINDING=$(TEST)/pathfinding_test.c pathfinding.c coord.c errors.c
PFEXEC=$(BIN)/pathfinding_test

pathfinding: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(PFEXEC) $(PATHFINDING)
