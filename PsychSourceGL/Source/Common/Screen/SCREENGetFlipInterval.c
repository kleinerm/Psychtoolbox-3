/*
  SCREENGetFlipInterval.c

  AUTHORS:
  Allen.Ingling@nyu.edu                 awi
  mario.kleiner.de@gmail.com            mk

  PLATFORMS:    All


  HISTORY:
  03/10/02  awi         Created.
  05/09/05  mk          Put into use.

  DESCRIPTION:

   Returns the estimated/measured flip interval for the specified onscreen window surface.
   It either returns the value of a previous estimate (e.g., from Screen('OpenWindow',...)) or performs
   a measurement run.

*/

#include "Screen.h"

static char useString[] = "[ monitorFlipInterval nrValidSamples stddev ] =Screen('GetFlipInterval', windowPtr [, nrSamples] [, stddev] [, timeout]);";
static char synopsisString[] = 
    "Returns an estimate of the monitor flip interval for the specified onscreen window."
    "\"windowPtr\" is the handle of the onscreen window for which info should be returned. "
    "\"nrSamples\" If left out, the estimated interval from previous calls to GetFlipInterval "
    "or from the initial calibration done during Screen('OpenWindow'...) is reported. "
    "If set to a value greater zero, PTB will perform a measurement loop "
    "to compute a good estimate. The loop will run until at least nrSamples valid samples have "
    "been taken and standard deviation of the measurement is below \"stddev\" seconds. A timeout "
    "value \"timeout\" can be set to abort the measurement after timeout seconds if the measurement "
    "doesn't converge for some reason. Timeout is 10 seconds and stddev is 50 microseconds by default. "
    "PTB automatically takes at least 50 valid samples when opening an onscreen-window, trying to achieve a "
    "standard deviation below 100 microseconds, but timing out after a maximum of 15 seconds. "
    "If you require very high precision, call this routine with values that suit your demands. "
    "The returned monitorRefreshInterval is in seconds, but has sub-millisecond accuracy. "
    "The real number of valid samples taken and the final standard deviation is returned as well. "
    "CAUTION: When using OpenGL flip-frame stereo (stereomode=1 in OpenWindow) on ATI graphics hardware, "
    "the flip interval (as reported here) may be twice as long as the monitor refresh interval! Using "
    "Anti-Aliasing with a high multiSample level at a high display resolution may also cause the graphics "
    "hardware to switch to a flip-interval twice as long as the monitor refresh interval, because it is "
    "not capable of performing all anti-aliasing computations in one single refresh interval. ";

static char seeAlsoString[] = "OpenWindow, Flip, NominalFrameRate";

PsychError SCREENGetFlipInterval(void) 
{
    PsychWindowRecordType *windowRecord;

    double ifi_estimate=0;
    double maxsecs=10;         // Time-out after ten seconds by default.
    double stddev=0.00005;     // Require a std-deviation less than 50 microseconds by default..
    int nrSamples=0;
    double ifi_hint;

    //all subfunctions should have these two lines.  
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};

    PsychErrorExit(PsychCapNumInputArgs(4));     //The maximum number of inputs
    PsychErrorExit(PsychRequireNumInputArgs(1)); //The required number of inputs	
    PsychErrorExit(PsychCapNumOutputArgs(3));    //The maximum number of outputs

    //get the window record from the window record argument and get info from the window record
    PsychAllocInWindowRecordArg(kPsychUseDefaultArgPosition, TRUE, &windowRecord);

    if(windowRecord->windowType!=kPsychDoubleBufferOnscreen) {
        PsychErrorExitMsg(PsychError_user, "GetFlipInterval called on window without backbuffers.");
    }

    // Query number of valid samples to take for calibration. Defaults to zero = Just query, don't measure...
    PsychCopyInIntegerArg(2, FALSE, &nrSamples);
    if (nrSamples<0) {
        PsychErrorExitMsg(PsychError_user, "nrSamples must be greater or equal to zero.");
    }

    // Query threshold for stddev... Defaults 50 microseconds jitter.
    PsychCopyInDoubleArg(3, FALSE, &stddev);
    if (stddev<0.00002) {
        PsychErrorExitMsg(PsychError_user, "stddev be greater or equal to 0.00002 secs aka 20 microseconds.");
    }

    // Query timeout... Defaults to 10 seconds.
    PsychCopyInDoubleArg(4, FALSE, &maxsecs);
    if (maxsecs<=0) {
        PsychErrorExitMsg(PsychError_user, "maxsecs must be greater than zero seconds.");
    }

    // Query framerate / ifi-estimate from operating system:
    ifi_hint = PsychGetNominalFramerate(windowRecord->screenNumber);
    if (ifi_hint > 0) {
        // Valid nominal framerate reported. Use it as ifi_hint:
        ifi_hint = 1.0 / (double) ifi_hint;
    }
    else {
        // Invalid os reporting. Use ifi_beamestimate instead, as measured by
        // beamposition query based video refresh measurement during onscreen
        // window creation:
        ifi_hint = windowRecord->ifi_beamestimate;
    }

    ifi_estimate = PsychGetMonitorRefreshInterval(windowRecord, &nrSamples, &maxsecs, &stddev, ifi_hint, NULL);
    // Child protection:
    if (ifi_estimate==0) {
        PsychErrorExitMsg(PsychError_user, "GetFlipInterval failed to compute good estimate of monitor refresh! Somethings screwed up with VBL syncing!");
    }

    // Return the measured IFI:
    PsychCopyOutDoubleArg(1, FALSE, ifi_estimate);
    // Return number of valid samples taken:
    PsychCopyOutDoubleArg(2, FALSE, nrSamples);
    // Return standard deviation of measurement (jitter):
    PsychCopyOutDoubleArg(3, FALSE, stddev);

    // Done.
    return(PsychError_none);
}
