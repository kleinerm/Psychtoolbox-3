/*
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkGetQueuedItems.c
 
	PROJECTS: Eyelink
 
	AUTHORS:
		e_flister@yahoo.com			edf
 
	PLATFORMS:	all
 
	HISTORY:
 
		21/03/2009	edf 		created it
 
	TARGET LOCATION:
 
		Eyelink.mexmac resides in:
			PsychHardware/EyelinkToolbox
 */

#include "PsychEyelink.h"

static char useString[] = "[samples, events, drained] = Eyelink('GetQueuedItems' [, eye])";

static char synopsisString[] =
"dequeues all samples and events from the link.\n"
"returns double matrices where columns are items and rows are fields from eyelink sample structs.\n"
"return flag 'drained' indicates whether queue was emptied or if this function needs to be called again.\n"
"if you include the eye argument (as LEFT_EYE or RIGHT_EYE as returned by EyelinkInitDefaults), samples will include raw fields for that eye.\n"
"if you don't remove items from the queue often enough, the oldest items will be replaced by a LOST_DATA_EVENT, which will appear in the sample records at the location where items were dropped (fields other than type will be set to MISSING_DATA).\n\n"

"sample rows are as follows: \n"
"\t 1: time of sample (when camera imaged eye, in milliseconds since tracker was activated)\n"
"\t 2: type (always SAMPLE_TYPE or LOST_DATA_EVENT)\n"
"\t 3: flags (bits indicating what types of data are present, and for which eye(s))\n"
"\t 4: left pupil center x (camera coordinates)\n"
"\t 5: right pupil center x (camera coordinates)\n"
"\t 6: left pupil center y (camera coordinates)\n"
"\t 7: right pupil center y (camera coordinates)\n"
"\t 8: left HEADREF x (angular gaze coordinates)\n"
"\t 9: right HEADREF x (angular gaze coordinates)\n"
"\t 10: left HEADREF y (angular gaze coordinates)\n"
"\t 11: right HEADREF y (angular gaze coordinates)\n"
"\t 12: left pupil size (arbitrary units, area or diameter as selected by pupil_size_diameter command)\n"
"\t 13: right pupil size (arbitrary units, area or diameter as selected by pupil_size_diameter command)\n"
"\t 14: left gaze position x (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 15: right gaze position x (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 16: left gaze position y (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 17: right gaze position y (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 18: angular resolution x (at gaze position in screen pixels per visual degree)\n"
"\t 19: angular resolution y (at gaze position in screen pixels per visual degree)\n"
"\t 20: status (Error and status flags (only useful for EyeLink II and EyeLink1000, report CR status and tracking error). See eye_data.h for useful bits.)\n"
"\t 21: input (data from input port(s))\n"
"\t 22: button input data: high 8 bits indicate changes from last sample, low 8 bits indicate current state of buttons 8 (MSB) to 1 (LSB)\n"
"\t 23: head-tracker data type (0=none)\n"
"\t 24: head-tracker data (not prescaled) 1\n"
"\t 25: head-tracker data (not prescaled) 2\n"
"\t 26: head-tracker data (not prescaled) 3\n"
"\t 27: head-tracker data (not prescaled) 4\n"
"\t 28: head-tracker data (not prescaled) 5\n"
"\t 29: head-tracker data (not prescaled) 6\n"
"\t 30: head-tracker data (not prescaled) 7\n"
"\t 31: head-tracker data (not prescaled) 8\n\n"

"if you request the raw sample fields, they will appear in the following additional rows:\n"
"\t 32: raw x sensor position of the pupil\n"
"\t 33: raw y sensor position of the pupil\n"
"\t 34: raw x sensor position of the corneal reflection\n"
"\t 35: raw y sensor position of the corneal reflection\n"
"\t 36: raw pupil area\n"
"\t 37: raw corneal reflection area\n"
"\t 38: raw width of pupil\n"
"\t 39: raw height of pupil\n"
"\t 40: raw width of corneal reflection\n"
"\t 41: raw height of corneal reflection\n"
"\t 42: raw x position of tracking window on sensor\n"
"\t 43: raw y position of tracking window on sensor\n"
"\t 44: (raw pupil x) - (raw corneal reflection x)\n"
"\t 45: (raw pupil y) - (raw corneal reflection y)\n"
"\t 46: raw area of 2nd corneal reflection candidate\n"
"\t 47: raw x sensor position of the 2nd corneal reflection candidate\n"
"\t 48: raw y sensor position of the 2nd corneal reflection candidate\n\n"

