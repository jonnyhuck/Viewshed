#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

//#define OS_WINDOWS //are we using windows? change headers over if so. if not, we assume OSX (but in theory should work on *NIX too)

#ifdef OS_WINDOWS
	#include "wingetopt.h"
	#include "gdal.h"
	#include "cpl_conv.h"
	#include "cpl_string.h"
	#include "ogr_core.h"
	#include "ogr_srs_api.h"
#else
	#include <getopt.h>
	#include "gdal.h"
	#include "cpl_conv.h"
	#include "cpl_string.h"
	#include "ogr_core.h"
	#include "ogr_srs_api.h"
#endif


//#################
// definitions
//#################

//convenience so we can use bool/boolean and true/false
#define bool int
#define boolean int
#define true 1
#define false 0

//fast math for pi
#define PI 3.14159265358979323846
#define TWOPI 6.2831853072
#define EARTH_DIAMETER 12740000
#define REFRACTION_COEFFICIENT 0.13


//#################
// Structs
//#################


/**
 * options struct so we can keep track of all of our commandline options
 */
typedef struct options {
	boolean quiet;  //do we spout out a lot of printfs
	int radius;     //what radius to do raytrace
	int resolution;    //what resolution are we using? e.g 50m per pixel
	int centerX;        //where do we want the center
	int centerY;
	char * inputFileName;   //what filename should we read?
	char * outputFileName;   //what filename should we read?
	float observerHeight;
	float targetHeight;

	float earthD;		//earth diameter
	float refractionC;	//refraction coefficient

	// coords for line of sight analysis
	int ax;
	int ay;
	int bx;
	int by;

	//does the user want LoS or Viewshed?
	int areUsingPointToPoint;

	//proj4 string (for projection)
	char * projection;

} Options;


/*
 * output data, including variables. This section defines the square that wraps our output data (e.g center +/- radius),
 * not the data as supplied to gdal. if in point 2 point mode, some options are ignored.
 */
typedef struct output_data {

	//everything in here is stored in METERS!
	int minx;
	int miny;
	int maxx;
	int maxy;
	int centerx;
	int centery;
	int radius;
	int width;
	int height;
	int resolution; //TODO: not sure if we should store this here as well, it is already in the options struct. Stored here as it means we can pass around a ref to one struct, instead of two.

	//everything here is stored in pixels!
	int pixelWidth;
	int pixelHeight;
	int pixelMinx;
	int pixelMiny;
	int pixelMaxx;
	int pixelMaxy;
	int pixelRadius;
	int pixelCenterx;
	int pixelCentery;

	//this is the actual output data that we will be drawing to.
	GByte* data;

} OutputData;


/**
 * this describes our input data format. For possible use in a future iteration, unused as of this version.
 */
typedef struct input_data {

	void* inputdata;
	int width;
	int height;
	int minx;
	int miny;
	int maxx;
	int maxy;

} InputData;


//#################
// function
// definitions
//#################

//sets up the options struct to some default values
void setupDefaultOptions(Options* op);

//prints all the details about the options
void printOptions(Options* op);

//convert x and y to a position in 1D array
int lineate(int x, int y, int width);

//adjust height to account for atmospheric refraction and earth curvature
double adjustHeight(int height, double distance, double earthD, double refractionC);

//same as above, but all parameters are in meters
void doSingleRTMeters(OutputData* data, float* inputData, int x1met, int y1met, int x2met, int y2met);

//bresenhams circle - calling doSingleRT from center  to every point on circumfrence.
void doRTCalc(OutputData* out, float* inputData);

//doSingleRTPointToPoint. Can A see B?
int doSingleRTPointToPoint(float* inputData, OutputData* out, int x1, int y1, int x2, int y2);

//bilinear interp stuff. Given the input data, and our position what is a bilin interp height?
double bliniearInterp(int x, int y, double br, double bl, double tl, double tr, double xr, double xl, double yt, double yb);

//Coordinate to pixels for X and Y
int coordinateToPixelX(OutputData* out, long x);
int coordinateToPixelY(OutputData* out, long y);

//nice useable wrapper for bliniarinterp! ask for point, get interped height out.
double getHeightAtMeters(OutputData* out, float* inputData, int mX, int mY);

//utility to do a viewshed programatically (for Python Bindings)
void doViewshed(int radius, int resolution, int centreX, int centreY, float observerHeight, float targetHeight, char *  inputFile,  char * outputFile);

//utility to do a lineOfSight programatically (for Python Bindings)
int doLoS(int resolution, int observerX, int observerY, int targetX, int targetY, float observerHeight, float targetHeight, char *  inputFile);

//do a viewshed
void viewshed();

//do a line of sight analysis
int lineOfSight();
