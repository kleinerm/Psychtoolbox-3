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
int		verbosity = 2;

// Callback string for eyelink display callback function:
char eyelinkDisplayCallbackFunc[1024];

// Memory pointer to malloc()'ed image pixel buffer that holds the
// image data for a RGBA8 texture with the most recent eye camera image:
static byte* eyeimage = NULL;

// Width x Height of eye camera image in pixels:
static int eyewidth  = 0;
static int eyeheight = 0;

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

/* Eyelink('Verbosity') - Set level of verbosity.
 */
PsychError EyelinkVerbosity(void) 
{
 	static char useString[] = "oldlevel = Eyelink('Verbosity' [,level]);";
	static char synopsisString[] = 
		"Set level of verbosity for error/warning/status messages. 'level' optional, new level "
		"of verbosity. 'oldlevel' is the old level of verbosity. The following levels are "
		"supported: 0 = Shut up. 1 = Print errors, 2 = Print also warnings, 3 = Print also some info, "
		"4 = Print more useful info (default), >5 = Be very verbose (mostly for debugging the driver itself). ";		
	static char seeAlsoString[] = " ";	 

	int level= -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	PsychCopyInIntegerArg(1, kPsychArgOptional, &level);
	if (level < -1) PsychErrorExitMsg(PsychError_user, "Invalid level of verbosity provided. Valid are levels of zero and greater.");
	
	// Return current/old level:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) verbosity);

	// Set new level, if one was provided:
	if (level > -1) verbosity = level;

	return(PsychError_none);
}

// Return level of verbosity:
int Verbosity(void) {
	return(verbosity);
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

	// Not used for now, but defined anyway to make Eyelink runtime happy:
	fcns.setup_cal_display_hook = PsychEyelink_setup_cal_display;
	fcns.clear_cal_display_hook = PsychEyelink_clear_display;
	fcns.erase_cal_target_hook  = PsychEyelink_clear_display;
	fcns.draw_cal_target_hook   = PsychEyelink_draw_cal_target;
	fcns.image_title_hook       = PsychEyelink_image_title;
	fcns.get_input_key_hook     = PsychEyelink_get_input_key;
	fcns.alert_printf_hook      = PsychEyelink_alert_printf_hook;
	
	// Assign runtime environment display callback function:
	memset(eyelinkDisplayCallbackFunc, 0, sizeof(eyelinkDisplayCallbackFunc));
    #if PSYCH_SYSTEM != PSYCH_WINDOWS
	snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);
    #else
	_snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);
    #endif
    
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
	InputEvent keyinput;
	
	// Pseudo-Eyelink camera image test:
	
	// Setup pseudo eye-display of 640 x 480 pixels via setup callback:
	PsychEyelink_setup_image_display(640, 480);

	// Setup display:
	PsychEyelink_setup_cal_display();
	PsychEyelink_clear_display();
	
	// Build pseudo color LUT:
	for (i=0; i < 256; i++) {
		r[i]=i;
		g[i]=255 - i;
		b[i]=i * 2;
	}
	PsychEyelink_set_image_palette(256, r, g, b);

	// Set image title:
	PsychEyelink_image_title(1, "Foobar-O-Matic:");

	// Run pseudo-display loop for 600 frames:
	for (i = 0; i < 600; i++) {
		// Draw calibration target:
		PsychEyelink_draw_cal_target(i, 200);

		// Fill buffer with image pattern:
		for (y=1; y <= 480; y++) {
			// Build y'th scanline:
			for (x=0; x < 640; x++) scanline[x] = (byte) ((x + y + i) % 256);
			
			// Submit y'th scanline:
			PsychEyelink_draw_image_line(640, y, 480, (byte*) &scanline);
		}
		
		// Check keyboard:
		if (PsychEyelink_get_input_key(&keyinput) > 0) {
			PsychEyelink_alert_printf_hook("Eyelink: Key detected.\n");
			// Break out of loop on keycode 41 == ESCAPE on OS/X.
			if (keyinput.key.key == 41) break;
		}
	}

	// Tear down pseudo display:
	PsychEyelink_exit_image_display();

	return;
}

int PsychEyelinkCallRuntime(int cmd, int x, int y, char* msg)
{
	PsychGenericScriptType	*inputs[2];
	PsychGenericScriptType	*outputs[1];
	double* callargs;
	double rc;

	// Create a Matlab double matrix with 4 elements: 1st is command code '1'
	// 2nd is the double pointer, 3rd is image width, 4th is image height:
	outputs[0]  = NULL;
	inputs[0]   = mxCreateDoubleMatrix(1, 4, mxREAL);
	callargs    = mxGetPr(inputs[0]);
	
	callargs[0] = (double) cmd; // Command code.
	callargs[1] = (double) x;
	callargs[2] = (double) y;

	if (msg != NULL) {
		inputs[1] = mxCreateString(msg);
	}
	else {
		inputs[1] = NULL;
	}
	
	// Call the runtime environment:
	if (mexCallMATLAB((cmd == 2) ? 1 : 0, outputs, (inputs[1]) ? 2 : 1, inputs, eyelinkDisplayCallbackFunc) > 0) {
		printf("EYELINK: WARNING! PsychEyelinkCallRuntime() Failed to call eyelink callback function!\n");
	}

	// Release our matrix again:
	mxDestroyArray(inputs[0]);
	if (msg != NULL) mxDestroyArray(inputs[1]);
	
	if (outputs[0]) {
		rc = mxGetScalar(outputs[0]);
		mxDestroyArray(outputs[0]);
	}
	else {
		rc = 0;
	}
	
	return((int) rc);
}

