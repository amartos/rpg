CPP=gcc
CFLAGS=-O3 -no-pie -g
LDFLAGS=-lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_gfx -lSDL2_ttf -lm -lsqlite3
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
MAP_CREATOR=$(TOOLS)/map_creator.c errors.c screen.c images.c coord.c map.c 
MAP_CREATOR_EXEC=$(BIN)/map_creator

map_creator: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(MAP_CREATOR_EXEC) $(MAP_CREATOR)

#################
# Test units
#################
TEST=test
PATHFINDING=$(TEST)/pathfinding_test.c pathfinding.c coord.c errors.c macros.c
PFEXEC=$(BIN)/pathfinding_test
COORD=$(TEST)/coord_test.c coord.c macros.c
COORDEXEC=$(BIN)/coord_test
MAP=$(TEST)/map_test.c map.c errors.c
MAPEXEC=$(BIN)/map_test

pathfinding: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(PFEXEC) $(PATHFINDING)

coord: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(COORDEXEC) $(COORD)

map: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(MAPEXEC) $(MAP)