"About raw fields:\n"
"Normal samples do not contain the corneal reflection data, but some (non-saccade-based) calibration methods require this information.\n"
"The Eyelink 1000 can be configured to send this information as part of 'raw' samples, and this is required before use of this function.\n"
"Sol Simpson at SR-Research emphasizes that this is not officially supported or guaranteed.\n"
"1. You need to install Eyelink.dll from the latest software developer kit at https://www.sr-support.com/forums/showthread.php?t=6\n"
"\t   and the latest tracker host software at https://www.sr-support.com/forums/showthread.php?t=179\n"
"2. Issue the commands:\n"
"\t   Eyelink('command','link_sample_data = LEFT,RIGHT,GAZE,AREA,GAZERES,HREF,PUPIL,STATUS,INPUT,HMARKER');\n"
"\t   Eyelink('command','inputword_is_window = ON');\n"
"More info:\n"
"\t   HMARKER (originally for Eyelink2's infrared head tracking markers) and INPUT (originally for the TTL lines) are jury-rigged to hold the extra data.\n"
"\t   You can also set file_sample_data to collect raw samples in the .edf file.\n"
"CAUTION: Raw fields are not supported on Linux. It may or may not work on your setup with your tracker. Currently it doesn't work on Linux at least.\n\n"

" event rows are as follows: \n"
"\t 1: effective time of event\n"
"\t 2: event type\n"
"\t 3: read (bits indicating which data fields contain valid data)\n"
"\t 4: eye\n"
"\t 5: start time\n"
"\t 6: end time\n"
"\t 7: HEADREF gaze position starting point x\n"
"\t 8: HEADREF gaze position starting point y\n"
"\t 9: display gaze position starting point x (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 10: display gaze position starting point y (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 11: starting pupil size (arbitrary units, area or diameter as selected by pupil_size_diameter command)\n"
"\t 12: HEADREF gaze position ending point x\n"
"\t 13: HEADREF gaze position ending point y\n"
"\t 14: display gaze position ending point x (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 15: display gaze position ending point y (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 16: ending pupil size (arbitrary units, area or diameter as selected by pupil_size_diameter command)\n"
"\t 17: HEADREF gaze position average x\n"
"\t 18: HEADREF gaze position average y\n"
"\t 19: display gaze position average x (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 20: display gaze position average y (in pixel coordinates set by screen_pixel_coords command)\n"
"\t 21: average pupil size (arbitrary units, area or diameter as selected by pupil_size_diameter command)\n"
"\t 22: average gaze velocity magnitude (absolute value) in visual degrees per second\n"
"\t 23: peak gaze velocity magnitude (absolute value) in visual degrees per second\n"
"\t 24: starting gaze velocity in visual degrees per second\n"
"\t 25: ending gaze velocity in visual degrees per second\n"
"\t 26: starting angular resolution x in screen pixels per visual degree\n"
"\t 27: ending angular resolution x in screen pixels per visual degree\n"
"\t 28: starting angular resolution y in screen pixels per visual degree\n"
"\t 29: ending angular resolution y in screen pixels per visual degree\n"
"\t 30: status (Collected error and status flags from all samples in the event (only useful for EyeLink II and EyeLink1000, report CR status and tracking error). See eye_data.h for useful bits.)\n\n";

static char seeAlsoString[] = "";

#define NUM_SAMPLE_FIELDS 31
#define NUM_RAW_SAMPLE_FIELDS 17
#define NUM_EVENT_FIELDS 30
#define FUDGE_FACTOR 2 // how much more space we allocate beyond the reported queue length to account for additional items arriving as we are dequeueing
#define ERR_BUFF_LEN 100

