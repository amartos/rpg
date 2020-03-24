#!/usr/bin/env python3

# This utility script is used to (re)generate the assets db. It uses the
# ts_to_db script to fill the maps table with the maps located in the
# assets/maps subdir.

import os, subprocess, sqlite3

ASSETS_PATH="assets/"

DB_NAME = "assets.db"
DB_PATH = ASSETS_PATH+DB_NAME

def save_into_db(connection, header, line, values):
    connection.execute(header)
    for id in sorted(list(values.keys())):
        new_item = line.format(id, *values[id])
        connection.execute(new_item)

def create_images_table():
    header='''
create table images(
id integer primary key autoincrement,
type text not null,
path text not null,
frames integer not null,
velocity integer not null
);'''

    line = "insert into images (id, type, path, frames, velocity) values ({}, '{}', '{}', {}, {});"
    images_path = ASSETS_PATH+"images/"

    tiles_path = images_path+"tiles/"
    cursors_path = images_path+"cursors/"
    characters_path = images_path+"characters/"

    # The values defined here corresponds to the Asset array index
    values = {
            # id      type        path/to/file                      frames  velocity
              0x0010:["mouse",     cursors_path+"hover.png",           1,      0],
              0x0011:["mouse",     cursors_path+"valid.png",           1,      0],
              0x0012:["mouse",     cursors_path+"invalid.png",         1,      0],
              0x0013:["tile",      cursors_path+"grid.png",            1,      0],
              0x0100:["character", characters_path+"knight_green.png", 8,      2],
              0x0101:["character", characters_path+"knight_red.png",   2,      2],
              0x0102:["character", characters_path+"knight_blue.png",  2,      2],
              0x0103:["character", characters_path+"knight_black.png", 2,      2],
              0x0400:["tile",      tiles_path+"wall0.png",             1,      0],
              0x4000:["tile",      tiles_path+"ground.png",            1,      0],
              0x4001:["tile",      tiles_path+"wall1.png",             1,      0],
            }

    return header, line, values


def create_maps_table():
    header='''
create table maps(
id integer primary key autoincrement,
name text not null,
maxx integer not null,
maxy integer not null
);'''

    return header, "", {} # filling is handled by the tsv_to_db.py script

def fill_maps_table():
    maps_dir = ASSETS_PATH+"maps/"
    for m in os.listdir(maps_dir):
        subprocess.Popen(["tools/tsv_to_db.py", maps_dir+m])

if __name__ == "__main__":
    if not DB_NAME in os.listdir(ASSETS_PATH):
        conn = sqlite3.connect(DB_PATH)
        c = conn.cursor()

        save_into_db(c, *create_images_table())
        save_into_db(c, *create_maps_table())

        # Save & quit
        conn.commit()
        conn.close()

        # we first close the DB because the tsv_to_db script, called by
        # fill_maps_table, uses the DB too.
        fill_maps_table()
