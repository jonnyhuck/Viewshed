/* File: viewshed.cpp */

#include "viewshed.h"


//#################
// Globals
//#################

//output data and options
OutputData output;
Options program_options;


//#################
// Methods
//#################

/**
 * Sets up default options in Option struct;
 * TODO: should we lose this in favour of making all options required?
 */
void setupDefaultOptions(Options* op) {
	op->quiet = false;
	op->radius = 10000;
	op->resolution = 50;
	op->centerX = -1;
	op->centerY = -1;
	op->inputFileName = (char*)0; //null string
	op->outputFileName = (char*)0; //null string

	op->observerHeight = 50;
	op->targetHeight = 1.5;

	op->ax = 0;
	op->ay = 0;
	op->bx = 0;
	op->by = 0;

	op->areUsingPointToPoint = false;
}


/**
 * Prints out a given options struct
 */
void printOptions(Options* op) {
	printf("You are using the following options:\n");
	printf("Quiet mode: %s\n", op->quiet ? "true" : "false");
	printf("Radius: %i\n", op->radius);
	printf("Resolution: %i\n", op->resolution);

	//TODO: should put this back in. This function could do with a rework to change print if p2p mode is selected.
	// if(op->centerX == -1){
	//	  printf( "Center X: %s\n" : "Center X: %i\n");
	// }else{
	//
	// }
	// printf(op->centerX == -1 ? "Center X: %s\n" : "Center X: %i\n", op->centerX == -1 ?  "Unset" : op->centerX);
	// printf(op->centerY == -1 ? "Center Y: %s\n" : "Center Y: %i\n", op->centerY == -1 ?  "Unset" : op->centerY);
	printf("Target height: %g\n", op->targetHeight);
	printf("ObserverHeight: %g\n", op->observerHeight);
	printf("Input File: '%s'\n", op->inputFileName);
	printf("Output File: '%s'\n", op->inputFileName);
	printf("Ax: %i\n", op->ax);
	printf("Ay: %i\n", op->ay);
	printf("Bx: %i\n", op->bx);
	printf("By: %i\n", op->by);

}

/**
 * Prints out an output object (so we can see all of the variables)
 */
void printOutput(OutputData* out) {

	printf("Youre output data is as follows:\n");
	
	//everything here is stored in metres
	printf("minx: %i\n", out->minx);
	printf("miny: %i\n", out->miny);
	printf("maxx: %i\n", out->maxx);
	printf("maxy: %i\n", out->maxy);
	printf("centerx: %i\n", out->centerx);
	printf("centery: %i\n", out->centery);
	printf("radius: %i\n", out->radius);
	printf("width: %i\n", out->width);
	printf("height: %i\n", out->height);
	printf("resolution: %i\n", out->resolution);

	//everything here is stored in pixels!
	printf("pixelwidth: %i\n", out->pixelWidth);
	printf("pixelheight: %i\n", out->pixelHeight);
	printf("pixelminx: %i\n", out->pixelMinx);
	printf("pixelminy: %i\n", out->pixelMiny);
	printf("pixelmaxx: %i\n", out->pixelMaxx);
	printf("pixelmaxy: %i\n", out->pixelMaxy);
	printf("pixelradius: %i\n", out->pixelRadius);
	printf("pixelcenterx: %i\n", out->pixelCenterx);
	printf("pixelcentery: %i\n", out->pixelCentery);
}


/**
* Bilinear Interpolation
*
* x - the x coordinate of the current value
* y - the y coordinate of the current value
* br - value to the bottom right of the coordinate
* bl - value to the bottom left of the coordinate
* tl - value to the top left of the coordinate
* tr - value to the top right of the coordinate
* xr - the x coordinate for the values to the right of the coordinate
* xl - the x coordinate for the values to the left of the coordinate
* yt - the y coordinate for the values above the coordinate
* yb - the y coordinate for the values below the coordinate
*/
double bliniearInterp(int x, int y, double br, double bl, double tl, double tr, double xr, double xl, double yt, double yb) {
	// printf("%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",  x,  y,  br,  bl, tl,  tr,  xr,  xl,  yt,  yb);
	//bottom two x values
	double R1 = ((xr - x) / (xr - xl)) * bl + ((x - xl) / (xr - xl)) * br;

	//top two x values
	double R2 = ((xr - x) / (xr - xl)) * tl + ((x - xl) / (xr - xl)) * tr;

	//combine with y values
	double P = ((yt - y) / (yt - yb)) * R1 + ((y - yb) / (yt - yb)) * R2;

	//kept separate for now for convenience if we want to print out values
	//printf("DORP\n");
	return P;
}


/**
 * Return the value at a given pixel location
 */
