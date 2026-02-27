#ifndef PSYCH_IS_INCLUDED_PsychFlipCallbacks
#define PSYCH_IS_INCLUDED_PsychFlipCallbacks

//includes
#include "Screen.h"

struct FlipCallbackNode;

typedef struct FlipCallbackNode {
    struct FlipCallbackNode *next;
    PsychGenericScriptType **args;
    PsychWindowIndexType windowIndex;
    int numArgs;
} FlipCallbackNode;

void PsychScheduleCallbackOnFlip(PsychWindowIndexType windowIndex, const PsychGenericScriptType *callback, int numArgs, const PsychGenericScriptType **args);
void PsychClearFlipCallbacks(PsychWindowIndexType windowIndex);
void PsychClearAllFlipCallbacks(void);
void PsychRunFlipCallbacks(PsychWindowIndexType windowIndex, double vbl_timestamp, double time_at_onset, double time_at_flipend, double miss_estimate, double beamposatflip);
void PsychExecuteCallback(FlipCallbackNode *node, const PsychGenericScriptType *timeStruct);
void freeNode(FlipCallbackNode *node);

#endif
