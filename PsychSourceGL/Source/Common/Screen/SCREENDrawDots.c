/*
	SCREENDrawDots.c	
 
	AUTHORS:
 
		kas@princeton.edu				kas		Keith Schneider
		fcalabro@bu.edu					fjc		Finnegan Calabro
		mario.kleiner@tuebingen.mpg.de                  mk		Mario Kleiner
 
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
                1/30/05         mk              Bug-fixes: Adding PsychMallocTemp()'s and a few checks to avoid memory corruption
                                                crashes.
                2/25/05         awi             Added call to PsychUpdateAlphaBlendingFactorLazily().  Drawing now obeys settings by Screen('BlendFunction').
                3/22/05         mk              Added possibility to spec vectors with individual color and size spec per dot.
                4/29/05         mk              Bugfix for color vectors: They should also take values in range 0-255 instead of 0.0-1.0.
							
		
 
	TO DO:
 
		figure out easy way to antialias rectangles 
 
 */


#include "Screen.h"

// If you change the useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "Screen('DrawDots', windowPtr, xy [,size] [,color] [,center] [,dot_type]);";
//                                            1          2    3       4        5         6
static char synopsisString[] = 
"Quickly draw an array of dots.  "
"\"xy\" is a two-row vector containing the x and y coordinates of the dot centers, "
"relative to \"center\" (default center is [0 0]).  "
"\"size\" is the width of each dot in pixels (default is 1). "
"Instead of a common size for all dots you can also provide a "
"vector which defines a different dot size for each dot.  "
"\"color\" is the the clut index (scalar or [r g b a] vector) "
"that you want to poke into each dot pixel (default is black).  "
"Instead of a single \"color\" you can also provide a 3 or 4 row vector,"
"which specifies an individual RGB or RGBA color for each corresponding point."
"\"dot_type\" is a flag that determines what type of dot is drawn: "
"0 (default) squares, 1 circles (with anti-aliasing), 2 circles (with high-quality "
"anti-aliasing, if supported by your hardware). "
"If you use dot_type = 1 you'll also need to set a proper blending mode with the "
"Screen('BlendFunction') command!";  
static char seeAlsoString[] = "BlendFunction";	 

