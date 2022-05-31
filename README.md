# Viewshed
## A GDAL-based Viewshed command line tool with python bindings

### Written in collaboration with David Gullick at Lancaster University

This is a work in progress package for a research project. The goal is to provide a fast and efficient binary viewshed program that can be run either on the command line programatically (via Python) to analyse large batches of data.

In order to make a composite viewshed, you can use the [**Faster Calculator**](https://github.com/jonnyhuck/FasterCalculator) to combine the output binary viewsheds.

#### Command Line Usage

```
./viewshed

--radius <value> or -r <value> : set the viewshed radius.
--centerX <value> or -x <value> : set x coordinate of observer.
--centerY <value> or -y <value> : set y coordinate of observer.
--resolution <value> or -z <value> : set the resolution of the input data.
--observerheight <value> or -o <value> : set the height a.g.l. of the observer.
--targetheight <value> or -t <value> : set the height a.g.l. of the target.

--pointtopoint  or -p : calculate line of sight rather than viewshed
--pointtopointax <value> or -j <value> : set x coordinate of observer.
--pointtopointay <value> or -k <value> : set y coordinate of observer.
--pointtopointbx <value> or -l <value> : set x coordinate of target.
--pointtopointby <value> or -m <value> : set y coordinate of target.

--inputfile <value> or -i <value> : input file name (.tif).
--outputfile <value> or -f <value> : output file name (.tif).

```
For example:

`./viewshed -r 20000 -x 345678 -y 456789 -z 50 -o 1.5 -t 100 -i DEM.tif -f VIEWSHED.tif`

Would run a viewshed of 20km around the point 345678,456789 assuming a 1.5m tall person looking for a 100m tall object in the landscape. The input data (50m resolution) would be DEM.tif and the output file would be saved to VIEWSHED.tif.

To use with the included demo data, you could use, for example:
Generate a Viewshed:

`./viewshed -r 10000 -x 325000 -y 515000 -z 50 -o 1.5 -t 100 -i data.tif -f viewshed.tif`

Determine point-to-point visibility - returns 1 (true) or 0 (false)):

`./viewshed -p -j 325000 -k 515000 -l 326000 -m 516000 -o 1.5 -t 100 -i data.tif`

#### Python Usage

```python
import viewshed

#calculate a viewshed
viewshed.doViewshed(radius, resolution, centreX, centreY, observerHeight, targetHeight, inputFile, outputFile)

#calculate a line of sight
viewshed.doLoS(resolution, observerX, observerY, targetX, targetY, observerHeight, targetHeight, inputFile)
```

#### Requirements:
* [**GDAL**](http://www.gdal.org/)
	* **Mac (Homebrew):** `brew install gdal`
	* **Ubuntu:** `apt-get install gdal libgdal-dev`
	* **Windows:** [OSGeo4W](https://trac.osgeo.org/osgeo4w)

#### Building:
To compile the command line program:

##### Linux:

```bash
gcc viewshed.cpp -lgdal -lm -o viewshed -O3
```

##### Mac:
```bash
g++ -std=c++11 -stdlib=libc++ viewshed.cpp -F/Library/Frameworks/ -framework GDAL -o viewshed -O3
```
To rebuild the Python Bindings (requires [SWIG](http://swig.org/)):

```bash
swig -python -c++ viewshed.i; python setup.py build_ext --inplace

CFLAGS=-stdlib=libstdc++ 
```