/*
ROUTINE: EyelinkGetQueuedItems
PURPOSE:
	matlab is slow at dealing with structs and looping over eyelink_get_float_data to drain the queue, so we take care of this for the client.
	also eliminates usage error of supplying incorrect type from Eyelink('GetNextDataType') to Eyelink('GetFloatData').
 
 TODO:
	if raw values requested, is there a way to query the tracker to make sure that the following requirements have been set?
		Eyelink('command','link_sample_data = LEFT,RIGHT,GAZE,AREA,GAZERES,HREF,PUPIL,STATUS,INPUT,HMARKER')
		Eyelink('command','inputword_is_window = ON')
 
	UINT16 flags;
	ILINKDATA* current_data;
	current_data = eyelink_data_status();
	flags = current_data -> sample_data; //contains the link_sample_data flags
 
	or can call UINT16 eyelink_sample_data_flags (void) to get it for one sample (the one before or after get_next_data?)
 
	to decode, bitwise and (&) against these (from eye_data.h):
	SAMPLE_LEFT, SAMPLE_RIGHT, SAMPLE_PUPILXY, SAMPLE_HREFXY, SAMPLE_GAZEXY, SAMPLE_GAZERES, SAMPLE_PUPILSIZE, SAMPLE_STATUS, SAMPLE_INPUTS, SAMPLE_BUTTONS, SAMPLE_HEADPOS
	is SAMPLE_HEADPOS the same as HMARKER?  description is confusing:  Head-position: byte tells # words
	anyway to get inputword_is_window?
 
 
 
 You can query value of link_sample_data by using eyelink_read_request()
 
Here is a simple example of how to use it:
 
int get_var_data(char *var)
{
  char buf[100] = "";
  UINT32 t;
 
 
  if(!eyelink_is_connected()) return 1;    // not connected
  eyelink_read_request(var);
  t = current_msec();
  while(current_msec()-t < 500)
	{
	  if(eyelink_read_reply(buf) == OK_RESULT)
		{
		  printf("reply = %s\n", buf);
			return 0;
		}
	}
  return -1;  // timed out
}
 
 
 
 make sure that you 0 out the FSAMPLE_RAW before you pass in, so you can do a sanity check.
 
 
	note 50 doubles at 2000Hz is nearly 1MB/sec, so if the user collects everything we give them, trials have a practical limit of ~5mins...
 
	if raw values requested, we need to make sure that the host tracker software version and the dll/sdk version are compatible, otherwise values are inaccurate!
	need to call eyelink_get_tracker_version to get host side, and eyelink_dll_version to get display side, and store boolean compatibility in a static var (so we don't waste time calling it every time).
 
 */