PsychError SCREENDrawDots(void)  
{
	
	PsychColorType				color;
	PsychWindowRecordType                   *windowRecord;
	int                                     depthValue, whiteValue, colorPlaneSize, numColorPlanes, m,n,p,mc,nc,pc,idot_type;
        int                                     i, nrpoints, nrsize;
	boolean                                 isArgThere, usecolorvector;
	double					*xy, *size, *center, *dot_type, *colors, *tmpcolors, *pcolors, *tcolors;
	float pointsizerange[2];
	const double convfactor = 1/255.0;
    
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString,seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check for superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(6));   //The maximum number of inputs
	PsychErrorExit(PsychCapNumOutputArgs(0));  //The maximum number of outputs
	
	//get the window record from the window record argument and get info from the window record
	PsychAllocInWindowRecordArg(1, kPsychArgRequired, &windowRecord);
		
	//get xy coordinates argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 2);
	if(!isArgThere) {
            PsychErrorExitMsg(PsychError_user, "No xy argument supplied");
	}
	PsychAllocInDoubleMatArg(2, TRUE, &m, &n, &p, &xy);
	if(p!=1 || m!=2) PsychErrorExitMsg(PsychError_user, "xy must be a 2-row vector");
        nrpoints=n;
        
	//get size argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 3);
	if(!isArgThere){
            // No size provided: Use a default size of 1.0:
            size = (double *) PsychMallocTemp(sizeof(double));
            size[0] = 1;
            nrsize=1;
	} else {
            PsychAllocInDoubleMatArg(3, TRUE, &m, &n, &p, &size);
            if(p!=1) PsychErrorExitMsg(PsychError_user, "Size must be a scalar or a vector with one column or row");
            nrsize=m*n;
            if (nrsize!=nrpoints && nrsize!=1) PsychErrorExitMsg(PsychError_user, "Size vector must contain one size value per point");
	}
        
	//Get the depth from the window, we need this to interpret the color argument.
	depthValue=PsychGetWindowDepthValueFromWindowRecord(windowRecord);
	numColorPlanes=PsychGetNumPlanesFromDepthValue(depthValue);
	colorPlaneSize=PsychGetColorSizeFromDepthValue(depthValue);
	
        // Check if color argument is provided:
        isArgThere = PsychIsArgPresent(PsychArgIn, 4);        
	if(!isArgThere){
            // No color argument provided - Use defaults:
            whiteValue=PsychGetWhiteValueFromDepthValue(depthValue);
            PsychLoadColorStruct(&color, kPsychIndexColor, whiteValue ); //index mode will coerce to any other.
            usecolorvector=false;
	}
        else {
            // Some color argument provided. Check first, if it's a valid color vector:
            PsychAllocInDoubleMatArg(4, TRUE, &mc, &nc, &pc, &colors);
            // Do we have a color vector, aka one element per vertex?
            if(pc==1 && nc==nrpoints && nrpoints>1) {
                // Looks like we might have a color vector... ... Double-check it:
                if (mc!=3 && mc!=4) PsychErrorExitMsg(PsychError_user, "Color vector must be a 3 or 4 row vector");
                // Yes. colors is a valid pointer to it.
                usecolorvector=true;
                
                // We have to loop through the vector and divide all values by 255, so the input values
                // 0-255 get mapped to the range 0.0-1.0, as OpenGL expects values in range 0-1 when
                // a color vector is passed in Double- or Float format.
                // This is inefficient, as it burns some cpu-cycles, but necessary to keep color
                // specifications consistent in the PTB - API.
                tmpcolors=PsychMallocTemp(sizeof(double) * nc * mc);
                pcolors = colors;
                tcolors = tmpcolors;
                for (i=0; i<(nc*mc); i++) {
                    *(tcolors++)=(*pcolors++) * convfactor;
                }
            }
            else {
                // No color vector provided: Check for a single valid color triplet or quadruple:
                usecolorvector=false;
                isArgThere=PsychCopyInColorArg(4, TRUE, &color);                
            }
        }
        
	PsychSetGLContext(windowRecord);
        // Enable this windowRecords framebuffer as current drawingtarget:
        PsychSetDrawingTarget(windowRecord);
	PsychUpdateAlphaBlendingFactorLazily(windowRecord);

	
 	// Set up common color for all dots if no color vector has been provided:
        if (!usecolorvector) {
            PsychCoerceColorModeFromSizes(numColorPlanes, colorPlaneSize, &color);
            PsychSetGLColor(&color, depthValue);
	}
        
	// get center argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 5);
	if(!isArgThere){
                center = (double *) PsychMallocTemp(2 * sizeof(double));
                center[0] = 0;
                center[1] = 0;
	} else {
		PsychAllocInDoubleMatArg(5, TRUE, &m, &n, &p, &center);
		if(p!=1 || n!=2 || m!=1) PsychErrorExitMsg(PsychError_user, "center must be a 1-by-2 vector");
	}
	
	// get dot_type argument
	isArgThere = PsychIsArgPresent(PsychArgIn, 6);
	if(!isArgThere){
		idot_type = 0;
	} else {
		PsychAllocInDoubleMatArg(6, TRUE, &m, &n, &p, &dot_type);
		if(p!=1 || n!=1 || m!=1 || (dot_type[0]<0 || dot_type[0]>2))
			PsychErrorExitMsg(PsychError_user, "dot_type must be 0, 1 or 2");
		idot_type = (int) dot_type[0];
	}
		
        // Child-protection: Alpha blending needs to be enabled for smoothing to work:
        if (idot_type>0 && windowRecord->actualEnableBlending!=TRUE) {
            PsychErrorExitMsg(PsychError_user, "Point smoothing doesn't work with alpha-blending disabled! See Screen('BlendFunction') on how to enable it.");
        }
	
        //draw dots

	// turn on antialiasing to draw circles	
	if(idot_type) {
	  glEnable(GL_POINT_SMOOTH);
	  glGetFloatv(GL_POINT_SIZE_RANGE, &pointsizerange);
	  // A dot type of 2 requests for highest quality point smoothing:
	  glHint(GL_POINT_SMOOTH_HINT, (idot_type>1) ? GL_NICEST : GL_DONT_CARE);
	}
	else {
	  #ifndef GL_ALIASED_POINT_SIZE_RANGE
	  #define GL_ALIASED_POINT_SIZE_RANGE 0x846D
	  #endif

	  glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &pointsizerange);
	}

	// Set size of a single dot:
	if (size[0] > pointsizerange[1] || size[0] < pointsizerange[0]) {
	  printf("PTB-ERROR: You requested a point size of %f units, which is not in the range (%f to %f) supported by your graphics hardware.\n",
		 size[0], pointsizerange[0], pointsizerange[1]);
	  PsychErrorExitMsg(PsychError_user, "Unsupported point size requested in Screen('DrawDots').");
	}

        glPointSize(size[0]);

	// Setup modelview matrix to perform translation by 'center':
        glMatrixMode(GL_MODELVIEW);

        // Make a backup copy of the matrix:
        glPushMatrix();

        // Apply a global translation of (center(x,y)) pixels to all following points:
        glTranslated(center[0], center[1], 0);
        
        // Render the array of 2D-Points - Efficient version:
        // This command sequence allows fast processing of whole arrays
        // of vertices (or points, in this case). It saves the call overhead
        // associated with the original implementation below and is potentially
        // optimized in specific OpenGL implementations.
        
        // Pass a pointer to the start of the point-coordinate array:
        glVertexPointer(2, GL_DOUBLE, 0, &xy[0]);
        
        // Enable fast rendering of arrays:
        glEnableClientState(GL_VERTEX_ARRAY);

        if (usecolorvector) {
            glColorPointer(mc, GL_DOUBLE, 0, tmpcolors);
            glEnableClientState(GL_COLOR_ARRAY);
        }
        
        // Render all n points, starting at point 0, render them as POINTS:
        if (nrsize==1) {
            // One common point size for all dots provided. Good! This is very efficiently
            // done with one single render-call:
            glDrawArrays(GL_POINTS, 0, nrpoints);
        }
        else {
            // Different size for each dot provided: We have to do One GL - call per dot.
            // This is *pretty inefficient* and should be reimplemented in the future via
            // Point-Sprite extensions, cleverly used display lists or via vertex-shaders...
            // For now we do it the stupid way:
            for (i=0; i<nrpoints; i++) {
	      if (size[i] > pointsizerange[1] || size[i] < pointsizerange[0]) {
		printf("PTB-ERROR: You requested a point size of %f units, which is not in the range (%f to %f) supported by your graphics hardware.\n",
		       size[i], pointsizerange[0], pointsizerange[1]);
		PsychErrorExitMsg(PsychError_user, "Unsupported point size requested in Screen('DrawDots').");
	      }

	      // Setup point size for this point:
	      glPointSize(size[i]);

	      // Render point:
	      glDrawArrays(GL_POINTS, i, 1);
            }
        }
        
        // Disable fast rendering of arrays:
        glDisableClientState(GL_VERTEX_ARRAY);
        if (usecolorvector) glDisableClientState(GL_COLOR_ARRAY);
        
        // Old implementation: Loops over the array of points and submits each point
        // via a glVertex() call. The new solution above saves the overhead for the
        // for-loop and for the individual glVertex-calls.
        // glBegin(GL_POINTS);
	// for(i=0; i<2*n; i+=2) glVertex2d(xy[i], xy[i+1]);
	// glEnd();
	
        // Restore old matrix from backup copy, undoing the global translation:
        glPopMatrix();

        // turn off antialiasing again
        if(idot_type) glDisable(GL_POINT_SMOOTH);

        // Reset pointsize to 1.0
        glPointSize(1);
        
        // Mark end of drawing op. This is needed for single buffered drawing:
        PsychFlushGL(windowRecord);
        
 	//All psychfunctions require this.
	return(PsychError_none);
}
