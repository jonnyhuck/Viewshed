/* File: viewshed.i */
%module viewshed

%{
#define SWIG_FILE_WITH_INIT
#include "viewshed.h"
%}

void doViewshed(int radius, int resolution, int centreX, int centreY, float observerHeight, float targetHeight, char *  inputFile,  char * outputFile);
int doLoS(int resolution, int observerX, int observerY, int targetX, int targetY, float observerHeight, float targetHeight, char *  inputFile);