
/*
SCREENDrawLines.c	
 
	AUTHORS:
 
		kas@princeton.edu				kas		Keith Schneider
		fcalabro@bu.edu					fjc		Finnegan Calabro
		mario.kleiner@tuebingen.mpg.de                  mk		Mario Kleiner
		dtaylor@ski.org					dgt		Douglas Taylor
 
	PLATFORMS:	
	
		Only OS X for now.  
 
	HISTORY:
	
		mm/dd/yy   
 
		12/13/04	kas		Created  
		12/17/04	fjc		added antialiased points
		12/17/04	kas		added dot_type flag
		1/11/04		awi		Merged into pychtoolbox.org distribution.
		1/01/05		mk		Performance optimizations and small bug-fixes
								- Fixes some minor bugs (or possible bugs, they would only
								trigger in some usage cases): GL-Calls were missing to undo
								parameter changes like glPointSize,
								glEnable(GL_POINT_SMOOTH). This can affect later
								invocations of DrawDots, e.g., Call DrawDots with point
								smoothing on --> Smoothing gets enabled, after that it
								would not be possible to disable point smoothing again -->
								any following DrawDots call would always render smooth
								points, regardless of setting.
								
								- Uses an optimized way of drawing the dots: Instead of
								looping over the 2-column vector of dots in a for-loop and
								submitting each Point via glVertex2d(), it now *requires* a
								2-row vector of dots as input and then renders all dots
								with one function call to glDrawArrays(). This is the most
								efficient way of passing a huge number of primitives
								(Points, rectangles, ...) to the OpenGL-System, avoiding
								for-loop and function call overhead and allowing possible
								optimizations by the graphics driver.								
		1/12/05		awi		Merged mk's improvements into psychtoolbox.org distribution.
		3/8/05		dgt		Created it from SCREENDrawDots.c
								- Uses the same vertex array method of drawing a lot of lines.
								Line drawing is not as optimised as point drawing but this
								method has the advantage of passing an rgb or rgba color
								array to use as the lines are drawn.
		3/23/05		dgt		Merged mk's improvements into SCREENDrawLines.c
                4/22/05         mk              Small bug fix (size = PsychMallocTemp.....)
		
 
 
 */


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('DrawLines', windowPtr, xy [,width] ,colors [,center] [,smooth]);";
//                                            1          2    3       4        5         6
static char synopsisString[] = 
"Quickly draw an array of lines using Vertex and Color arrays.  "
"\"xy\" is a two-row vector containing the x and y coordinates of the line ends, "
"relative to \"center\" (default center is [0 0]).  "
"\"size\" is the width of each line in pixels (default is 1).  "
"\"colors\" an array of rgb or rgba color data for each line "
"that you want to poke into each line (default is black).  "
"\"smooth\" is a flag that determines whether smoothing is on: "
"0 (default) no smoothing, 1 smoothing (with antialiasing) "
"If you use smoothing, you'll also need to set a proper blending mode "
"with Screen('BlendFunction').";  
static char seeAlsoString[] = "BlendFunction";	 

PsychError SCREENDrawLines(void)  
{

#ifdef OLDCOLOR	
	PsychColorType				color;		// To get rid of warnings;
	int					whiteValue;
#endif
	PsychWindowRecordType			*windowRecord;
	int					depthValue, colorPlaneSize, numColorPlanes, m,n,p, smooth;
	boolean                                 isArgThere;
	double					*xy, *size, *center, *dot_type;
	int                                     disable_at_exit = false;
	double                                  *colors;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(6));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
	
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
	
	//get size argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 3);
	if(!isArgThere){
            size = (double *) PsychMallocTemp(1 * sizeof(double));
            size[0] = 1;
	} else {
		PsychAllocInDoubleMatArg(3, TRUE, &m, &n, &p, &size);
		if(p!=1 || n!=1 || m!=1)
			PsychErrorExitMsg(PsychError_user, "size must be a scalar");
	}
	
	//Get the depth from the window, we need this to interpret the color argument.
	depthValue=PsychGetWindowDepthValueFromWindowRecord(windowRecord);
	numColorPlanes=PsychGetNumPlanesFromDepthValue(depthValue);
	colorPlaneSize=PsychGetColorSizeFromDepthValue(depthValue);

	PsychSetGLContext(windowRecord); 
	PsychUpdateAlphaBlendingFactorLazily(windowRecord);

#if OLDCOLOR	
	//Get the color argument or use the default, then coerce to the form determined by the window depth.  
	isArgThere=PsychCopyInColorArg(4, FALSE, &color);
	if(!isArgThere){
		whiteValue=PsychGetWhiteValueFromDepthValue(depthValue);
		PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
	}
 	PsychCoerceColorModeFromSizes(numColorPlanes, colorPlaneSize, &color);
	PsychSetGLColor(&color, depthValue);
