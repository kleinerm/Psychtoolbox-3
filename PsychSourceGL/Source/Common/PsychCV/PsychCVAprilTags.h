/*
    PsychToolbox-3/PsychSourceGL/Source/Common/PsychCV/PsychCVAprilTags.h

    PLATFORMS:  All

    AUTHORS:

    Mario Kleiner   mk      mario.kleiner.de@gmail.com

    HISTORY:

    12.03.2024      mk      Initial implementation.
*/

//begin include once
#ifndef PSYCH_IS_INCLUDED_PsychCVAprilTags
#define PSYCH_IS_INCLUDED_PsychCVAprilTags

//project includes
#include "Psych.h"
#include "PsychTimeGlue.h"
#include "PsychCV.h"

PsychError PSYCHCVAprilInitialize(void);
PsychError PSYCHCVAprilShutdown(void);
PsychError PSYCHCVAprilDetectMarkers(void);
PsychError PSYCHCVAprilSettings(void);
PsychError PSYCHCVApril3DSettings(void);

void PsychCVAprilExit(void);

//end include once
#endif
