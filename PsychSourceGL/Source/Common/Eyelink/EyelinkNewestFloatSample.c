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
 now, the normal data structure returned from eyelink will give you
 pupil position, but not corneal reflection position. we've been
 working with sol simpson at sr research, who has given us access to an
 undocumented way to access the corneal reflection data.
 
 i've got it working in c, but am dreading integrating it into matlab.
 since it is extremely simple and of potential interest to other
 labs, i was hoping you'd be willing to add support to the eyelink
 toolbox (both windows and osx versions). sol said it was ok to share
 this info with the developers/users of the eyelink toolbox as long as
 i made it very clear that this functionality is unofficial and not
 guaranteed.
 
 
 */
static char useStringRaw[] = "[sample, raw] = Eyelink('NewestFloatSampleRaw')";

static char synopsisStringRaw[] =
"makes copy of most recent float sample received"
"and will copy extra (raw) data as well"
"returns -1 if no new sample or error";

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
}


