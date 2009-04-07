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
	
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_init_core_graphics()\n");
	
	memset(&fcns, 0, sizeof(fcns));
	
	// Setup cam image callbacks:
	fcns.draw_image_line_hook   = PsychEyelink_draw_image_line;
	fcns.set_image_palette_hook = PsychEyelink_set_image_palette;
	fcns.exit_image_display_hook= PsychEyelink_exit_image_display;
	fcns.setup_image_display_hook = PsychEyelink_setup_image_display;

	// Other callbacks to make Eyelink runtime happy:
	fcns.setup_cal_display_hook = PsychEyelink_setup_cal_display;
	fcns.draw_cal_target_hook   = PsychEyelink_draw_cal_target;
	fcns.image_title_hook       = PsychEyelink_image_title;
	fcns.get_input_key_hook     = PsychEyelink_get_input_key;
	fcns.alert_printf_hook      = PsychEyelink_alert_printf_hook;

	// All of these, except the first one, don't make much sense. Just set
	// to make eyelink-core happy:
	fcns.clear_cal_display_hook = PsychEyelink_clear_display;
	fcns.exit_cal_display_hook  = PsychEyelink_clear_display;
	fcns.erase_cal_target_hook  = PsychEyelink_clear_display;
	fcns.record_abort_hide_hook = PsychEyelink_clear_display;
	
	// Assign runtime environment display callback function:
	memset(eyelinkDisplayCallbackFunc, 0, sizeof(eyelinkDisplayCallbackFunc));

	snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);
//    #if PSYCH_SYSTEM != PSYCH_WINDOWS
//	snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);
//    #else
//	_snprintf(eyelinkDisplayCallbackFunc, sizeof(eyelinkDisplayCallbackFunc) - 1, callback);
//    #endif
    
	// Assign hooks to Eyelink runtime:
	setup_graphic_hook_functions(&fcns);
	
	// Optionally dump the whole hookfunctions struct:
	if (Verbosity() > 5) PsychEyelink_dumpHookfunctions();
	
	return;
}

// Disable all hook functions at shutdown time:
void PsychEyelink_uninit_core_graphics(void)
{
	HOOKFCNS fcns;
	
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_uninit_core_graphics()\n");
	
	memset(&fcns, 0, sizeof(fcns));
	setup_graphic_hook_functions(&fcns);

	// Optionally dump the whole hookfunctions struct:
	if (Verbosity() > 5) PsychEyelink_dumpHookfunctions();

	return;
}

void PsychEyelink_dumpHookfunctions(void)
{
	HOOKFCNS* pfcns = get_all_hook_functions();
	int i;
	
	printf("PsychEyelink: Dump of current Eyelink HOOKFCNS struct as byte array:\n\n");
	for (i=0; i < sizeof(HOOKFCNS); i++) printf(" %02x", (int)(((unsigned char*) pfcns)[i]));
	printf("\nPsychEyelink: Dump done\n\n");

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
			// Break out of loop on keycode 41 or 27 == ESCAPE on OS/X or Windows.
			if (keyinput.key.key == 41 || keyinput.key.key == 27) break;
			if (keyinput.key.key == TERMINATE_KEY) {
				printf("Eyelink: TestSuite: WARNING: Abort code detected. Master abort.\n");
				break;
			}
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
	int retc;

	// Callbacks forcefully disabled by error-handling? Return with error code if so:
	if (0 == eyelinkDisplayCallbackFunc[0]) return(0xdeadbeef);

	// Create a Matlab double matrix with 4 elements: 1st is command code 
	// others are available for use specific to each command
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
	
	// Call the runtime environment, on Matlab with the trap flag set, so control
	// returns to us on error, instead of returning to Matlab runtime system. Eyelink
	// runtime system doesn't like losing control and would crash otherwise!
	#if PSYCH_LANGUAGE == PSYCH_MATLAB
		mexSetTrapFlag(1);
	#endif
	
	// Call the runtime environment:
	if ((retc = mexCallMATLAB((cmd == 2) ? 1 : 0, outputs, (inputs[1]) ? 2 : 1, inputs, eyelinkDisplayCallbackFunc)) > 0) {
		printf("EYELINK: WARNING! PsychEyelinkCallRuntime() Failed to call eyelink runtime callback function %s [rc = %i]!\n", eyelinkDisplayCallbackFunc, retc);
		printf("EYELINK: WARNING! Make sure that function is on your Matlab/Octave path and properly initialized.\n");
		printf("EYELINK: WARNING! May also be an error during execution of that function. Type ple at command prompt for error messages.\n");
		printf("EYELINK: WARNING! Auto-Disabling all callbacks to the runtime environment for safety reasons.\n");
		eyelinkDisplayCallbackFunc[0] = 0;
	}

	// Reset error handling to default on Matlab:
	#if PSYCH_LANGUAGE == PSYCH_MATLAB
		mexSetTrapFlag(0);
	#endif

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

void ELCALLBACK PsychEyelink_noop(void)
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
	static INT16 lastline = -1;
	static int wrapcount = 0;
	static tlastwrap = 0.0;
	double tnow;
	int rc;
	int ind;
	byte* p;
	byte* v0;
	short i;
	
	if (Verbosity() > 8) printf("Eyelink: Entering PsychEyelink_draw_image_line()\n");

	// Callbacks forcefully disabled by error-handling? Simply return with no-op, if so:
	if (0 == eyelinkDisplayCallbackFunc[0]) return;

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
		v0 = ( eyeimage + ( (totlines - line) * width * 4 ) );
		
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

		if (Verbosity() > 8) printf("Eyelink: PsychEyelink_draw_image_line(): Scanline %i received.\n", (int) line);

		// Premature wraparound?
		if (line < lastline) {
			// Premature wraparound due to too slow processing. Increase wrapcounter:
			wrapcount++;			
		}

		// More than some threshold?
		if (wrapcount > 10) {
			// Spill a warning?
			PsychGetAdjustedPrecisionTimerSeconds(&tnow);
			if (tnow - tlastwrap > 2.0) {
				// Last invocation longer than 2 seconds away:
				// Output some warning to console...
				if (Verbosity() > 1) {
					printf("Eyelink: Warning: Skipped videoframes from eye camera detected within last seconds (count=%i)\n", wrapcount);
					printf("Eyelink: Warning: Timing problems on your machine or network problems on tracker connection?!?\n\n");
				}

				// Update / Reset detector:
				tlastwrap = tnow;
				wrapcount = 0;
			}				
		}
		
		// Update skip detector:
		lastline = line;

		// Complete new eye image received?
		if (line == totlines) {
			// Yes. Our eyeimage buffer contains a new image.
			
			// Reset skip detector:
			lastline  = -1;

			// Compute double-encoded Matlab/Octave compatible memory pointer to image buffer:
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

			// Call the runtime environment, on Matlab with the trap flag set, so control
			// returns to us on error, instead of returning to Matlab runtime system. Eyelink
			// runtime system doesn't like losing control and would crash otherwise!
			#if PSYCH_LANGUAGE == PSYCH_MATLAB
				mexSetTrapFlag(1);
			#endif
			
			rc = mexCallMATLAB(0, outputs, 1, inputs, eyelinkDisplayCallbackFunc);
			if(rc) {
				printf("EYELINK: WARNING! Failed to call eyelink camera image display callback function %s [rc=%i]!\n", eyelinkDisplayCallbackFunc, rc);
				printf("EYELINK: WARNING! Make sure that function is on your Matlab/Octave path and properly initialized.\n");
				printf("EYELINK: WARNING! May also be an error during execution of that function. Type ple at command prompt for error messages.\n");
				printf("EYELINK: WARNING! Auto-Disabling all callbacks to the runtime environment for safety reasons.\n");
				eyelinkDisplayCallbackFunc[0] = 0;
			}

			// Reset error handling to default on Matlab:
			#if PSYCH_LANGUAGE == PSYCH_MATLAB
				mexSetTrapFlag(0);
			#endif

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
	if (0xdeadbeef == PsychEyelinkCallRuntime(7, 0, 0, NULL)) {
		// Error condition. Return error to eyelink runtime:
		return(-1);
	}

	// Return success:
	return(0);
}

void ELCALLBACK   PsychEyelink_clear_display(void)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_clear_display()\n");

	// Tell runtime to clear display: Command code 6.
	PsychEyelinkCallRuntime(6, 0, 0, NULL);

	return;
}

