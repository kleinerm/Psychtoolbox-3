/*
	 /osxptb/trunk/PsychSourceGL/Source/Common/Eyelink/EyelinkGetQueuedItems.c
	 
	 PROJECTS: Eyelink
	 
	 AUTHORS:
	 e_flister@yahoo.com			edf
	 
	 PLATFORMS:	all except linux
	 
	 HISTORY:
	 
	 21/03/2009	edf 		created it
	 
	 TARGET LOCATION:
	 
	 Eyelink.mexmac resides in:
		PsychHardware/EyelinkToolbox
 */

#include "PsychEyelink.h"

static char useString[] = "[samples, events] = Eyelink('GetQueuedItems'[, eye][, max_items])";
static char synopsisString[] =
	"returns double matrices where columns are items and rows are fields from eyelink sample structs (including raw fields).\n"
	"loops over eyelink_get_float_data and collects items until type is 0, indicating the queue is empty, or max_items is reached (default 100).\n"
	"specify the eye for which raw values are desired as LEFT_EYE or RIGHT_EYE as returned by EyelinkInitDefaults. omission of this argument is deprecated.\n"
	"if you don't remove items from the queue often enough, the oldest items will be replaced by a LOST_DATA_EVENT, which will appear in the sample records at the location where items were dropped (fields other than type will be set to MISSING_DATA).\n"
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
	"\t 31: head-tracker data (not prescaled) 8\n"
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
	"\t 30: status (Collected error and status flags from all samples in the event (only useful for EyeLink II and EyeLink1000, report CR status and tracking error). See eye_data.h for useful bits.)\n\n"
	"CAUTION: Not supported on Linux. It may or may not work on your setup with your tracker. Currently it doesn't work on Linux at least.\n\n";

static char seeAlsoString[] = "";

/*
	ROUTINE: EyelinkGetQueuedItems
	PURPOSE:
	matlab is slow at dealing with structs and looping over eyelink_get_float_data to drain the queue, so we take care of this for the client.
	also eliminates usage error of supplying incorrect type from Eyelink('GetNextDataType') to Eyelink('GetFloatData').
*/
PsychError EyelinkGetQueuedItems(void)
{
	FSAMPLE      fs;
	FSAMPLE_RAW  fr;
	FEVENT       fe;
	int numSamples = 0, numEvents = 0, maxItems = 100, type, eye, NUM_SAMPLE_FIELDS = 48, NUM_EVENT_FIELDS = 30, fieldNum, index;
	double *samples, *events;
	boolean useEye=FALSE, done=FALSE;
	
	//all sub functions should have these two lines
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
	
#if PSYCH_SYSTEM != PSYCH_LINUX
	
	//check to see if the user supplied superfluous arguments
	PsychErrorExit(PsychCapNumInputArgs(2));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	if(PsychCopyInIntegerArg(2, kPsychArgOptional, &maxItems)) {
		if (maxItems<=0) {
			PsychErrorExitMsg(PsychErorr_argumentValueOutOfRange, "EyeLink: GetQueuedItems:  max_items must be greater than zero\n");
		}
	}
	
	if (PsychCopyInIntegerArg(1, kPsychArgOptional, &eye)) {
		if (eye!=LEFT_EYE && eye!=RIGHT_EYE) {
			PsychErrorExitMsg(PsychErorr_argumentValueOutOfRange, "EyeLink: GetQueuedItems:  eye argument must be LEFT_EYE or RIGHT_EYE as returned by EyelinkInitDefaults\n");
		}
		useEye=TRUE;
	} else {
		mexPrintf("EYELINK: WARNING! Omission of the eye argument to GetQueuedItems is deprecated.\n");
	}
	
	samples = (double *)PsychMallocTemp(maxItems*NUM_SAMPLE_FIELDS*sizeof(double));
	events = (double *)PsychMallocTemp(maxItems*NUM_EVENT_FIELDS*sizeof(double));
	
	while(!done){
		type = eyelink_get_next_data(NULL);
		
		switch(type) {
			case SAMPLE_TYPE:                
				eyelink_get_float_data(&fs);
				if (useEye) {
					eyelink_get_extra_raw_values_v2(&fs, eye, &fr);
				} else {
					eyelink_get_extra_raw_values(&fs, &fr); //deprecated as of Dec 1, 2006 (see eyelink sdk core_expt.h)
				}
					index=PsychIndexElementFrom2DArray(NUM_SAMPLE_FIELDS, maxItems, 0, numSamples++);
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
				break;
				
			case LOST_DATA_EVENT: // queue overflowed, we are not supposed to call eyelink_get_float_data on this
				index=PsychIndexElementFrom2DArray(NUM_SAMPLE_FIELDS, maxItems, 0, numSamples++);
				for(fieldNum=0; fieldNum<NUM_SAMPLE_FIELDS; fieldNum++){
					samples[index++]= (double)((fieldNum==1) ? LOST_DATA_EVENT : MISSING_DATA);
				}
					break;
				
			case 0: // queue empty
				done=TRUE;
				break;
				
			default: // it is an event
				eyelink_get_float_data(&fe);
				index=PsychIndexElementFrom2DArray(NUM_EVENT_FIELDS, maxItems, 0, numEvents++);
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
		
		done = done || numSamples+numEvents >= maxItems;
	}
	
	//these only work for cutting off the unused parts of the buffers because matlab storage is columnwise
	//this is a little fragile - would be nice if PsychCopy* took the dims of the source and handled noncontiguous cases
	PsychCopyOutDoubleMatArg(1, kPsychArgOptional, NUM_SAMPLE_FIELDS, numSamples, 1, samples);
	PsychCopyOutDoubleMatArg(2, kPsychArgOptional, NUM_EVENT_FIELDS, numEvents, 1, events);
	
	return(PsychError_none);
#else
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, this function is not supported on Linux.");
#endif
}