#endif	
	// get center argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 5);
	if(!isArgThere){
		center = (double *) PsychMallocTemp(2 * sizeof(double));
		center[0] = 0;
		center[1] = 0;
	} else {
		PsychAllocInDoubleMatArg(5, TRUE, &m, &n, &p, &center);
		if(p!=1 || n!=2 || m!=1)
			PsychErrorExitMsg(PsychError_user, "center must be a 1-by-2 vector");
	}
	
	// get smooth argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 6);
	if(!isArgThere){
		smooth = 0;
	} else {
		PsychAllocInDoubleMatArg(6, TRUE, &m, &n, &p, &dot_type);
		smooth = (int) dot_type[0];
		if(p!=1 || n!=1 || m!=1 || (smooth!=0 && smooth!=1))
			PsychErrorExitMsg(PsychError_user, "smooth must be 0 or 1");
	}

        // Child-protection: Alpha blending needs to be enabled for smoothing to work:
        if (smooth>0 && windowRecord->actualEnableBlending!=TRUE) {
            PsychErrorExitMsg(PsychError_user, "Line smoothing doesn't work with alpha-blending disabled! See Screen('BlendFunction') on how to enable it.");
        }

	//get xy coordinates argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 2);
	if(!isArgThere){
		PsychErrorExitMsg(PsychError_user, "No xy argument supplied");
	}
	PsychAllocInDoubleMatArg(2, TRUE, &m, &n, &p, &xy);
	if(p!=1 || m!=2)
		PsychErrorExitMsg(PsychError_user, "xy must be a 2-row vector");
	
	//get colors coordinates argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 4);
	if(!isArgThere){
		PsychErrorExitMsg(PsychError_user, "No color array argument supplied");
	}
	PsychAllocInDoubleMatArg(4, TRUE, &m, &n, &p, &colors);
	if(p!=1 || m<3 || m>4)
		PsychErrorExitMsg(PsychError_user, "colors must be a 3- or 4-row vector");

#ifdef OLD
	(char *)colors = NewPtr(n*3*sizeof(float));
	for(i=0;i<n*3; )
	{
		colors[i++] = 1.0;//(float)Random()/ (float)32767;
		colors[i++] = 1.0;//(float)Random()/ (float)32767;
		colors[i++] = (float)i/(float)(n*3);	//Random()/ (float)32767;
	}
	m = 3;
#endif
	//draw lines
	// 1st make sure blending is enabled
	
/*	if( glIsEnabled(GL_BLEND) )
	{
		disable_at_exit = false;	// don't disable at exit
	}
	else
	{
		glEnable(GL_BLEND);			// turn it on and disable it at exit
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		disable_at_exit = true;	
	}
*/	
	// turn on antialiasing to draw circles	
	if(smooth) glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_TEXTURE_2D);

	// Set size of a single dot:
        glLineWidth(size[0]);

	// Setup modelview matrix to perform translation by 'center':
        glMatrixMode(GL_MODELVIEW);		// was incorrectly using GL_MODELVIEW_MATRIX

        // Make a backup copy of the matrix:
        glPushMatrix();

        // Apply a global translation of (center(x,y)) pixels to all following points:
        glTranslated(center[0], center[1],0);
        
        // Render the array of 2D-Points - Efficient version:
        // This command sequence allows fast processing of whole arrays
        // of vertices (or points, in this case). It saves the call overhead
        // associated with the original implementation below and is potentially
        // optimized in specific OpenGL implementations.
        
        // Pass a pointer to the start of the point-coordinate array:
        glVertexPointer(2, GL_DOUBLE, 0, &xy[0]);
         glColorPointer(m, GL_DOUBLE, 0, colors);
       // Enable fast rendering of arrays:
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        // Render all n points, starting at point 0, render them as POINTS:
        glDrawArrays(GL_LINES, 0, n);
        // Disable fast rendering of arrays:
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        
        // Old implementation: Loops over the array of points and submits each point
        // via a glVertex() call. The new solution above saves the overhead for the
        // for-loop and for the individual glVertex-calls.
        // glBegin(GL_POINTS);
	// for(i=0; i<2*n; i+=2) glVertex2d(xy[i], xy[i+1]);
	// glEnd();
	
        // Restore old matrix from backup copy, undoing the global translation:
        glPopMatrix();

        // turn off antialiasing again
		if(smooth) glDisable(GL_LINE_SMOOTH);

       // turn off blending if we turned it on
       //		if(disable_at_exit) glDisable(GL_BLEND);

        // Reset pointsize to 1.0
        glLineWidth(1);
       	//DisposePtr((char *)colors);
 
 	//All psychfunctions require this.
	return(PsychError_none);
	}
