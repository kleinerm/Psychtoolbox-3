/*
	PsychToolbox3/Source/windows/Screen/PsychWindowGlue.h
	
	PLATFORMS:	
	
		This is the Windows version only.  
				
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		12/20/02		awi		Wrote it mostly by modifying SDL-specific refugees (from an experimental SDL-based Psychtoolbox).
		11/16/04		awi		Added description.  
		04/03/05                mk              Support for stereo display output and enhanced Screen('Flip') behaviour.
                05/09/05                mk              New function PsychGetMonitorRefreshInterval -- queries (and measures) monitor refresh.
	
        DESCRIPTION:
	
		Functions in this file comprise an abstraction layer for probing and controlling window state, except for window content.  
		
		Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
		should be platform-neutral, despite that the calls in OS X and Windows to detect available pixel sizes are different.  Platform 
		specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, PsychWindowTextClue.

	NOTES:
	
	TO DO: 
	
		¥ The "glue" files should should be suffixed with a platform name.  The original (bad) plan was to distingish platform-specific files with the same 
		name by their placement in a directory tree.
		
		¥ PsychFlushGL() and PsychGLFlush() need to be merged.

*/

//include once
#ifndef PSYCH_IS_INCLUDED_PsychWindowGlue
#define PSYCH_IS_INCLUDED_PsychWindowGlue

#include "Screen.h"

// Add missing defines from Vista SDK for the DWM, if not already defined:
#ifndef _DWMAPI_H_

typedef struct _UNSIGNED_RATIO {
    UINT32 uiNumerator;
    UINT32 uiDenominator;
} UNSIGNED_RATIO;

typedef ULONGLONG DWM_FRAME_COUNT;
typedef ULONGLONG QPC_TIME;