double getHeightAt(OutputData* out, float* inputData, int pixelX, int pixelY) {
	
	//verify that the supplied pixel coordinates are appropriate
	if (pixelX < out->pixelMinx || pixelX > out->pixelMaxx || pixelY < out->pixelMiny || pixelY > out->pixelMaxy) {
		//printf("Out of bounds here!! %d %d %d %d %d %d %d  \n", pixelX,pixelY,out->pixelWidth, out->minx, out->maxx, out->miny, out->maxy);
		return 0;// TODO: This will minimise damage to calcs for now!  FIX ME!   -DBL_MAX; // a height of this means there is some kind of error!
	}
	else {
		//printf("I segfault here! %d \n",inputData[lineate(pixelX,pixelY,out->pixelWidth)]);
		return (double)inputData[lineate(pixelX, pixelY, out->pixelWidth)];
	}
}


/**
 * Return the value at a given coordinate location
 */
double getHeightAtMeters(OutputData* out, float* inputData, int mX, int mY) {
	if (mX < out->minx || mX >= out->maxx || mY < out->miny || mY >= out->maxy) {
		//printf("GOT ILLEGAL GHAM!!  %d %d  \n", mX, mY);
		return 0;
	}
	else {
		//printf("GOT GHAM!!  %d %d %d %d \n", mX, mY, coordinateToPixelX(out, mX), coordinateToPixelY(out, mY));
		return getHeightAt(out, inputData, coordinateToPixelX(out, mX), coordinateToPixelY(out, mY));
	}
}


/**
 * Works out the values required to pass to bliniear interpolation based upon the position
 *  within the cell.
 */
double getBliniearHeight(OutputData* out, float* inputData, int xmet, int ymet) {

	//x and y component - the distance into the pixel that the location is
	double xcomp = (fmod(xmet, out->resolution)) / out->resolution;
	double ycomp = (fmod(ymet, out->resolution)) / out->resolution;

	//the centre of the pixel containing the location
	double midInterval = (out->resolution / 2);
	double pixelCentreX = floor(xmet / midInterval) * midInterval;
	double pixelCentreY = floor(ymet / midInterval) * midInterval;

	//these just hold the coordinates of the values to pass to the interpolation
	double jlx, jrx, jty, jby;

	//do bilinear interpolation on nearest 4 cells - depends upon position in the cell
	if (xcomp < 0.5) {
		//left
		jrx = pixelCentreX;
		jlx = pixelCentreX - out->resolution;

		if (ycomp < 0.5) {
			//bottom
			jty = pixelCentreY;
			jby = pixelCentreY - out->resolution;

		}
		else if (ycomp > 0.5) {
			//top
			jty = pixelCentreY + out->resolution;
			jby = pixelCentreY;

		}
		else {
			//centre
			jty = pixelCentreY;
			jby = pixelCentreY;
		}

	}
	else if (xcomp > 0.5) {

		//right
		jrx = pixelCentreX + out->resolution;
		jlx = pixelCentreX;

		if (ycomp < 0.5) {
			//bottom
			jty = pixelCentreY;
			jby = pixelCentreY - out->resolution;

		}
		else if (ycomp > 0.5) {
			//top
			jty = pixelCentreY + out->resolution;
			jby = pixelCentreY;

		}
		else {  //centre
				//right centre
			jty = pixelCentreY;
			jby = pixelCentreY;
		}

	}
	else {

		//centre
		jrx = pixelCentreX;
		jlx = pixelCentreX;

		if (ycomp < 0.5) {  //bottom
							// centre bottom
			jty = pixelCentreY;
			jby = pixelCentreY - out->resolution;

		}
		else if (ycomp > 0.5) {     //top
									// centre top
			jty = pixelCentreY + out->resolution;
			jby = pixelCentreY;

		}
		else {
			//perfect centre, no need to interpolate
			return getHeightAtMeters(out, inputData, xmet, ymet); //heres your problem!!

		}
	}
	//TODO: Here really should be checking if any are equal to - dbl_max as it will for sure screw up our calculations..
	//get the interpolated value and return
	double height2 = bliniearInterp(xmet, ymet, getHeightAtMeters(out, inputData, jrx, jby), getHeightAtMeters(out, inputData, jlx, jby), getHeightAtMeters(out, inputData, jlx, jty), getHeightAtMeters(out, inputData, jrx, jty), jrx, jlx, jty, jby);
	return height2;
}


/**
 * Simple Pythagorean distance calculation
 */
float distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


/**
 * Converts a planar X coordinate to its corresponding pixel coordinate
 */
int coordinateToPixelX(OutputData* out, long x) {
	return (int)(x - out->minx) / out->resolution;
}


