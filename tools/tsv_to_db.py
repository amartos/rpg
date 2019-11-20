#!usr/bin/env python3

import re, sys, sqlite3

if len(sys.argv) < 2:
    print("Error: give names of files as arguments")
else:
    for filename in sys.argv[1:]:
        with open(filename, "r") as f:
            r = f.readlines()

        r = [i.rstrip("\n") for i in r]
        r = [re.sub("F\t","F", i) for i in r]
        r = [re.sub("D.*?\t","F", i) for i in r]
        r = [re.sub("\t","W", i) for i in r]
        r = [i+"W" for i in r]

        maxx = len(r[0])
        maxy = len(r)

        conn = sqlite3.connect('ignore/assets.db')
        c = conn.cursor()

        query = '''
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
            );'''.format(filename)
        c.execute(query)

        query = "insert into maps (name, maxx, maxy) values('{}', {}, {});".format(
                filename, maxx, maxy
                )
        c.execute(query)

        query = "insert into {} (x,y,collision,cost,weather,z0,z1,z2) values ".format(filename)
        for y in range(0,maxy):
            for x in range(0,maxx):
                collision = cost = weather = z1 = z2 = 0
                if (r[y][x] == "F"):
                    z0 = 0x4000
                elif (r[y][x] == "W"):
                    z0 = 0x0400
                    z1 = 0x4001
                    collision = 1
                query += "({}, {}, {}, {}, {}, {}, {}, {}), ".format(
                            x, y, collision, cost, weather, z0, z1, z2
                            )

        query = query[:-2]+";"
        print(query)
        c.execute(query)

# Save (commit) the changes
conn.commit()

# We can also close the connection if we are done with it.
# Just be sure any changes have been committed or they will be lost.
conn.close()

