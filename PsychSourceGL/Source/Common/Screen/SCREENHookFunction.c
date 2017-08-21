/*
    SCREENHookFunction.c

    AUTHORS:

        mario.kleiner.de@gmail.com      mk

    PLATFORMS:

        All.

    HISTORY:

        12/05/06    mk        Wrote it.

    DESCRIPTION:

        Manage special callback functions (processing hooks). Screen allows to "hook" specific
        OpenGL GLSL shaders, C callback functions or Matlab M-File functions into its internal
        processing chain at well defined points in a (hopefully) well defined manner. This allows
        to extend or customize Screens behaviour. Useful applications: Automatic transparent
        on-the-fly image processing on stimuli just before stimulus onset, e.g., filtering.
        Customization of drawing commands. Advanced stereo stimulus output algorithms. Support for
        special output devices like BrightSide HDR, Bits++, Pelli & Zhang attenuator..., interfacing
        with 3rd party hardware like trigger systems...

        We provide well defined hook points, each one with a unique descriptive name. Each hook is
        associated with a queue or chain of callback functions which is executed from the beginning
        to the end whenever that hook is processed. A callback can be a GLSL OpenGL shader program
        for image processing, a Matlab M-Function for high-level processing or a C callback function
        for complex low-level or realtime operations like triggering some output device or similar.

        This Screen subfunction provides subfunctions to add callbacks to the chain, reset the chain,
        enable or disable the chain, query properties of single hooks or dump the whole chain state
        to the command window for debug purpose.
*/

#include "Screen.h"