/**
 * Converts a planar Y coordinate to its corresponding pixel coordinate
 */
int coordinateToPixelY(OutputData* out, long y) {
	return (out->pixelHeight) - ((y - ((int)out->miny)) / out->resolution);
}


/**
 * Converts a 2D array position into its corresponding position in a 1D array
 */
int lineate(int x, int y, int width) {
	//printf("Accessing %d %d \n", x, y);
	return (y*width) + x; ;
}


/**
 * Runs a single ray-trace from one point to another point, return whether the end point is visible
 * TODO: Consoloidate with the above
 */
int doSingleRTPointToPoint(float* inputData, OutputData* data, int x1, int y1, int x2, int y2) {

	//printf("DATA %d %d %d %d \n", x1, y1, x2, y2);
	int deltax = abs(x2 - x1);
	int deltay = abs(y2 - y1);
	int count = 0; //this is how many pixels we are in to our ray.
	float initialHeight = 0;  //getHeightAt(fx,fy);
	float biggestDYDXSoFar = 0; //biggest peak so far
	float currentDYDX = 0; //current peak
	char visible = 0;   //used a char here, bool doesnt exist!
	float tempHeight = 0; //temp height used for offset comparisons.
	float distanceTravelled = 0;

	if (x1 < data->minx || x1 > data->maxx || y1 < data->miny || y1 > data->maxy) {
		printf("Illegal Coordinate1:%d,%d\n", x1, y1);
		printOutput(data);
	}
	if (x2 < data->minx || x2 > data->maxx || y2 < data->miny || y2 > data->maxy) {
		printf("Illegal Coordinate2:%d,%d\n", x2, y2);
		printOutput(data);
	}

	int x = x1;           // Start x off at the first pixel
	int y = y1;           // Start y off at the first pixel
	int xinc1;
	int xinc2;
	int yinc1;
	int yinc2;
	int curpixel;
	int den, num, numadd, numpixels;

	if (x2 >= x1)       // The x-values are increasing
	{
		xinc1 = data->resolution;
		xinc2 = data->resolution;
	}
	else              // The x-values are decreasing
	{
		xinc1 = -data->resolution;
		xinc2 = -data->resolution;
	}

	if (y2 >= y1)       // The y-values are increasing
	{
		yinc1 = data->resolution;
		yinc2 = data->resolution;
	}
	else              // The y-values are decreasing
	{
		yinc1 = -data->resolution;
		yinc2 = -data->resolution;
	}

	if (deltax >= deltay)   // There is at least one x-value for every y-value
	{
		xinc1 = 0;          // Don't change the x when numerator >= denominator
		yinc2 = 0;          // Don't change the y for every iteration
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;   // There are more x-values than y-values
	}
	else              // There is at least one y-value for every x-value
	{
		xinc2 = 0;          // Don't change the x for every iteration
		yinc1 = 0;          // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;   // There are more y-values than x-values
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel += data->resolution)
	{

			int x1pixel = coordinateToPixelX(data, (long)x);
		int y1pixel = coordinateToPixelY(data, (long)y);
	

		distanceTravelled = distance(x1, y1, x, y);

		//if we are on the first pixel (center of the circle)
		if (count == 0) {

			initialHeight = getBliniearHeight(data, inputData, x, y) + program_options.observerHeight; //set the initial height
			visible = 1;  //we of course can see ourselves


						  //we are on the second pixel
		}
		else if (count == 1) {

			biggestDYDXSoFar = (getBliniearHeight(data, inputData, x, y) - initialHeight) / distanceTravelled; //first angle we have come accross, so clearly the biggest.
			visible = 1; //again, obviously visible

						 //we are on any of the others
		}
		else {

			tempHeight = (getBliniearHeight(data, inputData, x, y) - initialHeight + program_options.targetHeight) / distanceTravelled;   //height of cell with offset

			currentDYDX = (getBliniearHeight(data, inputData, x, y) - initialHeight) / distanceTravelled;  //height of cell without offset

			if ((tempHeight - biggestDYDXSoFar) >= 0) {   //is the angle bigger than we have seen?
				visible = 1;
			}
			else {
				visible = 0;
			}

			if (currentDYDX >= biggestDYDXSoFar) {  //if this angle is greater than the biggest we have seen before, remember it.
				biggestDYDXSoFar = currentDYDX;  //note we are recording the height without the offset. Otherwise we would be raising the whole terrain by this amount rather than just this cell.
												 // that is it would affect all cells after this one.
			}
		}

		//increment the iterators
		count++; 
		num += numadd;        // Increase the numerator by the top of the fraction
		if (num >= den)     // Check if numerator >= denominator
		{
			num -= den;       // Calculate the new numerator value
			x += xinc1;       // Change the x as appropriate
			y += yinc1;       // Change the y as appropriate
		}
		x += xinc2;       // Change the x as appropriate
		y += yinc2;       // Change the y as appropriate
	}

	return visible;
}


