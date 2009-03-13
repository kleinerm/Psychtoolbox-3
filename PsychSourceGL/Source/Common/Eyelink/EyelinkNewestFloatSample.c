/*
 
	/osxptb/trunk/PsychSourceGL/Source/OSX/Eyelink/EyelinkNewestFloatSample.c
 
	PROJECTS: Eyelink 
 
	AUTHORS:
 cburns@berkeley.edu				cdb
 E.Peters@ai.rug.nl				emp
 f.w.cornelissen@med.rug.nl		fwc
 
	PLATFORMS:	Currently only OS X  
 
	HISTORY:
 
 2003	 emp		first version
 11/23/05  cdb		adapted.
 31-10-06	fwc		added EyelinkNewestFloatSampleRaw function
 19-02-09   edf     elaborated doc for EyelinkNewestFloatSampleRaw
 
	TARGET LOCATION:
 
 Eyelink.mexmac resides in:
 EyelinkToolbox
 
 */

#include "PsychEyelink.h"

static char useString[] = "sample = Eyelink('NewestFloatSample')";

static char synopsisString[] =
"makes copy of most recent float sample received"
"returns -1 if no new sample or error";

static char seeAlsoString[] = "";

/*
 ROUTINE: EYELINKnewestfloatsample
 PURPOSE:
 uses INT16 CALLTYPE eyelink_newest_float_sample(void FARTYPE *buf);
 makes copy of most recent float sample received
 makes FSAMPLE copy of most recent sample
 returns -1 if none or error, 0 if old, 1 if new  
 
 
 */   

PsychError EyelinkNewestFloatSample(void)
{
	FSAMPLE		structFloatSample;
	mxArray		**mxOutArg;
	int			iSampleType;
	
	// Add help strings
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}
	
	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(1));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	//mexPrintf("EyelinkSystemIsConnected\n");
	EyelinkSystemIsInitialized();
	//mexPrintf("EyelinkSystemIsInitialized\n");
	
	// Grab the sample
	iSampleType=eyelink_newest_float_sample(&structFloatSample);
	//mexPrintf("iSampleType is %d\n", iSampleType);
	
	mxOutArg = PsychGetOutArgMxPtr(1);
	
	if( iSampleType==1 || iSampleType==0 )
	{
		//		mexPrintf("old or new sample\n");
		*mxOutArg = (mxArray *)CreateMXFSample(&structFloatSample);
		
	}
	else
	{
		//		mexPrintf("error or no new sample\n");
		*mxOutArg = mxCreateDoubleMatrix(1,1,mxREAL);
		mxGetPr(*mxOutArg)[0]=iSampleType;
		
	}
	
	return(PsychError_none);
}


/* 
Special Raw function
 ON request of e_flister@yahoo.com
http://tech.groups.yahoo.com/group/psychtoolbox/message/5156
http://tech.groups.yahoo.com/group/psychtoolbox/message/5336 
 */
static char useStringRaw[] = "[sample, raw] = Eyelink('NewestFloatSampleRaw')";

static char synopsisStringRaw[] =
"Same as Eyelink('NewestFloatSample') but additionally returns raw sample data. "
"Normal samples do not contain the corneal reflection data, but some (non-saccade-based) calibration methods require this information. "
"The Eyelink 1000 can be configured to send this information as part of 'raw' samples. "
"Sol Simpson at SR-Research emphasizes that this is not officially supported or guaranteed. "
"1. You may need to install Eyelink.dll from the latest software developer kit at https://www.sr-support.com/forums/showthread.php?t=6 "
"2. Issue the commands: "
"   Eyelink('command','link_sample_data = LEFT,RIGHT,GAZE,AREA,GAZERES,HREF,PUPIL,STATUS,INPUT,HMARKER'); "
"   Eyelink('command','inputword_is_window = ON'); "
"More info: "
"   HMARKER (originally for Eyelink2's infrared head tracking markers) and INPUT (originally for the TTL lines) are jury-rigged to hold the extra data. "
"   This may cause lots of console output 'data too big 45'.  SR developer Suganthan Subraman says this can be ignored if you are not using velocity functions (eg eyelink_calculate_velocity_and_acceleration). "
"   You can also set file_sample_data to collect raw samples in the .edf file. "
"Raw structure fields: "
"   raw_pupil           raw x, y sensor position of the pupil "
"   raw_cr              raw x, y sensor position of the corneal reflection "
"   pupil_area          raw pupil area "
"   cr_area             raw cornela reflection area "
"   pupil_dimension     w,h of raw pupil "
"   cr_dimension        w,h of raw cr "
"   window_position     position of tracking window on sensor "
"   pupil_cr            calculated pupil-cr from the raw_pupil and raw_cr fields "
"CAUTION: Not supported on Linux. It may or may not work on your setup with your tracker. Currently it doesn't work on Linux at least. ";

PsychError EyelinkNewestFloatSampleRaw(void)
{
	FSAMPLE			structFloatSample;
	FSAMPLE_RAW		structFloatSampleRaw;
	mxArray			**mxOutArg[2];
	int				iSampleType;
	
	// Add help strings
	PsychPushHelp(useStringRaw, synopsisStringRaw, seeAlsoString);
	
	// Output help if asked
	if(PsychIsGiveHelp()) {
		PsychGiveHelp();
		return(PsychError_none);
	}
	
	// Check arguments
	PsychErrorExit(PsychCapNumInputArgs(0));
	PsychErrorExit(PsychRequireNumInputArgs(0));
	PsychErrorExit(PsychCapNumOutputArgs(2));
	
	// Verify eyelink is up and running
	EyelinkSystemIsConnected();
	EyelinkSystemIsInitialized();
	
	mxOutArg[0] = PsychGetOutArgMxPtr(1);
	mxOutArg[1] = PsychGetOutArgMxPtr(2);
	
#if PSYCH_SYSTEM != PSYCH_LINUX
	// Grab the sample
	iSampleType=eyelink_newest_float_sample(&structFloatSample);

	if( iSampleType==1 || iSampleType==0 )
	{
		//	mexPrintf("old or new sample\n");
		*mxOutArg[0] = (mxArray*) CreateMXFSample(&structFloatSample);
		eyelink_get_extra_raw_values(&structFloatSample, &structFloatSampleRaw);
		
		// Can we assume that there is always a raw sample?
		(*mxOutArg)[1] = (mxArray *)CreateMXFSampleRaw(&structFloatSampleRaw);
	}
	else
	{
		//		mexPrintf("error or no new sample\n");
		*mxOutArg[0] = mxCreateDoubleMatrix(1,1,mxREAL);
		mxGetPr((*mxOutArg[0]))[0]=iSampleType;
		*mxOutArg[1] = mxCreateDoubleMatrix(1,1,mxREAL);
		mxGetPr((*mxOutArg[1]))[0]=iSampleType;
		
	}
	
	return(PsychError_none);
#else
	PsychErrorExitMsg(PsychError_unimplemented, "Sorry, this function is not supported on Linux.");
#endif
}
