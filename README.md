# Viewshed
##A GDAL-based Viewshed command line tool with python bindings

###Written in collaboration with David Gullick at Lancaster University

This is a work in progress package for a research project. The goal is to provide a fast and efficient binary viewshed program that can be run either on the command line programatically (via Python) to analyse large batches of data.

In order to make a composite viewshed, you can use the [**Faster Calculator**](https://github.com/jonnyhuck/FasterCalculator) to combine the output binary viewsheds.

####Command Line Usage

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


####Python Usage

```
import viewshed

#calculate a viewshed
viewshed.doViewshed(radius, resolution, centreX, centreY, observerHeight, targetHeight, inputFile, outputFile)

#calculate a line of sight
viewshed.doLoS(resolution, observerX, observerY, targetX, targetY, observerHeight, targetHeight, inputFile)
```

####Requirements:
* [**GDAL**](http://www.gdal.org/)
	* **Mac (Homebrew):** `brew install gdal`
	* **Ubuntu:** `apt-get gdal`
	* **Windows:** [OSGeo4W](https://trac.osgeo.org/osgeo4w)

####Building:
To compile the command line program:

```
 gcc viewshed.cpp -framework GDAL -o viewshed -O3
```

To rebuild the Python Bindings (requires `swig`):

```
swig -python -c++ viewshed.i ; python setup.py build_ext --inplace
```
---
###TODO:
* Add in Earth curvature.
* Tidy up code!
* Add in option to output Line of Sight Lines in GeoTiff
* Support other map projections