/**
 * Runs a single ray-trace from one point to another point, set output data to 1 for each visible cell
 * TODO: Consoloidate with the above
 */
void doSingleRTMeters(OutputData* data, float* inputData, int x1, int y1, int x2, int y2) {
	//printf("DATA %d %d %d %d \n",x1,y1,x2,y2);
	int deltax = abs(x2 - x1);
	int deltay = abs(y2 - y1);


	int count = 0; //this is how many pixels we are in to our ray.
	float initialHeight = 0;  //getHeightAt(fx,fy);
	float biggestDYDXSoFar = 0; //biggest peak so far
	float currentDYDX = 0; //current peak
	char visible = 0;   //used a char here, bool doesnt exist!
	float tempHeight = 0; //temp height used for offset comparisons.
	float distanceTravelled = 0;

	if (x1 < data->minx || x1 > data->maxx || y1 < data->miny || y1 > data->maxy) {
		printf("Illegal Coordinate1:%d,%d\n", x1, y1);
		printOutput(data);
	}
	if (x2 < data->minx || x2 > data->maxx || y2 < data->miny || y2 > data->maxy) {
		printf("Illegal Coordinate2:%d,%d\n", x2, y2);
		printOutput(data);
	}

	int x = x1;           // Start x off at the first pixel
	int y = y1;           // Start y off at the first pixel
	int xinc1;
	int xinc2;
	int yinc1;
	int yinc2;
	int curpixel;
	int den, num, numadd, numpixels;

	if (x2 >= x1)       // The x-values are increasing
	{
		xinc1 = data->resolution;
		xinc2 = data->resolution;
	}
	else              // The x-values are decreasing
	{
		xinc1 = -data->resolution;
		xinc2 = -data->resolution;
	}

	if (y2 >= y1)       // The y-values are increasing
	{
		yinc1 = data->resolution;
		yinc2 = data->resolution;
	}
	else              // The y-values are decreasing
	{
		yinc1 = -data->resolution;
		yinc2 = -data->resolution;
	}

	if (deltax >= deltay)   // There is at least one x-value for every y-value
	{
		xinc1 = 0;          // Don't change the x when numerator >= denominator
		yinc2 = 0;          // Don't change the y for every iteration
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;   // There are more x-values than y-values
	}
	else              // There is at least one y-value for every x-value
	{
		xinc2 = 0;          // Don't change the x for every iteration
		yinc1 = 0;          // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;   // There are more y-values than x-values
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel += data->resolution)
	{

		int x1pixel = coordinateToPixelX(data, (long)x);
		int y1pixel = coordinateToPixelY(data, (long)y);
		
		distanceTravelled = distance(x1, y1, x, y);

		//if we are on the first pixel (center of the circle)
		if (count == 0) {

			initialHeight = getBliniearHeight(data, inputData, x, y) + program_options.observerHeight; //set the initial height
			visible = 1;  //we of course can see ourselves


						  //we are on the second pixel
		}
		else if (count == 1) {

			biggestDYDXSoFar = (getBliniearHeight(data, inputData, x, y) - initialHeight) / distanceTravelled; //first angle we have come accross, so clearly the biggest.
			visible = 1; //again, obviously visible

						 //we are on any of the others
		}
		else {

			tempHeight = (getBliniearHeight(data, inputData, x, y) - initialHeight + program_options.targetHeight) / distanceTravelled;   //height of cell with offset

			currentDYDX = (getBliniearHeight(data, inputData, x, y) - initialHeight) / distanceTravelled;  //height of cell without offset

			if ((tempHeight - biggestDYDXSoFar) >= 0) {   //is the angle bigger than we have seen?
				visible = 1;
			}
			else {
				visible = 0;
			}

			if (currentDYDX >= biggestDYDXSoFar) {  //if this angle is greater than the biggest we have seen before, remember it.
				biggestDYDXSoFar = currentDYDX;  //note we are recording the height without the offset. Otherwise we would be raising the whole terrain by this amount rather than just this cell.
												 // that is it would affect all cells after this one.
			}
		}

		count++; //increment outselves along the line
				 // printf ("Current blin height %.6f \n", getBliniearHeight(data, inputData, x ,y) - initialHeight);
				 // printf("Biggest: %.6f, Current %.6f \n", biggestDYDXSoFar, currentDYDX);
		if (visible == 1) { //if we are visible, mark it in the output data.
			data->data[lineate(x1pixel, y1pixel, data->pixelWidth)] = 1;//
		}

		//update iterators
		num += numadd;        // Increase the numerator by the top of the fraction
		if (num >= den)     // Check if numerator >= denominator
		{
			num -= den;       // Calculate the new numerator value
			x += xinc1;       // Change the x as appropriate
			y += yinc1;       // Change the y as appropriate
		}
		x += xinc2;       // Change the x as appropriate
		y += yinc2;       // Change the y as appropriate
	}
}