void ELCALLBACK   PsychEyelink_draw_cal_target(INT16 x, INT16 y)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_draw_cal_target(): x=%i y=%i.\n", (int) x, (int) y);

	// Tell runtime about where to draw calibration target: Command code 5.
	PsychEyelinkCallRuntime(5, (int) x, (int) y, NULL);

	return;
}

void ELCALLBACK   PsychEyelink_image_title(INT16 threshold, char *title)
{
	if (Verbosity() > 5) printf("Eyelink: Entering PsychEyelink_image_title(): threshold = %i : Title = %s\n", (int) threshold, title);

	// Tell runtime about image title: Command code 4.
	PsychEyelinkCallRuntime(4, (int) threshold, 0, title);

	return;
}

#ifndef ELKEY_DOWN
#define ELKEY_DOWN 1 //temporary while we wait for sr-research's lib to get updated with this
#endif

INT16 ELCALLBACK  PsychEyelink_get_input_key(InputEvent *keyinput)
{
	int ky = 0;
	double tnow;
	static double tlastquery = 0;
	const  double tmininterval = 0.1;	// Allow one query every 0.1 seconds.
	InputEvent *key_input = keyinput;

	// Throttling routine:
	// We don't want key queries to call out to the runtime too often, as this
	// creates a quite significant overhead, e.g., approx. 1 msec for a KbCheck
	// for a fast 2009'ish machine on OS/X!
	PsychGetAdjustedPrecisionTimerSeconds(&tnow);
	if (tnow - tlastquery < tmininterval) {
		// Last invocation less than tmininterval seconds away. Throttle this,
		// we just return "no key pressed".
		if (Verbosity() > 9) printf("Eyelink: In PsychEyelink_get_input_key(): Throttling...\n");
		return(0);
	}
	else {
		// Last invocation longer than tmininterval seconds away. Accept this
		// query and update timestamp:
		tlastquery = tnow;
	}
	
	if (Verbosity() > 7) printf("Eyelink: Entering PsychEyelink_get_input_key()\n");

	// Call runtime for keycode of pressed key (command code 2):
	if ((ky = PsychEyelinkCallRuntime(2, 0, 0, NULL)) == 0xdeadbeef) {
		// Error condition in runtime callback! Can't progress. We try to
		// shutdown the current eyelink runtime operation by sending a fake
		// keycode corresponding to the terminate key:
		if (Verbosity() > 0) printf("Eyelink: In PsychEyelink_get_input_key(): Error condition detected: Trying to send TERMINATE_KEY abort keycode!\n");
		ky = TERMINATE_KEY;
	}
	
	if (ky > 0) {
		// Fill Eyelinks InputEvent struct:
		memset(key_input, 0, sizeof(InputEvent));
		key_input->key.key = ky;
		key_input->key.state = ELKEY_DOWN; 
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
