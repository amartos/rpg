.PHONY: map_creator generate_db

#################
# Common options
#################

CPP=gcc
CFLAGS=-O3 -no-pie -g -Wconversion
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
# DATABASE
#################

ASSETS_DB=assets/assets.db
ASSETS_SQL=assets/assets.sql
TOOLS=tools

dump_db:
	@sqlite3 $(ASSETS_DB) .dump > $(ASSETS_SQL)

reset_db: load_db

load_db:
	@rm -f $(ASSETS_DB)
	@cat $(ASSETS_SQL) | sqlite3 $(ASSETS_DB)

generate_db:
	@python3 $(TOOLS)/generate_db.py

#################
# Tools
#################

DEP_ALL=errors.c macros.c
DEP_SCREEN=screen.c images.c assets.c animation.c movement.c

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