/**
 * Offset a X coordinate by a given distance and direction
 */
double pointOffsetX(double x, double  y, double distance, double azimuth) {
	//simple geometric offset in each dimension
	return x + (sin((azimuth)) * distance);
}


/**
 * Offset a Y coordinate by a given distance and direction
 */
double pointOffsetY(double  x, double  y, double distance, double azimuth) {
	//simple geometric offset in each dimensions
	return y + (cos((azimuth)) * distance);
}


/**
 * Do a bunch of (360 deg) RTs from pixel to radius.
 */
void doRTCalc(OutputData* out, float* inputData) {

	//printf("Start (origins) %d %d %d \n", xOrigin,yOrigin, radius); //this is center not top left... i think.

	//the location of thie start of the raytraces
	int startX = out->centerx;
	int startY = out->centery;

	//the arc distance of the resolution in radians (to increment the rays)
	double angle = (TWOPI / (TWOPI*out->radius) * out->resolution) / 1.414213562;

	//move around a circle incrementing by the above
	double d;
	for (d = 0; d < TWOPI; d += angle) {

		//work out the end of the ray
		double endX = pointOffsetX(startX, startY, out->radius, d);
		double endY = pointOffsetY(startX, startY, out->radius, d);

		//do a ray trace
		doSingleRTMeters(out, inputData, startX, startY, endX, endY);
	}
}


/**
 * Print out some helpful tips for the user
 */
void printHelpInfo() {

	printf("Available options are:\n");
	printf("--verbose or --brief : turn on/disable verbose mode.\n");
	printf("--radius <value> or -r <value> : set the output radius.\n");
	printf("--centerX <value> or -x <value> : set the x position of the viewer.\n");
	printf("--centerY <value> or -y <value> : set the y position of the viewer.\n");
	printf("--resolution <value> or -z <value> : set the resolution of the view data.\n");
	printf("--observerheight <value> or -o <value> : set the height of the observer (centre).\n");
	printf("--targetheight <value> or -t <value> : set the height of the target (everywhere).\n");

	printf("--pointtopoint  or -p : enable point to point mode (requires ax,ay,bx,by).\n");
	printf("--pointtopointax <value> or -j <value> : set pointtopointmode ax value.\n");
	printf("--pointtopointay <value> or -k <value> : set pointtopointmode ax value.\n");
	printf("--pointtopointbx <value> or -l <value> : set pointtopointmode ax value.\n");
	printf("--pointtopointby <value> or -m <value> : set pointtopointmode ax value.\n");

	printf("--inputfile <value> or -i <value> : input file name (geotiff).\n");
	printf("--outputfile <value> or -f <value> : output file name (geotiff).\n");
}


/**
 * Perform line of sight analysis based upon the options already loaded into memory
 * JJH:
 */
