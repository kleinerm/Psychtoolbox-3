/*

	/osxptb/trunk/PsychSourceGL/Source/Common/Eyelink/PsychEyelink.c

	PROJECTS: Eyelink 
  
	AUTHORS:

		cburns@berkeley.edu				cdb
		E.Peters@ai.rug.nl				emp
		f.w.cornelissen@med.rug.nl		fwc
		mario.kleiner@tuebingen.mpg.de  mk
  
	PLATFORMS:	All.
    
	HISTORY:

		11/22/05  cdb		Created.
		29/06/06  fwc		fixed EyelinkSystemIsConnected to allow dummy mode connections
		15/03/09  mk		Added experimental support for eye camera image display.

	TARGET LOCATION:

		Eyelink.mexmac resides in:
			EyelinkToolbox
*/

#include "PsychEyelink.h"

/////////////////////////////////////////////////////////////////////////
// Global variables used throughout eyelink C files

int		giSystemInitialized = 0;

// Callback string for eyelink display callback function:
char eyelinkDisplayCallbackFunc[1024];

// Memory pointer to malloc()'ed image pixel buffer that holds the
// image data for a RGBA8 texture with the most recent eye camera image:
static byte* eyeimage = NULL;

// Width x Height of eye camera image in pixels:
static unsigned int eyewidth  = 0;
static unsigned int eyeheight = 0;

// Color remapping palette table:
static byte palmap24[3 * 256];

/////////////////////////////////////////////////////////////////////////
// Check if system is initialized
//
PsychError EyelinkSystemIsConnected(void)
{
	int iStatus=-9999;
	iStatus=eyelink_is_connected();
//	mexPrintf("EyelinkSystemIsConnected status %d ((iStatus==0)=%d)\n", iStatus, (iStatus==0) );
	if (iStatus==0) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not connected!\n");
	}
/*	
	if (eyelink_is_connected()==0) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not connected!\n");
	}
	*/
	return(PsychError_none);
}

/////////////////////////////////////////////////////////////////////////
// Check is system is initialized
//
PsychError EyelinkSystemIsInitialized(void)
{
	if (giSystemInitialized != 1) {
		PsychErrorExitMsg(PsychError_user, "Eyelink system is not initialized!\n");
	}
	return(PsychError_none);
}

// Initialize all callback hook functions for use by Eyelink runtime, e.g.,
// all the callbacks for eye camera image display:
void PsychEyelink_init_core_graphics(const char* callback)
{
	HOOKFCNS fcns;
	memset(&fcns, 0, sizeof(fcns));
	
	// Setup cam image callbacks:
	fcns.draw_image_line_hook   = PsychEyelink_draw_image_line;
	fcns.set_image_palette_hook = PsychEyelink_set_image_palette;
	fcns.exit_image_display_hook= PsychEyelink_exit_image_display;
	fcns.setup_image_display_hook = PsychEyelink_setup_image_display;

	// Not used for now, no clue what they do or if they are needed in any way:
	//fcns.setup_cal_display_hook = setup_cal_display;
	//fcns.clear_cal_display_hook = clear_display;
	//fcns.erase_cal_target_hook  = clear_display;
	//fcns.draw_cal_target_hook   = draw_cal_target;
	//fcns.image_title_hook       = image_title;
	//fcns.get_input_key_hook     = get_input_key;
	//fcns.alert_printf_hook      = alert_printf_hook;
	
	// Assign runtime environment display callback function:
	memset(eyelinkDisplayCallbackFunc, 0, sizeof(eyelinkDisplayCallbackFunc));
	snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);

	// Assign hooks to Eyelink runtime:
	setup_graphic_hook_functions(&fcns);
	
	return;
}

// Disable all hook functions at shutdown time:
void PsychEyelink_uninit_core_graphics(void)
{
	HOOKFCNS fcns;
	memset(&fcns, 0, sizeof(fcns));
	setup_graphic_hook_functions(&fcns);
	return;
}

void PsychEyelink_TestEyeImage(void)
{
	int i, x, y;
	byte r[256], g[256], b[256];
	byte scanline[640];
	
	// Pseudo-Eyelink camera image test:
	
	// Setup pseudo eye-display of 640 x 480 pixels via setup callback:
	PsychEyelink_setup_image_display(640, 480);

	// Build pseudo color LUT:
	for (i=0; i < 256; i++) {
		r[i]=i;
		g[i]=255 - i;
		b[i]=i * 2;
	}
	PsychEyelink_set_image_palette(256, r, g, b);

	// Run pseudo-display loop for 600 frames:
	for (i = 0; i < 600; i++) {
		// Fill buffer with image pattern:
		for (y=1; y <= 480; y++) {
			// Build y'th scanline:
			for (x=0; x < 640; x++) scanline[x] = (byte) ((x + y + i) % 256);
			
			// Submit y'th scanline:
			PsychEyelink_draw_image_line(640, y, 480, (byte*) &scanline);
		}
	}

	// Tear down pseudo display:
	PsychEyelink_exit_image_display();

	return;
}

// Callback functions, called by Eyelink runtime at various occassions, e.g,
// during tracker setup, drift correction/calibration etc.:
// =========================================================================

