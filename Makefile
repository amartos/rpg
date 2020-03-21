.PHONY: map_creator

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

DEP_ALL=errors.c macros.c
DEP_SCREEN=screen.c images.c assets.c animation.c movement.c

TOOLS=tools

MAP_CREATOR=$(TOOLS)/map_creator.c $(DEP_ALL) $(DEP_SCREEN) navigation.c pathfinding.c
MAP_CREATOR_EXEC=$(BIN)/map_creator
map_creator: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(MAP_CREATOR_EXEC) $(MAP_CREATOR)

#################
# Test units
#################

TEST=test

PATHFINDING=$(TEST)/pathfinding_test.c pathfinding.c navigation.c $(DEP_ALL) $(DEP_SCREEN)
PFEXEC=$(BIN)/pathfinding_test
pathfinding_test: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(PFEXEC) $(PATHFINDING)
	@$(PFEXEC)

NAV=$(TEST)/navigation_test.c navigation.c $(DEP_ALL)
NAVEXEC=$(BIN)/navigation_test
navigation_test: init clean
	@${CPP} $(CFLAGS) ${LDFLAGS} -o $(NAVEXEC) $(NAV)
	@${NAVEXEC}