void viewshed() {

	//register all data drivers
	GDALAllRegister();

	//open the file as read only
	GDALDatasetH  hDataset;

	//open the input (DEM) file
	hDataset = GDALOpenShared(program_options.inputFileName, GA_ReadOnly);

	//if data has come in
	if (hDataset != NULL) {
		//init some variables
		GDALRasterBandH hBand;
		int             nBlockXSize, nBlockYSize;
		int             bGotMin, bGotMax;
		double          adfMinMax[2];
		float   *pafScanline;

		//get the band from the raster (there will only be one)
		hBand = GDALGetRasterBand(hDataset, 1);
		GDALGetBlockSize(hBand, &nBlockXSize, &nBlockYSize);

		//get min and max value
		adfMinMax[0] = GDALGetRasterMinimum(hBand, &bGotMin);
		adfMinMax[1] = GDALGetRasterMaximum(hBand, &bGotMax);
		if (!(bGotMin && bGotMax))
			GDALComputeRasterMinMax(hBand, TRUE, adfMinMax);

		//get the dimensions of the band
		int   nXSize = GDALGetRasterBandXSize(hBand);
		int   nYSize = GDALGetRasterBandYSize(hBand);

		//get the transforminfo
		double  adfGeoTransform[6];
		if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None) {
				
			//TODO: should we take come action here...?
		
			//    if (!program_options.quiet) printf( "Origin = (%.6f,%.6f)\n",
			//  adfGeoTransform[0], adfGeoTransform[3] );
			//    if (!program_options.quiet)  printf( "Pixel Size = (%.6f,%.6f)\n",
			//  adfGeoTransform[1], adfGeoTransform[5] );
		}

		//get the origin
		float topLeftXinputData = adfGeoTransform[0];
		float topLeftYinputData = adfGeoTransform[3];

		//get input values in metres
		output.minx = program_options.centerX - program_options.radius;
		output.maxx = program_options.centerX + program_options.radius;
		output.miny = program_options.centerY - program_options.radius;
		output.maxy = program_options.centerY + program_options.radius;
		output.radius = program_options.radius;
		output.centerx = program_options.centerX;
		output.centery = program_options.centerY;
		output.width = output.maxx - output.minx;
		output.height = output.maxy - output.miny;
	
		//get input values in pixels
		output.pixelWidth = (output.width) / program_options.resolution;		//(width in pixels) TODO: better terminology!
		output.pixelHeight = (output.height) / program_options.resolution;		//(heightin pixels) TODO: better terminology!
		output.pixelMinx = 0;
		output.pixelMaxx = output.width / program_options.resolution;
		output.pixelMiny = 0;
		output.pixelMaxy = output.height / program_options.resolution;
		output.pixelRadius = program_options.radius / program_options.resolution;
		output.pixelCenterx = (output.pixelMaxx / 2);
		output.pixelCentery = (output.pixelMaxy / 2);
		output.resolution = program_options.resolution;

		//get the distance from target centre to the top left corner
		float distance = sqrt(pow(output.radius, 2) + pow(output.radius, 2));

		//offset the point to that location (5.497787144 = 315 degrees in radians) whilst converting to pixels
		float offsetX = (program_options.centerX + (sin(5.497787144) * distance) - topLeftXinputData) / program_options.resolution;
		float offsetY = -(program_options.centerY + (cos(5.497787144) * distance) - topLeftYinputData) / program_options.resolution;
	
		//reserve some memory for the section of data that we are interested in
		pafScanline = (float *)CPLMalloc(sizeof(float)*(output.pixelWidth)*(output.pixelHeight));

		//open the section of the raster that we want to read
		GDALRasterIO(hBand, GF_Read, offsetX, offsetY, output.pixelWidth, output.pixelHeight,
			pafScanline, output.pixelWidth, output.pixelHeight, GDT_Float32,
			0, 0);

		//now we create a new file.
		GDALDriverH   hDriver = GDALGetDriverByName("GTiff");
		GDALDatasetH hDstDS;
		char **papszOptions = NULL;
		if (program_options.outputFileName == (void *)0) {
			hDstDS = GDALCreate(hDriver, "output.tif", output.pixelWidth, output.pixelHeight, 1, GDT_Byte,
				papszOptions);
			printf("NO output file name specified. Defaulting to 'output.tif'");
		}
		else {
			hDstDS = GDALCreate(hDriver, program_options.outputFileName, output.pixelWidth, output.pixelHeight, 1, GDT_Byte,
				papszOptions);
		}

		//set the transform params for the output file
		double adfGeoTransformO[6] = { output.minx, program_options.resolution, 0,  output.maxy, 0, -program_options.resolution };
		GDALSetGeoTransform(hDstDS, adfGeoTransformO);

		//set to OSGB (limits to GB for now)
		OGRSpatialReferenceH hSRS;
		char *pszSRS_WKT = NULL;
		hSRS = OSRNewSpatialReference(NULL);
		OSRImportFromProj4(hSRS, "+proj=tmerc +lat_0=49 +lon_0=-2 +k=0.9996012717 +x_0=400000 +y_0=-100000 +ellps=airy +units=m +no_defs");
		OSRExportToWkt(hSRS, &pszSRS_WKT);
		OSRDestroySpatialReference(hSRS);
		GDALSetProjection(hDstDS, pszSRS_WKT);
		CPLFree(pszSRS_WKT);

		//load a blank array into the output object
		output.data = (GByte *)CPLMalloc(sizeof(GByte)*(output.pixelWidth)*(output.pixelHeight));
		memset(output.data, 0, sizeof(GByte)*(output.pixelWidth)*(output.pixelHeight));

		//run calcs on the input data, saving results into output data
		doRTCalc(&output, pafScanline);

		//save output data into file
		GDALRasterBandH hBandO;
		hBandO = GDALGetRasterBand(hDstDS, 1);
		GDALRasterIO(hBandO, GF_Write, 0, 0, output.pixelWidth, output.pixelHeight,
			output.data, output.pixelWidth, output.pixelHeight, GDT_Byte, 0, 0);

		//Once we're done, close properly the dataset
		GDALClose(hDstDS);

		//free up the buffer
		CPLFree(pafScanline);
		CPLFree(output.data);

	} else {  //(if no data was loaded)
		printf(CPLGetLastErrorMsg());
	}
}


