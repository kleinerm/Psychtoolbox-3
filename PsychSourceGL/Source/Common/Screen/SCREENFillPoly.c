/*
	SCREENFillPoly.c		

	AUTHORS:

		Allen.Ingling@nyu.edu		awi 

	PLATFORMS:	
	
		Only OS X for now.


	HISTORY:

		07/23/04	awi		Created.
		10/12/04	awi		In useString: moved commas to inside [].
		01/12/05    mk      Added a slow-path that draws concave and self-intersecting polygons correctly.
		2/25/05		awi		Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
		

	TO DO:

	NOTES:

		It would be faster to use one call to glVertex2Dv instead of looping over glVertex2D except that we would have to reshape the matrix. 
		We have to reshape the matrix because glVertex assumes that values are stored in memory pairwise but MATLAB packs elements columnwise  
		and FillPolly assumes that a vertex is a row.  We could migrate to the new order except that 2x2 would be ambiguous.  If speed ever 
		becomes an issue then we could supply a preference to specify vertex columns instead of vertex rows and use glVertex2D.
*/


#include "Screen.h"

// Callback-Routines for the GLU-Tesselator functions used on the FillPoly - Slow - path:
void APIENTRY PsychtcbBegin(GLenum prim)
{
    glBegin(prim);
}

void APIENTRY PsychtcbVertex(void *data)
{
    glVertex3dv((GLdouble *)data);
}

void APIENTRY PsychtcbEnd(void)
{
    glEnd();
}

void APIENTRY PsychtcbCombine(GLdouble c[3], void *d[4], GLfloat w[4], void **out)
{
    GLdouble *nv = (GLdouble *) PsychMallocTemp((unsigned long) sizeof(GLdouble)*3);
    nv[0] = c[0];
    nv[1] = c[1];
    nv[2] = c[2];
    *out = nv;
}


// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('FillPoly', windowPtr [,color], pointList);";
//                                            1           2       3
static char synopsisString[] = 
	"Fill polygon. \"color\" is the clut index (scalar or [r g b a] vector) that you "
	"want to poke into each pixel; default produces white. \"pointList\" is a matrix: each row specifies the (x,y) "
	"coordinates of a vertex.";
	
static char seeAlsoString[] = "FramePoly";	

PsychError SCREENFillPoly(void)  
{
	
	PsychColorType				color;
	PsychWindowRecordType			*windowRecord;
	int					depthValue, whiteValue;
	int					i, mSize, nSize, pSize;
	boolean					isArgThere;
	double					*pointList;
        double                                  isConvex=-1;
	double*                                 tempv;
        static GLUtesselator                    *tess = NULL;
	int j,k;
	int flag = 0;
	double z;
	isConvex=0;

	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(3));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs

	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
	
	//Get the depth from the window, we need this to interpret the color argument.
	depthValue=PsychGetWindowDepthValueFromWindowRecord(windowRecord);
	

	//Get the color argument or use the default, then coerce to the form determened by the window depth.  
	isArgThere=PsychCopyInColorArg(2, FALSE, &color);
	if(!isArgThere){
		whiteValue=PsychGetWhiteValueFromWindow(windowRecord);
		PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
	}
 	PsychCoerceColorMode( &color);
        
	//get the list of pairs and validate.  
	PsychAllocInDoubleMatArg(3, kPsychArgRequired, &mSize, &nSize, &pSize, &pointList);
	if(nSize!=2)
		PsychErrorExitMsg(PsychError_user, "Width of point list must be 2");
	if(mSize<3)
		PsychErrorExitMsg(PsychError_user, "Polygons must consist of at leat 3 vertices; M dimension of pointList was <3");
	if(pSize>1)
		PsychErrorExitMsg(PsychError_user, "Polygons must consist of at leat 3 vertices; M dimension of pointList was <3");
	
	PsychSetGLContext(windowRecord);
	PsychUpdateAlphaBlendingFactorLazily(windowRecord);
		 
	PsychSetGLColor(&color, windowRecord);
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);

	///////// Test for convexity ////////
	// This algorithm checks, if the polygon is definitely convex, or not.
	// We take the slow-path, if polygon is non-convex or if we can't prove
	// that it is convex.
	//
	// Algorithm adapted from: http://astronomy.swin.edu.au/~pbourke/geometry/clockwise/
	// Which was written by Paul Bourke, 1998.
	//
	// -> This webpage explains the mathematical principle behind the test and provides
	// a C-Source file which has been adapted for use here.
	//	
	for (i=0;i<mSize;i++) {
		j = (i + 1) % mSize;
		k = (i + 2) % mSize;
		z  = (pointList[j] - pointList[i]) * (pointList[k+mSize] - pointList[j+mSize]);
		z -= (pointList[j+mSize] - pointList[i+mSize]) * (pointList[k] - pointList[j]);
		if (z < 0) {
			flag |= 1;
		}
		else if (z > 0) {
			flag |= 2;
		}
		
		if (flag == 3) {
			// This is definitely a CONCAVE polygon --> not Convex --> Take slow but safe path.
			isConvex = 0;
			break;
		}
	}
	
	if (flag != 0 && flag!=3) {
		// This is a convex polygon --> Take fast path.
		isConvex = 1;
	}
	else {
		// This is a complex polygon --> can't determine if it is convex or not --> Take slow but safe path.
		isConvex = 0;
	}
	
	////// Switch between fast path and slow path, depending on convexity of polygon:
	if (isConvex>0.5) {
	  // Convex, non-self-intersecting polygon - Take the fast-path:
	  //draw the rect
	  glBegin(GL_POLYGON);
	  for(i=0;i<mSize;i++) glVertex2d((GLdouble)pointList[i], (GLdouble)pointList[i+mSize]);
	  glEnd();
	}
	else {
	  // Possibly concave and/or self-intersecting polygon - At least we couldn't prove it is convex.
	  // Take the slow, but safe, path using GLU-Tesselators to break it up into a couple of convex, simple
	  // polygons:
	  
	  // Create and initialize a new GLU-Tesselator object:
	  tess = gluNewTess();
	  // Assign our callback-functions:
	  gluTessCallback(tess, GLU_TESS_BEGIN, PsychtcbBegin);
	  gluTessCallback(tess, GLU_TESS_VERTEX, PsychtcbVertex);
	  gluTessCallback(tess, GLU_TESS_END, PsychtcbEnd);
	  gluTessCallback(tess, GLU_TESS_COMBINE, PsychtcbCombine);

	  // We need to hold the values in a temporary array:
	  tempv=(double*) PsychMallocTemp(sizeof(double)*3*mSize);
	  
	  // Now submit our Polygon for tesselation:
	  gluTessBeginPolygon(tess, NULL);
	  gluTessBeginContour(tess);
	  for(i=0;i<mSize;i++) {
	    tempv[i*3]=(GLdouble) pointList[i];
	    tempv[i*3+1]=(GLdouble) pointList[i+mSize];
	    tempv[i*3+2]=0;
	    gluTessVertex(tess, (GLdouble*) &(tempv[i*3]), (void*) &(tempv[i*3]));
	  }            
	  gluTessEndContour(tess);
	  // Process, finalize and render it by calling our callback-functions:
	  gluTessEndPolygon (tess);
	  gluDeleteTess(tess);
	  tess=NULL;
	  
	  // Done with drawing the filled polygon. (Slow-Path)
	}
        
        // Mark end of drawing op. This is needed for single buffered drawing:
        PsychFlushGL(windowRecord);

	return(PsychError_none);
}

