#!/usr/bin/env python3

# This utility script is used to convert TSV map format to the correct format
# and store it in the current DB used.
# TSV maps can be generated from https://donjon.bin.sh/fantasy/dungeon/

import re, sys, sqlite3
import argparse

DB_PATH = 'assets/assets.db'

# The conversion values need to change if tiles are added to the DB
# F is considered as NULL and should be used as default value for unknown TSV
# codes.
# Tile:[collision, cost, weather, z0, z1, z2]
tsv_to_db = {
        "F":[0,0,0,0x4000,0x0000,0x0000],
        "W":[1,0,0,0x0400,0x4001,0x0000],
        }

CREATE_TABLE='''
create table {}(
x int not null,
y int not null,
collision int not null,
cost int not null,
weather int not null,
z0 int not null,
z1 int not null,
z2 int not null,
foreign key (z0) references images(id) on update cascade on delete cascade,
foreign key (z1) references images(id) on update cascade on delete cascade,
foreign key (z2) references images(id) on update cascade on delete cascade,
primary key (x,y)
);'''

REFERENCE_MAP="insert into maps (name, maxx, maxy) values('{}', {}, {});"

INSERT_LINE_HEADER="insert into {} (x,y,collision,cost,weather,z0,z1,z2) values "
INSERT_LINE_EMPTY="({}, {}, {}, {}, {}, {}, {}, {}), "

def parse_args():
    parser = argparse.ArgumentParser(
            description="Convert TSV maps and store them in an SQLite DB. "\
        )

    parser.add_argument(
            "filename",
            help="path of the TSV file",
        )

    parser.add_argument(
            "-n", "--name",
            help="Name of the map to be used in the database. By default "\
            "it is the name of the TSV file."
        )
    parser.add_argument(
            "-d", "--database",
            help="path of the database to use. By default the path is "\
            "./{}.".format(DB_PATH)
        )

    args = parser.parse_args()
    if args.name:
        name = args.name
    else:
        name = args.filename

    if args.database:
        database = args.database
    else:
        database = DB_PATH

    return args.filename, name, database

def parse_tsv(filename):
    with open(filename, "r") as f:
        r = f.readlines()

    r = [i.rstrip("\n") for i in r]
    r = [i.split("\t") for i in r]
    for y,l in enumerate(r):
        for x,t in enumerate(l):
            if not t:
                r[y][x] = "W"
    return r

def convert_tsv(value):
    if (value in tsv_to_db.keys()):
        return tsv_to_db[value]
    return tsv_to_db["F"]

def save_into_db(name, database, r):
    conn = sqlite3.connect(database)
    c = conn.cursor()

    c.execute(CREATE_TABLE.format(name))
    c.execute(REFERENCE_MAP.format(name, len(r[0]), len(r)))

    query = INSERT_LINE_HEADER.format(name)
    for y,l in enumerate(r):
        for x,t in enumerate(l):
            query += INSERT_LINE_EMPTY.format(x, y, *convert_tsv(t))
    query = query[:-2]+";" # remove the last ", " and close the query
    c.execute(query)

    # Save & quit
    conn.commit()
    conn.close()

if __name__ == "__main__":
    filename, name, database = parse_args()
    r = parse_tsv(filename)
    save_into_db(name, database, r)
