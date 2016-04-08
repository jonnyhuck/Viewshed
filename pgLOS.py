#!/usr/bin/env python

import csv, sys, math, subprocess, os.path, psycopg2, viewshed

###
# Get the letters for an OS grid reference, 
# Derived from http://www.movable-type.co.uk/scripts/latlong-gridref.html"
###
def getLetters(x, y):
    "Get the letters for an OS grid reference."
    
    # TODO: Validate coordinates!!

    # get 100k grid square
    x100k = math.floor(x / 100000)
    y100k = math.floor(y / 100000)

    # get letter indices (relate to char codes)
    l1 = int((19 - y100k) - (19 - y100k) % 5 + math.floor((x100k + 10) / 5))
    l2 = int((19 - y100k) * 5 % 25 + x100k % 5)

    # miss out the letter 'I' (not used on OS grid)
    if l1 > 7: 
        l1 += 1
    if l2 > 7: 
        l2 += 1

    # get letters based upon the char codes
    return chr(l1 + ord('a')) + chr(l2 + ord('a'))


###
# Get the tile numbers from an OS grid reference, 
# Derived from http://www.movable-type.co.uk/scripts/latlong-gridref.html"
###
def getNumbers(x, y):
    "Get the tile numbers from an OS grid reference."

    # TODO: Validate coordinates!!
    
    # get the second number form each coordinate
    x = int(math.floor((x % 100000)) / 10000)
    y = int(math.floor((y % 100000)) / 10000)
    return str(x) + str(y)


###
# Get the tile path required for a given an OS grid reference, 
###
def getPath(x, y, dd):
    "Get the tile path required for a given an OS grid reference."

    # get the components
    letters =  getLetters(x, y)
    numbers =  getNumbers(x, y)

    # build path for data tile
    return  '/'.join([dd, letters, letters.upper() + numbers + ".asc"])


###
# Build a textfile list of paths required for the VRT, for use with gdalbuildvrt
# This version uses a single point and a radius to define the required area
###
def buildVRT1(x, y, r, t, wd, dd):
    "Build a textfile list of paths required for the VRT, for use with gdalbuildvrt. This version uses a single point and a radius to define the required area"

    # get top left coordinates required
    tlx = x - r
    tly = y + r

    # get bottom right coordinates required
    brx = x + r
    bry = y - r
    
    #print tlx, tly, brx, bry
    
    # prepare the file
    f = open("/".join([wd,'in.txt']), 'w')

    # get all tiles in question and write to file (1 tile buffer around all sides for edge cases)
    for xi in xrange(tlx - t, brx + t, t):
        for yi in xrange(bry - t, tly + t, t):
            path = getPath(xi, yi, dd)

            # verify that the file exists... otherwise ignore
            if(os.path.exists(path)):
                f.write(path + '\n')
            else:
                print "no data! "+ path
    
    # finish the file
    f.close()
    
    # build vrt
    subprocess.call(["gdalbuildvrt", "-q", "-overwrite", "-input_file_list", "in.txt", "out.vrt"])


###
# Build a textfile list of paths required for the VRT, for use with gdalbuildvrt
# This version uses two points to define the required area
###
def buildVRT2(x1, y1, x2, y2, t, wd, dd):
    "Build a textfile list of paths required for the VRT, for use with gdalbuildvrt. This version uses two points to define the required area."

    # get top left coordinates required
    tlx = min(x1, x2)
    tly = max(y1, y2)

    # get bottom right coordinates required
    brx = max(x1, x2)
    bry = min(y1, y2)
    
#     print tlx, tly, brx, bry
    
    # prepare the file
    f = open("/".join([wd,'in.txt']), 'w')

    # get all tiles in question and write to file (1 tile buffer around all sides for edge cases)
    for xi in xrange(tlx - t, brx + t, t):
        for yi in xrange(bry - t, tly + t, t):
            path = getPath(xi, yi, dd)

            # verify that the file exists... otherwise ignore
            if(os.path.exists(path)):
                f.write(path + '\n')
            else:
                print "no data! "+ path
    
    # finish the file
    f.close()
    
    # build vrt
    # TODO: replace with Python
    subprocess.call(["gdalbuildvrt", "-q", "-overwrite", "-input_file_list", "in.txt", "out.vrt"])


#######################################################################


# path to working directory
wd = "."

# path to data directory
dd = "/Users/jonnyhuck/Dropbox/Maps/_OS_OPEN_DATA/terr50_gagg_gb/data"

# Connect to an existing database
conn = psycopg2.connect("dbname=viewshed user=jonnyhuck")

# Open a cursor to perform database operations
cur = conn.cursor()
cur2 = conn.cursor()
cur3 = conn.cursor()

# Query the database for all towers
cur.execute("SELECT * FROM towers;")

# loop through each origin tower
for tower in cur:
    
    # print name of the origin tower     
#     print tower[1]

    # get all of the destination towers within 30km of the origin tower
    cur2.execute("select id_tower, tower_name, easting, northing from towers where id_tower != %s and st_dwithin(geom, ST_SetSRID(ST_POINT(%s, %s), 27700), %s);", (tower[0], tower[2], tower[3], 30000))

    # build the vrt data tile (this way only do it once per origin)
    buildVRT1(tower[2], tower[3], 30000, 10000, wd, dd)

    # verify that there were results
    if(cur2.rowcount > 0):
    
        # loop through each destination tower
        for row in cur2:
        
            # test for line of sight
            output = viewshed.doLoS(50, tower[2], tower[3], row[2], row[3], 40, 40, "out.vrt")
        
            # print the name of all the towers it can see
            if(output>0):
#                 print "    " + row[1]
                cur3.execute("insert into i_visibility (origin, destination) values (%s, %s);", (tower[0], row[0]))
    
    # Make the changes to the database persistent
    conn.commit()

#     print "----------------------------"
    
# Make the changes to the database persistent
conn.commit()

# Close communication with the database
cur.close()
cur2.close()
cur3.close()
conn.close()