/**
 * Perform line of sight analysis  based upon the options already loaded into memory
 * JJH:
 */
int lineOfSight() {
	
	//register all data drivers
	GDALAllRegister();

	//open the file as read only
	GDALDatasetH  hDataset;

	//open the input (DEM) file
	hDataset = GDALOpenShared(program_options.inputFileName, GA_ReadOnly);

	//if data has come in
	if (hDataset != NULL) {

		//init some variables
		GDALRasterBandH hBand;
		int             nBlockXSize, nBlockYSize;
		int             bGotMin, bGotMax;
		double          adfMinMax[2];
		float   *pafScanline;

		//get the band from the raster (there will only be one)
		hBand = GDALGetRasterBand(hDataset, 1);
		GDALGetBlockSize(hBand, &nBlockXSize, &nBlockYSize);

		//get min and max value
		adfMinMax[0] = GDALGetRasterMinimum(hBand, &bGotMin);
		adfMinMax[1] = GDALGetRasterMaximum(hBand, &bGotMax);
		if (!(bGotMin && bGotMax))
			GDALComputeRasterMinMax(hBand, TRUE, adfMinMax);

		//get the dimensions of the band
		int   nXSize = GDALGetRasterBandXSize(hBand);
		int   nYSize = GDALGetRasterBandYSize(hBand);

		//get the transforminfo
		double  adfGeoTransform[6];
		if (GDALGetGeoTransform(hDataset, adfGeoTransform) == CE_None) {
		
			//TODO: should we take come action here...?
		
			//    if (!program_options.quiet) printf( "Origin = (%.6f,%.6f)\n",
			//  adfGeoTransform[0], adfGeoTransform[3] );
			//    if (!program_options.quiet)  printf( "Pixel Size = (%.6f,%.6f)\n",
			//  adfGeoTransform[1], adfGeoTransform[5] );
		}

		//get input values in metres
		output.resolution = program_options.resolution;
		output.width = nXSize*output.resolution;
		output.height = nYSize*output.resolution;
		output.minx = adfGeoTransform[0];
		output.maxx = adfGeoTransform[0] + (nXSize * output.resolution);
		output.miny = adfGeoTransform[3] - (nYSize * output.resolution);
		output.maxy = adfGeoTransform[3];
	
		//get input values in pixels
		output.pixelWidth = nXSize;		//(width in pixels) TODO: better terminology!
		output.pixelHeight = nYSize;	//(height in pixels) TODO: better terminology!
		output.pixelMinx = 0;
		output.pixelMaxx = nXSize;
		output.pixelMiny = 0;
		output.pixelMaxy = nYSize;

		//reserve some memory for the section of data that we are interested in
		pafScanline = (float *)CPLMalloc(sizeof(float)*(nXSize)*(nYSize));

		//read the whole thing in..
		GDALRasterIO(hBand, GF_Read, 0, 0, nXSize, nYSize,
			pafScanline, nXSize, nYSize, GDT_Float32,
			0, 0);

		//do LoS analysis...
		int completelyVisible1 = doSingleRTPointToPoint(pafScanline, &output, program_options.ax, program_options.ay, program_options.bx, program_options.by);
	
		//JJH: ...then do it again in the opposite direction - this is to remove false negatives in comparison with the viewshed (different Bresenham lines give different results)
		//TODO: Should this be optional?
		int completelyVisible2 = doSingleRTPointToPoint(pafScanline, &output, program_options.bx, program_options.by, program_options.ax, program_options.ay);

		//free up the buffer
		CPLFree(pafScanline);

		//combine the results and return
		int completelyVisible = (completelyVisible1 + completelyVisible2) > 0 ? 1 : 0;
		return completelyVisible;
	
		//TODO: Add in the option to have the output (line) file

	} else {  //(if no data was loaded)
		printf(CPLGetLastErrorMsg());
	}
	return -1;
}

/**
 * This is a method to programatically call the viewshed
 * Utility method for the Python Bindings
 * JJH:
 */
void doViewshed(int radius, int resolution, int centreX, int centreY, float observerHeight, float targetHeight, char *  inputFile,  char * outputFile){
	
	//populate options
	program_options.radius = radius;
	program_options.resolution = resolution;
	program_options.centerX = centreX;
	program_options.centerY = centreY;
	program_options.observerHeight = observerHeight;
	program_options.targetHeight = targetHeight;
	program_options.inputFileName = inputFile;
	program_options.outputFileName = outputFile;
		
	//call viewshed
	viewshed();
}