// If you change useString then also change the corresponding synopsis string in ScreenSynopsis.c
static char useString[] = "[ret1, ret2, ...] = Screen('HookFunction', windowPtr, 'Subcommand', 'HookName', arg1, arg2, arg3, arg4);";
//                                                                    1           2             3          4     5     6     7
static char synopsisString[] =
    "Manage Screen processing hook chains. Hook chains are a way to extend PTBs behaviour with plugins for generic processing or "
    "fast image processing. They should allow to interface PTB seamlessly with 3rd party special response collection devices or "
    "stimulus display devices. They also allow to perform high performance image processing, utilizing modern graphics hardwares "
    "huge computational bandwidth. Read 'help PsychGLImageProcessing' for more infos."
    "\n\n"
    "\nSubsubcommands and their syntax: \n\n"
    "Screen('HookFunction', [windowPtr], 'ListAll'); \n"
    "Print out a listing of all supported 'HookName' hook points, including a short synopsis on what they do, to the Matlab console "
    "in a human readable format - Useful as a reference for coding. "
    "The 'windowPtr' argument is optional. If left out (replaced by empty [] brackets) it prints all implemented chains."
    "\n\n"
    "[slot idstring blittercfg voidptr glslid luttexid insertString] = Screen('HookFunction', windowPtr, 'Query', hookname, slotnameOrIndex);\n"
    "Query information about a specific command slot in a specific hook processing chain: "
    "'hookname' is the name of the chain to query, e.g., 'StereoCompositingBlit' for the stereo processing chain. Use the subcommand "
    "'ListAll' for a printout of all available processing hooks and a short help on them. "
    "'slotnameOrIndex', either the symbolic name of the requested slot or the index in the hook-chain: Indices start with 0. Names can "
    "be assigned to slots when you add a processing slot with the 'Append' or 'Prepend' command, or they are system assigned names like "
    ", e.g., 'StereoCompositingShader' for builtin shaders that are needed for stereo processing."
    "\n\n"
    "Return arguments, all optional: 'slot' is the index of the named slot in the chain to which the queried subfunction is assigned, or "
    "-1 if no such slot exists in the chain. 'glslid' numeric GLSL handle of the OpenGL GLSL shader object if the slot contains a "
    "GLSL shader for image processing on the GPU, 0 otherwise. 'luttexid' OpenGL texture handle of the first assigned lookup texture, "
    "0 if none assigned. 'voidptr' Memory pointer (encoded as double) to a C callback function, if one is assigned to this slot, 0 "
    "otherwise. 'blittercfg' either a parameter string with a meaning dependent of slot type, or the string 'NONE' if none assigned. "
    "'idstring' The symbolic name of this slot if any assigned, 'NONE' otherwise. 'insertString' the subcommand to use for reinserting "
    "this slot at the place it was after a deletion."
    "\n\n"
    "Screen('HookFunction', windowPtr, 'AppendShader', hookname, idString, glslid [, blittercfg] [luttexid1]); \n"
    "Append a new instruction slot to the end of hook processing chain 'hookname', assign the symbolic name 'idString' for later query "
    "by the 'Query' command. 'glslid' must be the name of a valid GLSL program object which defines the algorithm to apply on the GPU. "
    "'blittercfg' optional string with configuration commands for the shader. 'luttexid' Optional handle to an OpenGL texture which is "
    "used to encode lookup tables for the shader."
    "\n\n"
    "Screen('HookFunction', windowPtr, 'PrependShader', hookname idString, glslid [, blittercfg] [luttexid1]); \n"
    "Same as 'AppendShader' but add shader slot to beginning of the hook chain. It's recommended that you prepend slots instead of "
    "appending them, because PTB itself may add special slots at the end of a chain."
    "\n\n"
    "Screen('HookFunction', windowPtr, 'AppendCFunction', hookname, idString, voidfunctionptr); \n"
    "Screen('HookFunction', windowPtr, 'PrependCFunction', hookname, idString, voidfunctionptr); \n"
    "Attach a C callable function to the chain. voidfunctionptr is a double value which encodes a memory pointer to the function in "
    "memory. Encoding of the pointer and requirements to the function are non-trivial, so this is for expert developers only!"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'AppendMFunction', hookname, idString, fevalstring); \n"
    "Screen('HookFunction', windowPtr, 'PrependMFunction', hookname, idString, fevalstring); \n"
    "Add a Matlab callable function to the chain. 'fevalstring' is the function call string: It will be passed to "
    "and evaluated by Matlabs or Octaves feval() function, so it has to work with that function. It is not allowed to "
    "return any return arguments. Some special names in the string will be replaced by PTB with internal settings, think "
    "of it as a macro replacement. Caution: The called function is not allowed to call any Screen() commands or you'll "
    "likely get undefined behaviour or a crash -- Screen is not reentrant!"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'AppendBuiltin', hookname, idString, builtincmd); \n"
    "Screen('HookFunction', windowPtr, 'PrependBuiltin', hookname, idString, builtincmd); \n"
    "Add a call to a PTB built-in function 'builtincmd'."
    "\n\n"
    "You can also insert a hook function somewhere at a specific slot index via:\n"
    "Screen('HookFunction', windowPtr, 'InsertAtXXXYYY', ...);\n"
    "where XXX is a numeric slot id and YYY is the type specifier. Example: Insert a Builtin function\n"
    "at index 4 in hook 'UserDefinedBlit':\n"
    "Screen('HookFunction', windowPtr, 'InsertAt4Builtin', 'UserDefinedBlit', ...);\n"
    "\n\n"
    "Screen('Hookfunction', windowPtr, 'Remove', hookname, slotindex);\n"
    "Remove slot at index 'slotindex' in hookchain 'hookname'. The slot after this slot will move up by one.\n"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'Enable', hookname); \n"
    "Screen('HookFunction', windowPtr, 'Disable', hookname); \n"
    "Enable or disable a specific hook chain. Chains are disabled until you enable them, with the exception of a few "
    "internal chains that get initialized and enabled by PTB itself, e.g., stereo algorithm chain. Disabled chains "
    "are not processed."
    "\n\n"
    "Screen('HookFunction', windowPtr, 'Reset', hookname); \n"
    "Reset a processing hook chain: All slots are deleted, resetting the chain to its startup state. Seldomly needed. "
    "\n\n"
    "Screen('HookFunction', windowPtr, 'Dump', hookname); \n"
    "Print out the full chain for hook 'hookname' to the Matlab console in a human readable format - Useful for debugging."
    "\n\n"
    "Screen('HookFunction', windowPtr, 'DumpAll'); \n"
    "Print out all chains for the given onscreen window 'windowPtr' to the Matlab console in a human readable format - Useful for debugging."
    "\n\n"
    "oldImagingMode = Screen('HookFunction', proxyPtr, 'ImagingMode' [, imagingMode]); \n"
    "Change or query imagingMode flags of provided proxy window 'proxyPtr' to 'imagingMode'. Proxy windows are used to define "
    "image processing operations, mostly for Screen('TransformTexture'). Returns old imaging mode."
    "\n\n"
    "[leftglHandle, rightglHandle, glTextureTarget, format, multiSample, width, height] = Screen('HookFunction', windowPtr, 'SetDisplayBufferTextures' [, hookname][, leftglHandle][, rightglHandle][, glTextureTarget][, format][, multiSample][, width][, height]);\n"
    "Changes the external backing textures and their parameters for the final output color buffers of the imaging pipeline.\n"
    "Optionally returns the previously used backing textures and their old parameters. All new parameters are optional, the "
    "old values are left as they were if a parameter is not provided.\n"
    "This only works if imagingMode flags kPsychNeedFinalizedFBOSinks and kPsychUseExternalSinkTextures are set, "
    "otherwise external changes are rejected. It is not allowed to set a configuration which would cause the underlying "
    "framebuffers to become framebuffer incomplete. Trying to do so will fail and try to revert to the old framebuffer configuration. "
    "It is not allowed to switch from multisample backing textures to single-sample textures or vice versa, as that would cause various "
    "processing failures in the image post-processing for a stimulus. Setting the kPsychSinkIsMSAACapable imagingMode flag signals that "
    "the external sink is capable of providing multisample backing textures and desires such textures, otherwise only external single-sample "
    "textures are desired. The implementation will decide if it honors the request for multisample textures, depending on general imaging "
    "pipeline setup. It may decide to allow MSAA for rendering but perform the multiSample resolve step internally. In any case the presence "
    "of kPsychSinkIsMSAACapable in the final imagingMode for the window signals if the caller of this function should provide multisample or "
    "single sample non-power-of-two textures (GL_TEXTURE_2D_MULTISAMPLE vs. GL_TEXTURE_2D). The current implementation does not allow for a "
    "change in multiSample setting, ie., the effective number of samples per pixel for a backing texture. This limitation may get relaxed "
    "in future versions of the software if possible and sensible.\n"
    "Parameters and their meaning:\n"
    "'hookName' Currently ignored, placeholder for future extensions.\n"
    "'leftglHandle' OpenGL texture object handle of the left-eye texture in stereoMode 12, or of the mono-texture in mono mode.\n"
    "'rightglHandle' OpenGL texture object handle of the right-eye texture in stereoMode 12, or ignored in mono mode.\n"
    "'glTextureTarget' OpenGL texture target: GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE, depending on multisample configuration.\n"
    "'format' OpenGL internal texture format, e.g., GL_RGBA. Must be a framebuffer renderable format to prevent framebuffer incompleteness.\n"
    "'multiSample' The number of samples per texel. Must be 0 for single-sampled GL_TEXTURE_2D textures, > 0 for GL_TEXTURE_2D_MULTISAMPLE textures.\n"
    "'width' and 'height' Width and height of the output framebuffer (=texture) in pixels or texels.\n"
    "\n\n"
    "[leftglHandle, rightglHandle, glTextureTarget, format, multiSample, width, height] = Screen('HookFunction', windowPtr, 'GetDisplayBufferTextures');\n"
    "Get the OpenGL handles of the backing textures and their parameters for the final output color buffers of the imaging pipeline.\n"
    "For the meaning of return values see 'SetDisplayBufferTextures' above.\n"
    "This only works if imagingMode flag kPsychNeedFinalizedFBOSinks is set or stereoMode 12 is active, which implicitely sets that flag.\n"
    "This query function works both with internally generated and maintained backing textures and externally injected/maintained ones.\n"
    "For internally generated textures (without flag kPsychUseExternalSinkTextures), the handles should be considered read-only: Binding "
    "the textures for sampling/reading from them is appropriate, modifying them in any way is forbidden.\n"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'SetOneshotFlipFlags' [, hookname], flipFlags);\n"
    "Assign special flags to be applied one-time during the next execution of Screen('Flip') or Screen('AsyncFlipBegin').\n"
    "'hookname' is accepted, but currently ignored. Pass '' or [] for now.\n"
    "These 'flipFlags' will be applied during the next window flip operation, and each applied flag will then auto-reset "
    "after application. This is mostly meant to be called from within imaging pipeline processing chains during preflip "
    "operations or the active presentation sequence to modify behaviour of that sequence. The following 'flipFlags' are "
    "currently implemented: kPsychSkipVsyncForFlipOnce, kPsychSkipTimestampingForFlipOnce, kPsychSkipSwapForFlipOnce, kPsychSkipWaitForFlipOnce.\n"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'SetOneshotFlipResults' [, hookname], VBLTimestamp [, StimulusOnsetTime=VBLTimestamp][, Missed=0]);\n"
    "Assign override timestamp values to return from Screen('Flip') or Screen('AsyncFlipBegin').\n"
    "'hookname' is accepted, but currently ignored. Pass '' or [] for now.\n"
    "The provided timestamps will be applied during return from the next window flip operation and returned as the "
    "corresponding Screen('Flip') return values. You must have called the 'SetOneshotFlipFlags' HookFunction beforehand "
    "with at least the kPsychSkipTimestampingForFlipOnce flag to suppress internal timestamping.\n"
    "This is mostly meant to be called from within imaging pipeline processing chains, notably the "
    "'PreSwapbuffersOperations' hook chain, to inject stimulus onset timestamps provided by some external display client or "
    "external timestamping mechanism. The following values can be injected:\n"
    "'VBLTimestamp' The vbl timestamp of Flip completion, or something semantically equivalent, useful for Flip scheduling.\n"
    "'StimulusOnsetTime' Optional true stimulus onset time. Will be set to VBLTimestamp if omitted. Must be StimulusOnsetTime >= VBLTimestamp.\n"
    "'Missed' The presentation deadline miss estimate aka 'Missed' flag of Screen('Flip'). Defaults to 0 if omitted.\n"
    "\n\n"
    "Screen('HookFunction', windowPtr, 'SetWindowBackendOverrides' [, hookname][, pixelSize][, refreshInterval][, proj]);\n"
    "Assign override values for various window properties, as provided by the backend client instead of the windowing system.\n"
    "'hookname' is accepted, but currently ignored. Pass '' or [] for now.\n"
    "'pixelSize' The net color depth of the display, as returned by Screen('PixelSize', windowPtr);\n"
    "'refreshInterval' The video refresh interval duration in seconds, as reported by the display backend, and after proper translation "
    "returned by Screen('NominalFramerate', windowPtr), Screen('Framerate', windowPtr), and Screen('GetFlipInterval', windowPtr).\n"
    "'proj' Override projection matrix/matrices for 2D drawing: proj = [] == don't change, proj = 1 == Disable overrides, proj = 4x4 matrix "
    "for mono-mode drawing, proj = 4x4x2 matrices for separate matrices in stereo modes (:,:,1) left eye, (:,:,2) right eye.\n"
    "\n\n"
    "General notes:\n\n"
    "* Hook chains are per onscreen window, so each window can have a different configuration and enable state.\n"
    "* Read all available documentation on the Psychtoolbox imaging pipeline in 'help PsychGLImageprocessing', the PsychDocumentation folder "
    "and on the Wiki before you make use of this function. Its way to complex to use it by guessing ;)\n";