typedef  struct _DWM_TIMING_INFO
{
    UINT32          cbSize;

    // Data on DWM composition overall
    
    // Monitor refresh rate
    UNSIGNED_RATIO  rateRefresh;

    // Actual period
    QPC_TIME        qpcRefreshPeriod;

    // composition rate     
    UNSIGNED_RATIO  rateCompose;

    // QPC time at a VSync interupt
    QPC_TIME        qpcVBlank;

    // DWM refresh count of the last vsync
    // DWM refresh count is a 64bit number where zero is
    // the first refresh the DWM woke up to process
    DWM_FRAME_COUNT cRefresh;

    // DX refresh count at the last Vsync Interupt
    // DX refresh count is a 32bit number with zero 
    // being the first refresh after the card was initialized
    // DX increments a counter when ever a VSync ISR is processed
    // It is possible for DX to miss VSyncs
    //
    // There is not a fixed mapping between DX and DWM refresh counts
    // because the DX will rollover and may miss VSync interupts
    UINT cDXRefresh;

    // QPC time at a compose time.  
    QPC_TIME        qpcCompose;

    // Frame number that was composed at qpcCompose
    DWM_FRAME_COUNT cFrame;

    // The present number DX uses to identify renderer frames
    UINT            cDXPresent;

    // Refresh count of the frame that was composed at qpcCompose
    DWM_FRAME_COUNT cRefreshFrame;


    // DWM frame number that was last submitted
    DWM_FRAME_COUNT cFrameSubmitted;

    // DX Present number that was last submitted
    UINT cDXPresentSubmitted;

    // DWM frame number that was last confirmed presented
    DWM_FRAME_COUNT cFrameConfirmed;

    // DX Present number that was last confirmed presented
    UINT cDXPresentConfirmed;

    // The target refresh count of the last
    // frame confirmed completed by the GPU
    DWM_FRAME_COUNT cRefreshConfirmed;

    // DX refresh count when the frame was confirmed presented
    UINT cDXRefreshConfirmed;

    // Number of frames the DWM presented late
    // AKA Glitches
    DWM_FRAME_COUNT          cFramesLate;
    
    // the number of composition frames that 
    // have been issued but not confirmed completed
    UINT          cFramesOutstanding;


    // Following fields are only relavent when an HWND is specified
    // Display frame


    // Last frame displayed
    DWM_FRAME_COUNT cFrameDisplayed;

    // QPC time of the composition pass when the frame was displayed
    QPC_TIME        qpcFrameDisplayed; 

    // Count of the VSync when the frame should have become visible
    DWM_FRAME_COUNT cRefreshFrameDisplayed;

    // Complete frames: DX has notified the DWM that the frame is done rendering

    // ID of the the last frame marked complete (starts at 0)
    DWM_FRAME_COUNT cFrameComplete;

    // QPC time when the last frame was marked complete
    QPC_TIME        qpcFrameComplete;

    // Pending frames:
    // The application has been submitted to DX but not completed by the GPU
 
    // ID of the the last frame marked pending (starts at 0)
    DWM_FRAME_COUNT cFramePending;

    // QPC time when the last frame was marked pending
    QPC_TIME        qpcFramePending;

    // number of unique frames displayed
    DWM_FRAME_COUNT cFramesDisplayed;

    // number of new completed frames that have been received
    DWM_FRAME_COUNT cFramesComplete;

     // number of new frames submitted to DX but not yet complete
    DWM_FRAME_COUNT cFramesPending;

    // number of frames available but not displayed, used or dropped
    DWM_FRAME_COUNT cFramesAvailable;

    // number of rendered frames that were never
    // displayed because composition occured too late
    DWM_FRAME_COUNT cFramesDropped;
    
    // number of times an old frame was composed 
    // when a new frame should have been used
    // but was not available
    DWM_FRAME_COUNT cFramesMissed;
    
    // the refresh at which the next frame is
    // scheduled to be displayed
    DWM_FRAME_COUNT cRefreshNextDisplayed;

    // the refresh at which the next DX present is 
    // scheduled to be displayed
    DWM_FRAME_COUNT cRefreshNextPresented;

    // The total number of refreshes worth of content
    // for this HWND that have been displayed by the DWM
    // since DwmSetPresentParameters was called
    DWM_FRAME_COUNT cRefreshesDisplayed;
	
    // The total number of refreshes worth of content
    // that have been presented by the application
    // since DwmSetPresentParameters was called
    DWM_FRAME_COUNT cRefreshesPresented;


    // The actual refresh # when content for this
    // window started to be displayed
    // it may be different than that requested
    // DwmSetPresentParameters
    DWM_FRAME_COUNT cRefreshStarted;

    // Total number of pixels DX redirected
    // to the DWM.
    // If Queueing is used the full buffer
    // is transfered on each present.
    // If not queuing it is possible only 
    // a dirty region is updated
    ULONGLONG  cPixelsReceived;

    // Total number of pixels drawn.
    // Does not take into account if
    // if the window is only partial drawn
    // do to clipping or dirty rect management 
    ULONGLONG  cPixelsDrawn;

    // The number of buffers in the flipchain
    // that are empty.   An application can 
    // present that number of times and guarantee 
    // it won't be blocked waiting for a buffer to 
    // become empty to present to
    DWM_FRAME_COUNT      cBuffersEmpty;

} DWM_TIMING_INFO;

#endif


psych_bool PsychRealtimePriority(psych_bool enable_realtime);
psych_bool PsychOSOpenOnscreenWindow(PsychScreenSettingsType *screenSettings, PsychWindowRecordType *windowRecord, int numBuffers, int stereomode, int conserveVRAM);
psych_bool PsychOSOpenOffscreenWindow(double *rect, int depth, PsychWindowRecordType **windowRecord);
void	PsychOSCloseOnscreenWindow(PsychWindowRecordType *windowRecord);
void	PsychOSCloseWindow(PsychWindowRecordType *windowRecord);
void	PsychOSCloseOffscreenWindow(PsychWindowRecordType *windowRecord);
void    PsychOSFlipWindowBuffers(PsychWindowRecordType *windowRecord);
void    PsychOSSetVBLSyncLevel(PsychWindowRecordType *windowRecord, int swapInterval);
void	PsychOSSetGLContext(PsychWindowRecordType *windowRecord);
void	PsychOSUnsetGLContext(PsychWindowRecordType *windowRecord);
void	PsychOSSetUserGLContext(PsychWindowRecordType *windowRecord, psych_bool copyfromPTBContext);
double  PsychOSGetVBLTimeAndCount(PsychWindowRecordType *windowRecord, psych_uint64* vblCount);
void    PsychGetMouseButtonState(double* buttonArray);
psych_bool PsychOSGetPresentationTimingInfo(PsychWindowRecordType *windowRecord, psych_bool postSwap, unsigned int flags, psych_uint64* onsetVBLCount, double* onsetVBLTime, psych_uint64* frameId, double* compositionRate);

//end include once
#endif
