/*
    PsychToolbox-3/PsychSourceGL/Source/Common/PsychCV/PsychCVARToolkit.h

    PLATFORMS:  All

    AUTHORS:

    Mario Kleiner   mk      mario.kleiner.de@gmail.com

    HISTORY:

    19.04.09        mk      Initial implementation.
*/

//begin include once
#ifndef PSYCH_IS_INCLUDED_PsychCVARToolkit
#define PSYCH_IS_INCLUDED_PsychCVARToolkit

//project includes
#include "Psych.h"
#include "PsychTimeGlue.h"
#include "PsychCV.h"

PsychError PSYCHCVARInitialize(void);
PsychError PSYCHCVARShutdown(void);
PsychError PSYCHCVARLoadMarker(void);
PsychError PSYCHCVARDetectMarkers(void);
PsychError PSYCHCVARRenderImage(void);
PsychError PSYCHCVARTrackerSettings(void);
PsychError PSYCHCVARRenderSettings(void);

void PsychCVARExit(void);

//end include once
#endif