/**
 * This is a method to programatically call the line of sight analysis
 * Utility method for the Python Bindings
 * JJH:
 */
int doLoS(int resolution, int observerX, int observerY, int targetX, int targetY, float observerHeight, float targetHeight, char *  inputFile){
	
	//populate options
	program_options.areUsingPointToPoint = true;
	program_options.resolution = resolution;
	program_options.ax = observerX;
	program_options.ay = observerY;
	program_options.bx = targetX;
	program_options.by = targetY;
	program_options.observerHeight = observerHeight;
	program_options.targetHeight = targetHeight;
	program_options.inputFileName = inputFile;
	
	//call line of sight
	int los = lineOfSight();
	return los;
}

/**
 * Main method - process arguments, populate structs and start necessary calculations
 * This only runs in the command line version
 */
int main(int argc, char **argv) {

	//init random number generator
	srand(time(NULL));

	//setup defaults
	setupDefaultOptions(&program_options);

	while (1) {
	
		struct option long_options[] =
		{
			/* These options set a flag. */
			{ "quiet", no_argument, 0, 'q' },
			{ "radius",     required_argument, 0, 'r' },
			{ "resolution",     required_argument, 0, 'z' },
			{ "centerX",  required_argument, 0, 'x' },
			{ "centerY",  required_argument, 0, 'y' },
			{ "observerheight",  required_argument, 0, 'o' },
			{ "targetheight",  required_argument, 0, 't' },
			{ "inputfile",  required_argument, 0, 'i' },
			{ "outputfile",  required_argument, 0, 'f' },
			{ "pointtopoint",  no_argument, 0, 'p' },
			{ "pointtopointax",  required_argument, 0, 'j' },
			{ "pointtopointay",  required_argument, 0, 'k' },
			{ "pointtopointbx",  required_argument, 0, 'l' },
			{ "pointtopointby",  required_argument, 0, 'm' },
			{ 0, 0, 0, 0 }
		};

		//getopt_long stores the option index here.
		int option_index = 0;
		int c = getopt_long(argc, argv, "r:z:x:y:o:t:i:f:hqpj:k:l:m:",
			long_options, &option_index);

		// Detect the end of the options.
		if (c == -1)
			break;

		//sort arguments and store as required
		switch (c) {
			case 'r':
				//  printf ("option -r with value `%s'\n", optarg);
				program_options.radius = atoi(optarg);
				break;

			case 'q':
				program_options.quiet = 1;
				break;

			case 'z':
				//  printf ("option -z with value `%s'\n", optarg);
				program_options.resolution = atoi(optarg);
				break;

			case 'x':
				// printf ("option -x with value `%s'\n", optarg);
				program_options.centerX = atoi(optarg);
				break;

			case 'y':
				//  printf ("option -y with value `%s'\n", optarg);
				program_options.centerY = atoi(optarg);
				break;

			case 'o':
				// printf ("option -v with value `%s'\n", optarg);
				program_options.observerHeight = atof(optarg);
				break;
			case 't':
				// printf ("option -v with value `%s'\n", optarg);
				program_options.targetHeight = atof(optarg);
				break;
			case 'f':
				// printf ("option -f with value `%s'\n", optarg);
				program_options.outputFileName = optarg;
				break;
			case 'i':
				// printf ("option -f with value `%s'\n", optarg);
				program_options.inputFileName = optarg;
				break;
			case 'h':
				printHelpInfo();
				break;
			case 'p':
				program_options.areUsingPointToPoint = true;
				break;
			case 'j':
				program_options.ax = atoi(optarg);
				break;
			case 'k':
				program_options.ay = atoi(optarg);
				break;
			case 'l':
				program_options.bx = atoi(optarg);
				break;
			case 'm':
				program_options.by = atoi(optarg);
				break;
			case '?':
				printf("Sorry, I cant understand one or more of your options.\n");
				printHelpInfo();
				/* getopt_long already printed an error message. */
				break;
			default:
				abort();
		}
	}

	//verify that we actually have a file name
	if (program_options.inputFileName == (void *)0) {
		printf("No input data specified!. Cowardly aborting..\n");
		exit(1);
	}
	
	/* Now we have the data, do the analysis */

	//are we using line of sight or viewshed?
	if (program_options.areUsingPointToPoint == true) { 	//LoS
	
		//run the Line of Sight and print out the result
		int vis = lineOfSight();
		printf("%i\n", vis);
		
	} else {												//viewshed
	
		//run the viewshed, writing the result to file
		viewshed();
	}
	return 0;
}