// PsychEyelink_setup_image_display() tells the width and height of the camera
// image in pixels.
INT16 ELCALLBACK PsychEyelink_setup_image_display(INT16 width, INT16 height)
{
	// Release any stale image buffer:
	if (eyeimage != NULL) free(eyeimage);
	
	// Reset everything to startup default:
	eyeimage  = NULL;
	eyewidth  = 0;
	eyeheight = 0;
	
	if (width < 1 || height < 1) {
		printf("EYELINK: WARNING! Invalid image dimensions (smaller than 1 pixel!) received from eyelink: Aborting image setup.\n");
		return(-1);
	}

	// Allocate an internal memory buffer of sufficient size to hold an image
	// of size width x height pixels:
	eyeimage = (byte*) malloc(sizeof(unsigned char) * 4 * width * height);
	if (eyeimage != NULL) {
		eyewidth  = width;
		eyeheight = height;
	}
	else {
		// Failed:
		return(-1);
	}
	
	// Done.
	return(0);
}

// PsychEyelink_exit_image_display() shuts down any camera image display:
void ELCALLBACK PsychEyelink_exit_image_display(void)
{
	// Release any allocated image buffer:
	if (eyeimage != NULL) free(eyeimage);
	
	// Reset everything to startup default:
	eyeimage  = NULL;
	eyewidth  = 0;
	eyeheight = 0;
	
	// Done.
	return;
}

// PsychEyelink_draw_image_line() retrieves exactly one scanline worth of eye camera
// image data. Once a full image has been received, it has to trigger the actual image
// display:
void ELCALLBACK PsychEyelink_draw_image_line(INT16 width, INT16 line, INT16 totlines, byte *pixels)
{
	PsychGenericScriptType			*inputs[1];
	PsychGenericScriptType			*outputs[1];
	double* callargs;
	double teximage;
	int rc;
	int ind;
	byte* p;
	byte* v0;
	short i;
	
	// width, line, totlines within valid range?
	if (width < 1 || width > eyewidth || line < 1 || line > eyeheight || totlines < 1 || totlines > eyeheight) {
		printf("EYELINK: WARNING! Eye camera image with invalid parameters received! (width = %i, line = %i, totlines = %i out of sane range %i x %i)!\n",
				width, line, totlines, eyewidth, eyeheight);
		printf("EYELINK: WARNING! Will try to clamp to valid values, but results may be junk.\n");
		width = eyewidth;
		line = (line < 1) ? 1 : line;
		line = (line > eyeheight) ? line : eyeheight;
		totlines = (totlines < 1) ? 1 : totlines;
		totlines = (totlines > eyeheight) ? totlines : eyeheight;
	}

	// Data structures properly initialized?
	if(eyeimage != NULL) {
		// Retrieve p as pointer to input pixel index color buffer:
		p = pixels;
		
		// Retrieve v0 as pointer to pixel row in output buffer:
		byte* v0 = ( eyeimage + ( (line - 1) * width * 4 ) );
		
		// Copy one row of pixels from input- to output buffer:
		for(i=0; i < width; i++) {
			// Retrieve color index from input buffer:
			ind = *p++;
			
			// Decode via palette color lookup table and store to output buffer:
			// RGB:
			v0[(i*4) + 0] = palmap24[(ind*3) + 0];
			v0[(i*4) + 1] = palmap24[(ind*3) + 1];
			v0[(i*4) + 2] = palmap24[(ind*3) + 2];

			// Alpha channel is a constant 255:
			v0[(i*4) + 3] = 255;
		}

		// Complete new eye image received?
		if (line == totlines) {
			// Yes. Our eyeimage buffer contains a new image. Compute double-encoded
			// Matlab/Octave compatible memory pointer to image buffer:
			teximage = PsychPtrToDouble((void*) eyeimage);

			// Ok, teximage is a memory pointer to our image buffer, encoded as a double.
			// Now we need to call our Matlab callback function which actually converts
			// the data in our internal image buffer into a PTB texture, then draws that
			// texture etc. to display the new eye camera image.
			
			// Create a Matlab double matrix with 4 elements: 1st is command code '1'
			// 2nd is the double pointer, 3rd is image width, 4th is image height:
			outputs[0]  = NULL;
			inputs[0]   = mxCreateDoubleMatrix(1, 4, mxREAL);
			callargs    = mxGetPr(inputs[0]);

			callargs[0] = 1; // 1 == Command code for "Show eye image".
			callargs[1] = teximage;
			callargs[2] = eyewidth;
			callargs[3] = eyeheight;

			// Call the runtime environment:
			rc = mexCallMATLAB(0, outputs, 1, inputs, eyelinkDisplayCallbackFunc);
			if(rc) {
				printf("EYELINK: WARNING! Failed to call eyelink camera image display function!\n");
			}
			
			// Release our matrix again:
			mxDestroyArray(inputs[0]);
		}
	}
	
	// Done.
	return;
}

// PsychEyelink_set_image_palette() sets the color palette for decoding 1-byte color index
// values in an eyelink camera image into RGB8 1-byte-per-color-component color values:
void ELCALLBACK PsychEyelink_set_image_palette(INT16 ncolors, byte r[], byte g[], byte b[])
{
	short i;
	
	if (ncolors > 256) {
		printf("EYELINK: WARNING! Invalid color palette size %i (> 256 colors!) received from eyelink: Clamping to 256 colors.\n", (int) ncolors);
		ncolors = 256;
	}

	// Copy given r,g,b color arrays into internal remapping table palmap24:
	for(i=0; i < ncolors; i++) {
		palmap24[(i*3) + 0] = r[i];
		palmap24[(i*3) + 1] = g[i];
		palmap24[(i*3) + 2] = b[i];
	}

	return;
}
