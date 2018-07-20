/*
  PsychToolbox3/Source/Common/PsychInit.c

  AUTHORS:

    Allen.Ingling@nyu.edu                 awi
    mario.kleiner.de@gmail.com            mk

  PLATFORMS: All

  PROJECTS:  All

  HISTORY:

  08/25/02  awi     wrote it.
  03/24/08  mk      Add call to PsychExitTimeGlue() and some cleanup.

  DESCRIPTION:

    Holds the master initialization and shutdown for the Psychtoolbox function.
    Sub-section hold their own inits which are called by the master
    init function contained herein.  After the Psychtoolbox library
    has initialzed itself in then invokes the project init which
    must be named PsychModuleInit().

    Each PsychToolbox module should register its subfuctions within
    its PsychModuleInit().

*/

#include "Psych.h"

#if PSYCH_LANGUAGE == PSYCH_PYTHON

#define PPYINIT(...) _PPYINIT(__VA_ARGS__)
#define _PPYNAME(n) #n
#define PPYNAME(...) _PPYNAME(__VA_ARGS__)

static PyMethodDef GlobalPythonMethodsTable[] = {
    {PPYNAME(PTBMODULENAME), PsychScriptingGluePythonDispatch, METH_VARARGS, "Make it so! Energize! Execute!"},
    {NULL, NULL, 0, NULL}
};

// Python 2 init code -- Python 2.6+ is required for PTB modules:
#if PY_MAJOR_VERSION < 3
#define _PPYINIT(n) PyMODINIT_FUNC init ## n(void)

// This is the entry point - module init function, called at module import:
// PTBMODULENAME is -DPTBMODULENAME myname defined by the build script to the
// name of the module, e.g., GetSecs.
PPYINIT(PTBMODULENAME)
{
    (void) Py_InitModule(PPYNAME(PTBMODULENAME), GlobalPythonMethodsTable);
}
// End of Python 2.x specific init code
#endif

// Python 3 init code:
#if PY_MAJOR_VERSION >= 3
#define _PPYINIT(n) PyMODINIT_FUNC PyInit_ ## n(void)

// Defined in PsychScriptingGluePython.c
PsychError PsychExitPythonGlue(void);

/* PythonModuleCleanup() - Call Python specific cleanup function.
 *
 * This cleanup function is only called on Python 3, and as far as i
 * understand only at interpreter shutdown time, ie. when calling quit()
 * or pressing CTRL+D. reload()ing of extension modules seems to be not
 * possible in Python 2 and 3, as described in PEP 0498:
 * https://www.python.org/dev/peps/pep-0489/#id29
 *
 * For this reason, this function is of limited value, but implemented
 * anyway for completeness and future reference.
 *
 */
void PythonModuleCleanup(void* userptr)
{
    (void) userptr;
    (void) PsychExitPythonGlue();
}

static struct PyModuleDef module_definition = {
    PyModuleDef_HEAD_INIT,                      // Base instance.
    PPYNAME(PTBMODULENAME),                     // Module name.
    "A Psychtoolbox module for Python 3",       // Help text.
    -1,                                         // -1 = No sub-interpreter support: https://docs.python.org/3/c-api/module.html#c.PyModuleDef
    GlobalPythonMethodsTable,                   // Function dispatch table.
    NULL,                                       // m_slots
    NULL,                                       // m_traverse
    NULL,                                       // m_clear
    PythonModuleCleanup                         // m_free = PythonModuleCleanup, cleanup at module destruction.
};

// This is the entry point - module init function, called at module import:
// PTBMODULENAME is -DPTBMODULENAME myname defined by the build script to the
// name of the module, e.g., GetSecs.
PPYINIT(PTBMODULENAME)
{
    return(PyModule_Create(&module_definition));
}

// End of Python 3.x specific init code
#endif

// End of Python init code.
#endif

PsychError PsychInit(void)
{
    // Assign distinctive name to master thread:
    PsychSetThreadName("PTB mainthread");

    // First init Psychtoolbox libraries:
    InitPsychError();
    InitPsychAuthorList();
    PsychInitTimeGlue();

    // Registration of the Psychtoolbox exit function is
    // done in PsychScriptingGlueXXX.c because how that is done is
    // specific to the scripting environment. Note that registration
    // of the project exit function is done within the project init.

    //then call call the project init.
    PsychModuleInit();

    return(PsychError_none);
}

/* PsychExit is the function invoked last before the module is
   purged. It is abstracted to be unspecific to the scripting
   language. The XXX language-specific versions are named
   PsychExitGlue() and they are found in PsychScriptingXXX.c
 */
PsychError PsychExit(void)
{
    PsychFunctionPtr projectExit;

    projectExit = PsychGetProjectExitFunction();
    if (projectExit != NULL) (*projectExit)();

    // Put whatever cleanup of the Psychtoolbox is required here.
    PsychExitTimeGlue();

    // Reset / Clear function and module name registry:
    PsychResetRegistry();

    return(PsychError_none);
}