static char seeAlsoString[] = "";

PsychError SCREENHookFunction(void)
{
    PsychWindowRecordType       *windowRecord;
    char                        numString[10];
    char                        *cmdString, *hookString, *idString, *blitterString, *insertString;
    int                         cmd, slotid, flag1, whereloc = 0;
    int                         leftglHandle, rightglHandle, glTextureTarget, format, multiSample, width, height;
    double                      doubleptr;
    double                      shaderid, luttexid1 = 0;
    int                         n, m, p;
    double                      *dblmat;
    int                         verbosity = PsychPrefStateGet_Verbosity();

    blitterString = NULL;

    // All subfunctions should have these two lines.
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) { PsychGiveHelp(); return(PsychError_none); };

    PsychErrorExit(PsychCapNumInputArgs(10));
    PsychErrorExit(PsychRequireNumInputArgs(2));
    PsychErrorExit(PsychCapNumOutputArgs(7));

    // Get the subcommand string:
    PsychAllocInCharArg(2, kPsychArgRequired, &cmdString);

    // Subcommand dispatcher:
    cmd=0;
    if (strstr(cmdString, "Append"))  { cmd=1; whereloc = INT_MAX; }
    if (strstr(cmdString, "Prepend")) { cmd=2; whereloc = 0; }
    if (strcmp(cmdString, "Reset")==0)   cmd=3;
    if (strcmp(cmdString, "Enable")==0)  cmd=4;
    if (strcmp(cmdString, "Disable")==0) cmd=5;
    if (strcmp(cmdString, "Query")==0)   cmd=6;
    if (strcmp(cmdString, "Dump")==0)    cmd=7;
    if (strcmp(cmdString, "DumpAll")==0) cmd=8;
    if (strcmp(cmdString, "ListAll")==0) cmd=9;
    if (strcmp(cmdString, "Edit")==0)   cmd=10;
    if (strcmp(cmdString, "ImagingMode")==0) cmd=11;
    if (strstr(cmdString, "InsertAt")) { cmd=12; whereloc = -1; sscanf(cmdString, "InsertAt%i", &whereloc); }
    if (strstr(cmdString, "Remove")) cmd=13;
    if (strcmp(cmdString, "GetDisplayBufferTextures")==0) cmd=14;
    if (strcmp(cmdString, "SetDisplayBufferTextures")==0) cmd=15;
    if (strcmp(cmdString, "SetOneshotFlipFlags")==0) cmd=16;
    if (strcmp(cmdString, "SetOneshotFlipResults")==0) cmd=17;
    if (strcmp(cmdString, "SetWindowBackendOverrides")==0) cmd=18;

    if (cmd == 0) PsychErrorExitMsg(PsychError_user, "Unknown subcommand specified to 'HookFunction'.");
    if (whereloc < 0) PsychErrorExitMsg(PsychError_user, "Unknown/Invalid/Unparseable insert location specified to 'HookFunction' 'InsertAtXXX'.");

    // Need hook name?
    if (cmd!=9 && cmd!=8 && cmd!=11 && cmd!=14 && cmd!=15 && cmd!=16 && cmd!=17 && cmd!=18) {
        // Get it:
        PsychAllocInCharArg(3, kPsychArgRequired, &hookString);
    }

    // Get the window structure for the onscreen window.
    windowRecord = NULL;
    PsychAllocInWindowRecordArg(1, (cmd!=9) ? TRUE : FALSE, &windowRecord);

    switch(cmd) {
        case 1:  // Append:
        case 2:  // Prepend:
        case 12: // Insert at location 'whereloc':
            // Add a new hook function callback to chain, either at beginning or end.

            // What type of callback/handler is to be added?
            if(strstr(cmdString, "Shader")) {
                // GLSL shader program object:

                // The id string:
                PsychAllocInCharArg(4, kPsychArgRequired, &idString);

                // The shader object handle:
                PsychCopyInDoubleArg(5, TRUE, &shaderid);

                // The blitter config string:
                PsychAllocInCharArg(6, FALSE, &blitterString);

                // (Optionally) a texture handle for a texture to attach to 2nd unit:
                PsychCopyInDoubleArg(7, FALSE, &luttexid1);

                // Add shader:
                PsychPipelineAddShaderToHook(windowRecord, hookString, idString, whereloc, (unsigned int) shaderid, blitterString, (unsigned int) luttexid1);
            }
            else if(strstr(cmdString, "CFunction")) {
                // C callback function:

                // First the id string:
                PsychAllocInCharArg(4, kPsychArgRequired, &idString);

                // Then the void* to the function, encoded as a double value:
                PsychCopyInDoubleArg(5, TRUE, &doubleptr);

                // Add the function void* to the chain:
                PsychPipelineAddCFunctionToHook(windowRecord, hookString, idString, whereloc, PsychDoubleToPtr(doubleptr));
            }
            else if(strstr(cmdString, "MFunction")) {
                // Matlab/Octave/Whatever runtime environment callback function:

                // First the id string:
                PsychAllocInCharArg(4, kPsychArgRequired, &idString);

                // Then the call string (for feval()) for the function:
                PsychAllocInCharArg(5, TRUE, &blitterString);

                // Add the function to the chain:
                PsychPipelineAddRuntimeFunctionToHook(windowRecord, hookString, idString, whereloc, blitterString);
            }
            else if(strstr(cmdString, "Builtin")) {
                // Built in special function:

                // First the id string:
                PsychAllocInCharArg(4, kPsychArgRequired, &idString);

                // Then the options-string:
                PsychAllocInCharArg(5, TRUE, &blitterString);

                // Add the function to the chain:
                PsychPipelineAddBuiltinFunctionToHook(windowRecord, hookString, idString, whereloc, blitterString);
            }
            else {
                // Unknown?!?
                PsychErrorExitMsg(PsychError_user, "Unknown callback type specified to 'HookFunction'.");
            }
        break;

        case 3: // Reset hook-chain:
            PsychPipelineResetHook(windowRecord, hookString);
        break;

        case 4: // Enable hook-chain:
            PsychPipelineEnableHook(windowRecord, hookString);
        break;

        case 5: // Disable hook-chain:
            PsychPipelineDisableHook(windowRecord, hookString);
        break;

        case 6: // Query properties of a slot in a specific hook-chain:
            // Get the id string:
            if (PsychGetArgType(4)!=PsychArgType_char) {
                // No id string provided: Try to get numeric slot id and
                // turn it into an id string:
                PsychCopyInIntegerArg(4, TRUE, &slotid);
                sprintf(numString, "%i", slotid);
                idString = (char*) &numString[0];
                slotid = -1;
            }
            else {
                PsychAllocInCharArg(4, TRUE, &idString);
            }

            // Query everything that's there and copy it out:
            slotid = PsychPipelineQueryHookSlot(windowRecord, hookString, &insertString, &idString, &blitterString, &doubleptr, &shaderid, &luttexid1);

            // Copy out all infos:
            PsychCopyOutDoubleArg(1, FALSE, slotid);
            PsychCopyOutCharArg(2, FALSE, (idString) ? idString : "NONE");
            PsychCopyOutCharArg(3, FALSE, (blitterString) ? blitterString : "NONE");
            PsychCopyOutDoubleArg(4, FALSE, doubleptr);
            PsychCopyOutDoubleArg(5, FALSE, shaderid);
            PsychCopyOutDoubleArg(6, FALSE, luttexid1);
            PsychCopyOutCharArg(7, FALSE, (insertString) ? insertString : "NONE");
        break;

        case 7: // Dump specific hook-chain:
            PsychPipelineDumpHook(windowRecord, hookString);
        break;

        case 8: // Dump all hook-chains:
            PsychPipelineDumpAllHooks(windowRecord);
        break;

        case 9: // List all hook-chains:
            PsychPipelineListAllHooks(windowRecord);
        break;

        case 10: // Set properties of a slot in a specific hook-chain:
            // Get the id string:
            PsychAllocInCharArg(4, kPsychArgRequired, &idString);

            // Query everything that's there and copy it out:
            slotid = PsychPipelineQueryHookSlot(windowRecord, hookString, &insertString, &idString, &blitterString, &doubleptr, &shaderid, &luttexid1);
            if (slotid < 0) PsychErrorExitMsg(PsychError_user, "In 'Edit' No such hook slot in that hook chain for that object.");

            // Copy out all infos:
            PsychCopyOutDoubleArg(1, FALSE, slotid);
            PsychCopyOutCharArg(2, FALSE, (idString) ? idString : "NONE");
            PsychCopyOutCharArg(3, FALSE, (blitterString) ? blitterString : "NONE");
            PsychCopyOutDoubleArg(4, FALSE, doubleptr);
            PsychCopyOutDoubleArg(5, FALSE, shaderid);
            PsychCopyOutDoubleArg(6, FALSE, luttexid1);
        break;

        case 11: // Change imagingMode flags for a proxy window:
            if (windowRecord->windowType!=kPsychProxyWindow) PsychErrorExitMsg(PsychError_user, "In 'ImagingMode' Invalid windowPtr provided. Must be a proxy window!");
            PsychCopyOutDoubleArg(1, FALSE, (double) windowRecord->imagingMode);

            luttexid1 = -1;
            PsychCopyInDoubleArg(3, FALSE, &luttexid1);
            if (luttexid1 < 0 && luttexid1!=-1) PsychErrorExitMsg(PsychError_user, "In 'ImagingMode' Invalid imagingMode flags provided. Must be positive!");
            if (luttexid1!=-1) {
                windowRecord->imagingMode = (unsigned int) luttexid1;
            }
        break;

        // case 12 see at top.

        case 13: // Remove slot at given index.
            PsychCopyInIntegerArg(4, TRUE, &slotid);
            PsychPipelineDeleteHookSlot(windowRecord, hookString, slotid);
        break;

        case 14: // GetDisplayBufferTextures
            if (!PsychGetPipelineExportTexture(windowRecord, &leftglHandle, &rightglHandle, &glTextureTarget, &format, &multiSample, &width, &height) && (verbosity > 1))
                printf("PTB-WARNING: Invalid HookFunction call to GetDisplayBufferTextures! Not supported with current imagingMode. Trying to carry on - Prepare for trouble!\n");

            PsychCopyOutDoubleArg(1, FALSE, leftglHandle);
            PsychCopyOutDoubleArg(2, FALSE, rightglHandle);
            PsychCopyOutDoubleArg(3, FALSE, glTextureTarget);
            PsychCopyOutDoubleArg(4, FALSE, format);
            PsychCopyOutDoubleArg(5, FALSE, multiSample);
            PsychCopyOutDoubleArg(6, FALSE, width);
            PsychCopyOutDoubleArg(7, FALSE, height);
        break;

        case 15: // SetDisplayBufferTextures
            // Get old values and return them:
            if (!PsychGetPipelineExportTexture(windowRecord, &leftglHandle, &rightglHandle, &glTextureTarget, &format, &multiSample, &width, &height) && (verbosity > 1))
                printf("PTB-WARNING: Invalid HookFunction call to SetDisplayBufferTextures! Not supported with current imagingMode. Trying to carry on - Prepare for trouble!\n");

            PsychCopyOutDoubleArg(1, FALSE, leftglHandle);
            PsychCopyOutDoubleArg(2, FALSE, rightglHandle);
            PsychCopyOutDoubleArg(3, FALSE, glTextureTarget);
            PsychCopyOutDoubleArg(4, FALSE, format);
            PsychCopyOutDoubleArg(5, FALSE, multiSample);
            PsychCopyOutDoubleArg(6, FALSE, width);
            PsychCopyOutDoubleArg(7, FALSE, height);

            // Get new optional override values and set them:
            PsychCopyInIntegerArg(4, FALSE, &leftglHandle);
            PsychCopyInIntegerArg(5, FALSE, &rightglHandle);
            PsychCopyInIntegerArg(6, FALSE, &glTextureTarget);
            PsychCopyInIntegerArg(7, FALSE, &format);
            PsychCopyInIntegerArg(8, FALSE, &multiSample);
            PsychCopyInIntegerArg(9, FALSE, &width);
            PsychCopyInIntegerArg(10, FALSE, &height);
            if (!PsychSetPipelineExportTexture(windowRecord, leftglHandle, rightglHandle, glTextureTarget, format, multiSample, width, height) && (verbosity > 1))
                printf("PTB-WARNING: HookFunction call to SetDisplayBufferTextures failed. See above error message for details. Trying to carry on - Prepare for trouble!\n");
        break;

        case 16: // SetOneshotFlipFlags
            flag1 = 0;
            if (!PsychCopyInIntegerArg(4, FALSE, &flag1) && (verbosity > 1))
                printf("PTB-WARNING: HookFunction call to SetOneshotFlipFlags failed, because mandatory flipFlags parameter is missing.\n");

            if (flag1 & ~(kPsychSkipVsyncForFlipOnce | kPsychSkipTimestampingForFlipOnce | kPsychSkipSwapForFlipOnce | kPsychSkipWaitForFlipOnce)) {
                if (verbosity > 1)
                    printf("PTB-WARNING: HookFunction call to SetOneshotFlipFlags failed, because invalid/unsupported flipFlags were specified.\n");
            } else {
                windowRecord->specialflags |= flag1;

                // Any flag set that would prevent proper timestamping for the onscreen window in this cycle?
                if (flag1 & (kPsychSkipVsyncForFlipOnce | kPsychSkipTimestampingForFlipOnce | kPsychSkipSwapForFlipOnce)) {
                    // Invalidate windowRecord's bookkeeping:
                    windowRecord->time_at_last_vbl = 0;
                    windowRecord->rawtime_at_swapcompletion = 0;
                    windowRecord->postflip_vbltimestamp = -1;
                    windowRecord->osbuiltin_swaptime = 0;
                }
            }
        break;

        case 17: // SetOneshotFlipResults
            // Make sure internal timestamping is really off and override with our settings is allowed:
            if (!(windowRecord->specialflags & kPsychSkipTimestampingForFlipOnce)) {
                if (verbosity > 1)
                    printf("PTB-WARNING: HookFunction call to SetOneshotFlipResults failed, because flipFlags did not include kPsychSkipTimestampingForFlipOnce for this sequence.\n");
            } else {
                // Override allowed: Assign usercode provided values: These will be latched into 'Flip' return values during flip completion.

                // vblTimestamp of Flip, mandatory:
                if (!PsychCopyInDoubleArg(4, FALSE, &windowRecord->time_at_last_vbl) && (verbosity > 1))
                    printf("PTB-WARNING: HookFunction call to SetOneshotFlipResults failed, because mandatory 'VBLTimestamp' is missing!\n");

                // onsetTimestamp of Flip, optional, aliasese to vblTimestamp if omitted:
                if (!PsychCopyInDoubleArg(5, FALSE, &windowRecord->osbuiltin_swaptime))
                    windowRecord->osbuiltin_swaptime = windowRecord->time_at_last_vbl;

                // missEstimate of Flip, optional:
                if (!PsychCopyInDoubleArg(6, FALSE, &windowRecord->postflip_vbltimestamp))
                    windowRecord->postflip_vbltimestamp = 0;
            }
        break;

        case 18: // SetWindowBackendOverrides
            // Retrieve override values for onscreen window properties:
            if (!(windowRecord->imagingMode & kPsychNeedFinalizedFBOSinks))
                PsychErrorExitMsg(PsychError_user, "In 'SetWindowBackendOverrides': No kPsychNeedFinalizedFBOSinks imaging mode selected. Overrides forbidden!");

            // Screen('PixelSize'):
            if (PsychCopyInIntegerArg(4, FALSE, &flag1))
                windowRecord->depth = flag1;

            // Video refresh interval / refresh rate / flipinterval indicators:
            if (PsychCopyInDoubleArg(5, FALSE, &windowRecord->VideoRefreshInterval)) {
                windowRecord->ifi_beamestimate = windowRecord->VideoRefreshInterval;
                windowRecord->IFIRunningSum = windowRecord->VideoRefreshInterval;
                windowRecord->nrIFISamples = 1;
            }

            if (PsychAllocInDoubleMatArg(6, kPsychArgOptional, &n, &m, &p, &dblmat)) {
                // Release old ones, if any:
                if (windowRecord->proj) {
                    free(windowRecord->proj);
                    windowRecord->proj = NULL;
                }

                // Validate and assign:
                if (n * m * p == 1) {
                    // Empty assignment - No-op, as matrix is already NULL'ed above, just for clarity:
                    windowRecord->proj = NULL;
                }
                else {
                    // New override matrix assignment:
                    if (n !=4 || m != 4 || (p != 1 && p != 2))
                        PsychErrorExitMsg(PsychError_user, "Invalid 'proj' matrices specified. Must be a 4x4 matrix or a stack of two 4x4 matrices.");

                    windowRecord->proj = (double*) malloc(2 * 16 * sizeof(double));
                    memcpy(windowRecord->proj, dblmat, m * n * p * sizeof(double));
                }
            }
        break;
    }

    // Done.
    return(PsychError_none);
}