PsychError EyelinkGetQueuedItems(void)
{
	FSAMPLE      fs;
	FSAMPLE_RAW  fr;
	FEVENT       fe;
	int numSamples = 0, numEvents = 0, maxSamples, maxEvents, type, eye, fieldNum, index, numSampleFields, err;
	double *samples, *events;
	boolean useEye=FALSE, done=FALSE;
	PsychNativeBooleanType drained=FALSE;
	char errmsg[ERR_BUFF_LEN];
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(1));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(3));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	if (PsychCopyInIntegerArg(1, kPsychArgOptional, &eye)) {
		#if PSYCH_SYSTEM != PSYCH_LINUX
		if (eye!=LEFT_EYE && eye!=RIGHT_EYE) {
			PsychErrorExitMsg(PsychErorr_argumentValueOutOfRange, "EyeLink: GetQueuedItems:  eye argument must be LEFT_EYE or RIGHT_EYE as returned by EyelinkInitDefaults\n");
		}
		useEye=TRUE;
		numSampleFields=NUM_SAMPLE_FIELDS+NUM_RAW_SAMPLE_FIELDS;
		#else
		PsychErrorExitMsg(PsychError_unimplemented, "Sorry, collecting raw values is not supported on Linux (do not supply an eye argument).");
		#endif
	} else {
		numSampleFields=NUM_SAMPLE_FIELDS;
		useEye=FALSE;
	}
	
	maxSamples = FUDGE_FACTOR*eyelink_data_count(1,0);
	maxEvents = FUDGE_FACTOR*eyelink_data_count(0,1);
	
	samples = (double *)PsychMallocTemp(maxSamples*numSampleFields*sizeof(double)); // according to mario if OOM, ultimately calls to mxCreateNumericArray/mxMalloc will error inside matlab rather than return NULL
	events = (double *)PsychMallocTemp(maxEvents*NUM_EVENT_FIELDS*sizeof(double));
	
	while(!done){
		type = eyelink_get_next_data(NULL);
		
		switch(type) {
			case SAMPLE_TYPE:
				if (eyelink_get_float_data(&fs) != type) {
					PsychErrorExitMsg(PsychError_internal, "Eyelink: GetQueuedItems: eyelink_get_float_data did not return same sample type as eyelink_get_next_data.");
				}
				
				if (useEye) {
					err = eyelink_get_extra_raw_values_v2(&fs, eye, &fr); // bug: accuracy of raw vals requires that we check that tracker is configured properly (see above) and tracker software/our eyelink dll are compatible versions -- how?
					if(err){
						//snprintf(errmsg, ERR_BUFF_LEN, "Eyelink: GetQueuedItems: eyelink_get_extra_raw_values_v2 returned error code %d: %s", err, eyelink_get_error(err,"eyelink_get_extra_raw_values_v2"));
						sprintf(errmsg, "Eyelink: GetQueuedItems: eyelink_get_extra_raw_values_v2 returned error code %d: %s", err, eyelink_get_error(err,"eyelink_get_extra_raw_values_v2")); //no snprintf in msvs?  bug: buff overflow
						PsychErrorExitMsg(PsychError_internal, errmsg);
					}
				}
				
				index=PsychIndexElementFrom2DArray(numSampleFields, maxSamples, 0, numSamples++);
				samples[index++]=(double)(fs.time); // 1
				samples[index++]=(double)(fs.type); // 2
				samples[index++]=(double)(fs.flags); // 3
				samples[index++]=(double)(fs.px[0]); // 4
				samples[index++]=(double)(fs.px[1]); // 5
				samples[index++]=(double)(fs.py[0]); // 6
				samples[index++]=(double)(fs.py[1]); // 7
				samples[index++]=(double)(fs.hx[0]); // 8
				samples[index++]=(double)(fs.hx[1]); // 9
				samples[index++]=(double)(fs.hy[0]); // 10
				samples[index++]=(double)(fs.hy[1]); // 11
				samples[index++]=(double)(fs.pa[0]); // 12
				samples[index++]=(double)(fs.pa[1]); // 13
				samples[index++]=(double)(fs.gx[0]); // 14
				samples[index++]=(double)(fs.gx[1]); // 15
				samples[index++]=(double)(fs.gy[0]); // 16
				samples[index++]=(double)(fs.gy[1]); // 17
				samples[index++]=(double)(fs.rx); // 18
				samples[index++]=(double)(fs.ry); // 19
				samples[index++]=(double)(fs.status); // 20
				samples[index++]=(double)(fs.input); // 21
				samples[index++]=(double)(fs.buttons); // 22
				samples[index++]=(double)(fs.htype); // 23
				samples[index++]=(double)(fs.hdata[0]); // 24
				samples[index++]=(double)(fs.hdata[1]); // 25
				samples[index++]=(double)(fs.hdata[2]); // 26
				samples[index++]=(double)(fs.hdata[3]); // 27
				samples[index++]=(double)(fs.hdata[4]); // 28
				samples[index++]=(double)(fs.hdata[5]); // 29
				samples[index++]=(double)(fs.hdata[6]); // 30
				samples[index++]=(double)(fs.hdata[7]); // 31
				
				if (useEye) {
					samples[index++]=(double)(fr.raw_pupil[0]); // 32
					samples[index++]=(double)(fr.raw_pupil[1]); // 33
					samples[index++]=(double)(fr.raw_cr[0]); // 34
					samples[index++]=(double)(fr.raw_cr[1]); // 35
					samples[index++]=(double)(fr.pupil_area); // 36
					samples[index++]=(double)(fr.cr_area); // 37
					samples[index++]=(double)(fr.pupil_dimension[0]); // 38
					samples[index++]=(double)(fr.pupil_dimension[1]); // 39
					samples[index++]=(double)(fr.cr_dimension[0]); // 40
					samples[index++]=(double)(fr.cr_dimension[1]); // 41
					samples[index++]=(double)(fr.window_position[0]); // 42
					samples[index++]=(double)(fr.window_position[1]); // 43
					samples[index++]=(double)(fr.pupil_cr[0]); // 44
					samples[index++]=(double)(fr.pupil_cr[1]); // 45
					samples[index++]=(double)(fr.cr_area2); // 46
					samples[index++]=(double)(fr.raw_cr2[0]); // 47
					samples[index++]=(double)(fr.raw_cr2[1]); // 48
				}
				break;
				
			case LOST_DATA_EVENT: // queue overflowed, we are not supposed to call eyelink_get_float_data on this
				index=PsychIndexElementFrom2DArray(numSampleFields, maxSamples, 0, numSamples++);
				for(fieldNum=0; fieldNum<numSampleFields; fieldNum++){
					samples[index++]= (double)((fieldNum==1) ? LOST_DATA_EVENT : MISSING_DATA);
				}
				break;
				
			case 0: // queue empty
				drained=TRUE;
				break;
				
				default: // it is an event
					if (eyelink_get_float_data(&fe) != type) {
						PsychErrorExitMsg(PsychError_internal, "Eyelink: GetQueuedItems: eyelink_get_float_data did not return same event type as eyelink_get_next_data.");
					}
					
					index=PsychIndexElementFrom2DArray(NUM_EVENT_FIELDS, maxEvents, 0, numEvents++);
					events[index++]=(double)(fe.time); // 1
					events[index++]=(double)(fe.type); // 2
					events[index++]=(double)(fe.read); // 3
					events[index++]=(double)(fe.eye); // 4
					events[index++]=(double)(fe.sttime); // 5
					events[index++]=(double)(fe.entime); // 6
					events[index++]=(double)(fe.hstx); // 7
					events[index++]=(double)(fe.hsty); // 8
					events[index++]=(double)(fe.gstx); // 9
					events[index++]=(double)(fe.gsty); // 10
					events[index++]=(double)(fe.sta); // 11
					events[index++]=(double)(fe.henx); // 12
					events[index++]=(double)(fe.heny); // 13
					events[index++]=(double)(fe.genx); // 14
					events[index++]=(double)(fe.geny); // 15
					events[index++]=(double)(fe.ena); // 16
					events[index++]=(double)(fe.havx); // 17
					events[index++]=(double)(fe.havy); // 18
					events[index++]=(double)(fe.gavx); // 19
					events[index++]=(double)(fe.gavy); // 20
					events[index++]=(double)(fe.ava); // 21
					events[index++]=(double)(fe.avel); // 22
					events[index++]=(double)(fe.pvel); // 23
					events[index++]=(double)(fe.svel); // 24
					events[index++]=(double)(fe.evel); // 25
					events[index++]=(double)(fe.supd_x); // 26
					events[index++]=(double)(fe.eupd_x); // 27
					events[index++]=(double)(fe.supd_y); // 28
					events[index++]=(double)(fe.eupd_y); // 29
					events[index++]=(double)(fe.status); // 30
		}
		
		done = drained || numSamples>=maxSamples || numEvents>=maxEvents;
	}
	
	//bug: who frees memory allocated by PsychAllocOutDoubleMatArg if there is no output arg?  according to matlab doc one should not rely on matlab to do it:
	//   "It is more efficient to perform this cleanup in the source MEX-file than to rely on the automatic mechanism."
	//mario reasons that mathworks would have to have done something stupid for this to be true
	
	//these only work for cutting off the unused parts of the buffers because matlab storage is columnwise
	//this is a little fragile - would be nice if PsychCopy* took the dims of the source and handled noncontiguous cases
	//mario says this is purposesly discouraged and inconvenient to encourage columnwise storage and prevent inefficient fragged memcpys
	PsychCopyOutDoubleMatArg(1, kPsychArgOptional, numSampleFields, numSamples, 1, samples);
	PsychCopyOutDoubleMatArg(2, kPsychArgOptional, NUM_EVENT_FIELDS, numEvents, 1, events);
	
	PsychCopyOutBooleanArg(3, kPsychArgOptional, drained);
	
	// PsychFreeTemp(samples); apparently not paradigmatic to do this oneself
	// PsychFreeTemp(events);
	
	return(PsychError_none);
}
