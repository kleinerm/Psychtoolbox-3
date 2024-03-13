/*
    PsychToolbox-3/PsychSourceGL/Source/Common/PsychCV/PsychCV.h

    PLATFORMS:  All

    AUTHORS:

    Mario Kleiner   mk  mario.kleiner.de@gmail.com

    HISTORY:

    5.01.2008       mk  wrote it.
*/

//begin include once
#ifndef PSYCH_IS_INCLUDED_PsychCV
#define PSYCH_IS_INCLUDED_PsychCV

//project includes
#include "Psych.h"
#include "PsychTimeGlue.h"
#include "PsychCVARToolkit.h"
#ifdef PSYCHCV_USE_APRILTAGS
#include "PsychCVAprilTags.h"
#endif


// Data structure for setting/returning/exchanging eye tracking data
// between PsychCV and the OpenEyes cvEyeTracker:
typedef struct PsychCVEyeResult {
    double gaze_x;
    double gaze_y;
    double pupil_x;
    double pupil_y;
    double cornea_x;
    double cornea_y;
    psych_bool validresult;
    double timestamp;
    unsigned int trackcount;
} PsychCVEyeResult;

// Internal helper functions:

// Startup function: Called at module load time / First time invocation:
void PsychCVInitialize(void);

// Module exit function: Called at shutdown/clear mex/... time:
PsychError PsychCVExit(void);

// Logger callback function to output OpenCV et al. debug messages at 'verbosity' > 5.
void PsychCVLogger(const char* msg);

// Setup help strings:
void InitializeSynopsis(void);

// Show command overview:
PsychError PSYCHCVDisplaySynopsis(void);

// Publically available functions:

PsychError MODULEVersion(void);
PsychError PSYCHCVVerbosity(void);
PsychError PSYCHCVCopyMatrixToMemBuffer(void);

#ifdef PSYCHCV_USE_OPENCV
// Helper functions from the OpenEyesCVEyeTracker submodule:
psych_bool cvEyeTrackerInitialize(const char* logfilename, int eyewidth, int eyeheight, int eyechannels, void** eyeInputImageMono8,
                                  void** eyeInputImageColor, int scenewidth, int sceneheight, void** sceneInputImageRGB8,
                                  void** ellipseOutputImageRGB8, void** thresholdOutputImageMono8);
psych_bool cvEyeTrackerShutdown(void);
psych_bool cvEyeTrackerExecuteTrackingCycle(PsychCVEyeResult* eyeResult, psych_bool useHighGUI);
void cvEyeTrackerSetPupilLocation(int px, int py);
void cvEyeTrackerAddCalibrationPoint(int px, int py);
void cvEyeTrackerRecalibrate(psych_bool resetCalib);
void cvEyeTrackerSetParameters(int pupilEdgeThreshold, int starburstRays, int minFeatureCandidates, int corneaWindowSize, int edgeThreshold,
                               int gaussWidth, double maxPupilEccentricity, double initialAngleSpread, double fanoutAngle1, double fanoutAngle2,
                               int featuresPerRay, int specialFlags);
void cvEyeTrackerSetRansacConstraints(double minDist, double maxDist, double minArea, double maxArea);
void cvEyeTrackerSetOverrideReferencePoint(int rx, int ry);

PsychError PSYCHCVOpenEyesInitialize(void);
PsychError PSYCHCVOpenEyesShutdown(void);
PsychError PSYCHCVOpenEyesTrackEyePosition(void);
PsychError PSYCHCVOpenEyesParameters(void);
#endif

//end include once
#endif
