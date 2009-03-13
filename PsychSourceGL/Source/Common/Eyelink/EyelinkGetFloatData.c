/*
    /osxptb/trunk/PsychSourceGL/Source/Common/Eyelink/EyelinkGetFloatData.c
 
    PROJECTS: Eyelink
 
    AUTHORS:
		f.w.cornelissen@rug.nl			fwc
		E.Peters@ai.rug.nl				emp
		e_flister@yahoo.com				edf
 
    PLATFORMS:	All
 
    HISTORY:
 
        29/05/2001	emp 		created it (OS9 version)
        30/10/06	fwc			Adapted from early alpha version.
        19/02/09    edf         added GetFloatDataRaw
 
    TARGET LOCATION:
 
        Eyelink.mexmac resides in:
            PsychHardware/EyelinkToolbox
 
/* WARNING returns only samples at the moment ??? doesn't look like that */


#include "PsychEyelink.h"

static char useString[] = "item = Eyelink('GetFloatData', type)";

static char synopsisString[] =
" makes copy of last item."
" You have to supply the type (returned from Eyelink('getnextdatatype'))."
" Returns the item.";

static char seeAlsoString[] = "";

/*
ROUTINE: EyelinkGetFloatData.c
PURPOSE:
   uses INT16 CALLTYPE eyelink_get_float_data(void FARTYPE *buf);
   makes copy of last item from getnextdatatype
   You have to supply the type (returned from eyelink('getnextdatatype')).
   Returns the item.*/

PsychError EyelinkGetFloatData(void)
{
    FSAMPLE   fs;
    FEVENT    fe;
    int type = 0;
    mxArray **mxpp;
    
    //all sub functions should have these two lines
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    
    //check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumOutputArgs(1));
    
    // Verify eyelink is up and running
    EyelinkSystemIsConnected();
    EyelinkSystemIsInitialized();
    
    PsychCopyInIntegerArg(1, TRUE, &type);
    
    mxpp = PsychGetOutArgMxPtr(1);
    
    switch(type)
    {
        case SAMPLE_TYPE:   
            eyelink_get_float_data(&fs);
            (*mxpp) = CreateMXFSample(&fs);
            break;
        case 0:             
            (*mxpp) = (mxArray*) mxCreateDoubleMatrix(1,1,mxREAL); //no data
            mxGetPr((*mxpp))[0] = 0;
            break;
        default:            
            eyelink_get_float_data(&fe);  // else it is an event
            (*mxpp) = CreateMXFEvent(&fe);
    }
    
    return(PsychError_none);
}


static char useStringRaw[] = "[item, raw] = Eyelink('GetFloatDataRaw', type)";

static char synopsisStringRaw[] =
"Same as Eyelink('GetFloatData') but additionally returns raw sample data. "
"If type is not SAMPLE_TYPE, identical to GetFloatData (raw will be returned as 0). "
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

/*
	ROUTINE: EyelinkGetFloatDataRaw.c
	PURPOSE: As EyelinkGetFloatData, but additionally returns raw sample data.
*/
PsychError EyelinkGetFloatDataRaw(void)
{
    FSAMPLE      fs;
    FSAMPLE_RAW  fr;
    FEVENT       fe;
    int type =   0;
    mxArray **mxpp[2];
    
    //all sub functions should have these two lines
    PsychPushHelp(useStringRaw, synopsisStringRaw, seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };

    // Check to see if the user supplied superfluous arguments
    PsychErrorExit(PsychCapNumInputArgs(1));
    PsychErrorExit(PsychRequireNumInputArgs(1));
    PsychErrorExit(PsychCapNumOutputArgs(2));
    
    // Verify eyelink is up and running
    EyelinkSystemIsConnected();
    EyelinkSystemIsInitialized();
    
    PsychCopyInIntegerArg(1, TRUE, &type);
    
    mxpp[0] = PsychGetOutArgMxPtr(1);
    mxpp[1] = PsychGetOutArgMxPtr(2);
    
#if PSYCH_SYSTEM != PSYCH_LINUX
    
    switch(type)
    {
        case SAMPLE_TYPE:   
            eyelink_get_float_data(&fs);
            (*mxpp)[0] = CreateMXFSample(&fs);
            eyelink_get_extra_raw_values(&fs, &fr);
            (*mxpp)[1] = (mxArray *)CreateMXFSampleRaw(&fr); //assumes there is always a raw sample...
            break;

        case 0:             
            (*mxpp)[0] = (mxArray*) mxCreateDoubleMatrix(1,1,mxREAL); //no data
            mxGetPr((*mxpp)[0])[0] = 0;
            (*mxpp)[1] = (mxArray*) mxCreateDoubleMatrix(1,1,mxREAL); //no raw
            mxGetPr((*mxpp)[0])[1] = 0;
            break;

        default:            
            eyelink_get_float_data(&fe);  // else it is an event
            (*mxpp)[0] = CreateMXFEvent(&fe);     
            (*mxpp)[1] = (mxArray*) mxCreateDoubleMatrix(1,1,mxREAL); //no raw
            mxGetPr((*mxpp)[0])[1] = 0;
    }
    
    return(PsychError_none);
	
#else
    PsychErrorExitMsg(PsychError_unimplemented, "Sorry, this function is not supported on Linux.");
#endif
}