// Callback functions, called by Eyelink runtime at various occassions, e.g,
// during tracker setup, drift correction/calibration etc.:
// =========================================================================

// PsychEyelink_setup_image_display() tells the width and height of the camera
// image in pixels.
INT16 ELCALLBACK PsychEyelink_setup_image_display(INT16 width, INT16 height)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_setup_image_display()\n");
	
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
	
	if (Verbosity() > 5) printf("Eyelink: Leaving PsychEyelink_setup_image_display()\n");

	// Done.
	return(0);
}

// PsychEyelink_exit_image_display() shuts down any camera image display:
void ELCALLBACK PsychEyelink_exit_image_display(void)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_exit_image_display()\n");

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
	
	if (Verbosity() > 6) printf("Eyelink: Entering PsychEyelink_draw_image_line()\n");

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
		v0 = ( eyeimage + ( (line - 1) * width * 4 ) );
		
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

		if (Verbosity() > 6) printf("Eyelink: PsychEyelink_draw_image_line(): Scanline %i received.\n", (int) line);

		// Complete new eye image received?
		if (line == totlines) {
			// Yes. Our eyeimage buffer contains a new image. Compute double-encoded
			// Matlab/Octave compatible memory pointer to image buffer:
			teximage = PsychPtrToDouble((void*) eyeimage);

			// Ok, teximage is a memory pointer to our image buffer, encoded as a double.
			// Now we need to call our Matlab callback function which actually converts
			// the data in our internal image buffer into a PTB texture, then draws that
			// texture etc. to display the new eye camera image.
			if (Verbosity() > 6) printf("Eyelink: PsychEyelink_draw_image_line(): All %i Scanlines received. Calling Runtime!\n", (int) line);
			
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
	
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_set_image_palette()\n");

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

INT16  ELCALLBACK PsychEyelink_setup_cal_display(void)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_setup_cal_display()\n");

	// Tell runtime to setup calibration display: Command code 7.
	PsychEyelinkCallRuntime(7, 0, 0, NULL);

	return(0);
}

void ELCALLBACK   PsychEyelink_clear_display(void)
{
	if (Verbosity() > 6) printf("Eyelink: Entering PsychEyelink_clear_display()\n");

	// Tell runtime to clear display: Command code 6.
	PsychEyelinkCallRuntime(6, 0, 0, NULL);

	return;
}

void ELCALLBACK   PsychEyelink_draw_cal_target(INT16 x, INT16 y)
{
	if (Verbosity() > 6) printf("Eyelink: Entering PsychEyelink_draw_cal_target(): x=%i y=%i.\n", (int) x, (int) y);

	// Tell runtime about where to draw calibration target: Command code 5.
	PsychEyelinkCallRuntime(5, (int) x, (int) y, NULL);

	return;
}

void ELCALLBACK   PsychEyelink_image_title(INT16 unused, char *title)
{
	if (Verbosity() > 6) printf("Eyelink: Entering PsychEyelink_image_title(): state = %i : Title = %s\n", (int) unused, title);

	// Tell runtime about image title: Command code 4.
	PsychEyelinkCallRuntime(4, (int) unused, 0, title);

	return;
}

INT16 ELCALLBACK  PsychEyelink_get_input_key(InputEvent *keyinput)
{
	int ky = 0;
	InputEvent *key_input = keyinput;

	if (Verbosity() > 6) printf("Eyelink: Entering PsychEyelink_get_input_key()\n");

	// Call runtime for keycode of pressed key (command code 2):
	ky = PsychEyelinkCallRuntime(2, 0, 0, NULL);
	if (ky > 0) {
		// Fill Eyelinks InputEvent struct:
		memset(key_input, 0, sizeof(InputEvent));
		key_input->key.key = ky;
		key_input->key.state = 1;
		key_input->key.type = KEYINPUT_EVENT;
		key_input->key.modifier = 0; //event.key.keysym.mod;
		key_input->key.unicode = 0;//event.key.keysym.unicode;
		
		// One key pressed:
		return(1);
	}
	else {
		// No key pressed:
		return(0);
	}
}

void ELCALLBACK   PsychEyelink_alert_printf_hook(const char *msg)
{
	// Print error message to runtime console if error output is allowed:
	if (Verbosity() > 3) printf("Eyelink: Alert! Eyelink says: %s.\n\n", msg);
	
	// Tell runtime about alert condition: Command code 3.
	PsychEyelinkCallRuntime(3, 0, 0, (char*) msg);

	return;
}
