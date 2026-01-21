/*
    PsychSourceGL/Source/Common/Screen/PsychFlipCallbacks.c

    PLATFORMS:

        All.
    
    AUTHORS:
        
        Alex Forrence     adf     alex.forrence@gmail.com
    
    HISTORY:
        1/16/26  adf    Created.

    DESCRIPTION:

        Implementation of flip callback scheduling and execution.

*/

#include "Screen.h"

// The head of the linked list of scheduled flip callbacks
static FlipCallbackNode *callbackList = NULL;

// Field names for the flip timing struct passed to callbacks (should match the fields returned by Screen('Flip'))
static const char *FieldNames[] = {"VBLTimestamp", "StimulusOnsetTime", "FlipTimestamp", "Missed", "Beampos"};

/*
Schedule a callback function to run immediately following a synchronous flip for a specific window.

The callback function and its arguments are persisted to survive until execution time.
*/
void PsychScheduleCallbackOnFlip(PsychWindowIndexType windowIndex, const PsychGenericScriptType *callback, int numArgs, const PsychGenericScriptType **args)
{
    FlipCallbackNode *newCallback = (FlipCallbackNode*) malloc(sizeof(FlipCallbackNode));

    if (!newCallback) {
        PsychErrorExitMsg(PsychError_outofMemory, NULL);
    }

    newCallback->windowIndex = windowIndex;
    newCallback->numArgs = numArgs;
    newCallback->args = (PsychGenericScriptType**) calloc(2 + numArgs, sizeof(PsychGenericScriptType*));
    if (!newCallback->args) goto cleanup;

    newCallback->args[0] = PsychPersist(callback);
    if (!newCallback->args[0]) goto cleanup;

    newCallback->args[1] = NULL; // `Screen('Flip')` return values will be stored here eventually

    if (numArgs > 0) {
        for (int i = 0; i < numArgs; i++) {
            newCallback->args[i + 2] = PsychPersist(args[i]);
            if (!newCallback->args[i + 2]) goto cleanup;
        }
    }

    newCallback->next = callbackList;
    callbackList = newCallback;
    return;

cleanup:
    freeNode(newCallback);
    PsychErrorExitMsg(PsychError_outofMemory, NULL);
}

// Clear all pending callback functions for a given window.
void PsychClearFlipCallbacks(PsychWindowIndexType windowIndex)
{
    FlipCallbackNode **node = &callbackList;
    FlipCallbackNode *current = NULL;

    while (*node) {
        current = *node;
        // Is this callback for the current window?
        if (current->windowIndex == windowIndex) {
            *node = current->next;
            freeNode(current);
        } else {
            node = &current->next;
        }
    }
}

// Clear all pending callback functions for all windows.
void PsychClearAllFlipCallbacks(void)
{
    if (!callbackList) return;

    FlipCallbackNode *current = callbackList;
    FlipCallbackNode *next = NULL;

    // Indiscriminately free all nodes
    while (current) {
        next = current->next;
        freeNode(current);
        current = next;
    }
    callbackList = NULL;
}

// When a synchronous flip completes for a given window, run all scheduled callbacks for that window.
void PsychRunFlipCallbacks(PsychWindowIndexType windowIndex, double vbl_timestamp, double time_at_onset, double time_at_flipend, double miss_estimate, double beamposatflip)
{
    if (!callbackList) return;

    FlipCallbackNode **node = &callbackList;
    FlipCallbackNode *current = NULL;
    PsychGenericScriptType *timeStruct = NULL;
    PsychAllocOutStructArray(kPsychNoArgReturn, kPsychArgOptional, -1, 5, FieldNames, &timeStruct);
    PsychSetStructArrayDoubleElement("VBLTimestamp", 0, vbl_timestamp, timeStruct);
    PsychSetStructArrayDoubleElement("StimulusOnsetTime", 0, time_at_onset, timeStruct);
    PsychSetStructArrayDoubleElement("FlipTimestamp", 0, time_at_flipend, timeStruct);
    PsychSetStructArrayDoubleElement("Missed", 0, miss_estimate, timeStruct);
    PsychSetStructArrayDoubleElement("Beampos", 0, beamposatflip, timeStruct);

    while (*node) {
        current = *node;
        // is this callback for the current window?
        if (current->windowIndex == windowIndex) {
            *node = current->next;
            PsychExecuteCallback(current, timeStruct);
            freeNode(current);
        } else {
            node = &current->next;
        }
    }
    // TODO: Implement the Python equivalent. Should just be a Py_DECREF?
    mxDestroyArray((mxArray*) timeStruct);
}

// Execute a single callback node with the provided timing struct.
void PsychExecuteCallback(FlipCallbackNode* node, const PsychGenericScriptType* timeStruct)
{
    node->args[1] = timeStruct;
    if (Psych_mexCallMATLAB(0, NULL, node->numArgs + 2, (mxArray**)node->args, "feval")) {
        // TODO: Capture name of the function that errored?
        printf("PTB-ERROR: Flip callback function failed.\n");
    }
    // remove the timeStruct so it's not freed when we delete the node
    node->args[1] = NULL;
}

void freeNode(FlipCallbackNode *node)
{
    if (!node) return;
    if (node->args) {
        for(int i=0; i < (2 + node->numArgs); i++) {
            if (node->args[i]) {
                PsychUnpersist(node->args[i]);
            }
        }
        free(node->args);
    }
    free(node);
}
