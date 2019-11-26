.PHONY: map_creator pathfinding coord map

#################
# Common options
#################

CPP=gcc
CFLAGS=-O3 -no-pie -g
LDFLAGS=-lSDL2 -lSDL2_image -lm -lsqlite3

#################
# Main program
#################

BIN=bin
FILES=*
EXEC=$(BIN)/exec

all: init compile clean

compile: libs
	@${CPP} $(CFLAGS) -o $(EXEC) *.o ${LDFLAGS}

libs:
	@${CPP} $(CFLAGS) -c $(FILES).c

init:
	@mkdir -p bin

clean:	
	@rm -rf *.o

nuke: clean
	@rm -rf bin

#################
# Tools
#################

TOOLS=tools

MAP_CREATOR=$(TOOLS)/map_creator.c errors.c screen.c images.c coord.c map.c assets.c macros.c animation.c movement.c pathfinding.c
MAP_CREATOR_EXEC=$(BIN)/map_creator
map_creator: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(MAP_CREATOR_EXEC) $(MAP_CREATOR)

#################
# Test units
#################

TEST=test

PATHFINDING=$(TEST)/pathfinding_test.c pathfinding.c coord.c errors.c macros.c map.c screen.c
PFEXEC=$(BIN)/pathfinding_test
pathfinding: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(PFEXEC) $(PATHFINDING)

COORD=$(TEST)/coord_test.c coord.c macros.c
COORDEXEC=$(BIN)/coord_test
coord: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(COORDEXEC) $(COORD)

MAP=$(TEST)/map_test.c map.c errors.c
MAPEXEC=$(BIN)/map_test
map: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(MAPEXEC) $(MAP)
