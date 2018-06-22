/*
 * PsychSourceGL/Source/Common/Base/PsychScriptingGluePython.c
 *
 * AUTHORS:
 *
 * mario.kleiner.de@gmail.com   mk
 *
 * PLATFORMS: All -- Glue layer for CPython runtime environment.
 *
 * HISTORY:
 * 19-June-2018     mk  Derived from PsychScriptingGlueMatlab.c
 *
 * DESCRIPTION:
 *
 * ScriptingGluePython defines abstracted functions to pass values
 * between the calling Python environment and the PsychToolbox.
 *
 */

// During inclusion of Psych.h, we define the special flag PTBINSCRIPTINGGLUE. This
// will cause some of the system headers in Psych.h not to be included during build
// of PsychScriptingGluePython.c:
#define PTBINSCRIPTINGGLUE 1
#include "Psych.h"
#undef PTBINSCRIPTINGGLUE

// Define this to 1 if you want lots of debug-output for the Octave-Scripting glue.
#define DEBUG_PTBPYTHONGLUE 1

// Special hacks to allow Psychtoolbox to build on Python, stubbing out Mex Api replacements.

#include <string.h>
#include <setjmp.h>

/*
typedef enum
{
    PsychArgType_unclassified = 0,
    PsychArgType_cellArray,
    PsychArgType_structArray,
    PsychArgType_boolean,
    PsychArgType_char,
    PsychArgType_double,
    PsychArgType_single,
    PsychArgType_int8,
    PsychArgType_uint8,
    PsychArgType_int16,
    PsychArgType_uint16,
    PsychArgType_int32,
    PsychArgType_uint32,
    PsychArgType_int64,
    PsychArgType_uint64,
}
*/

// This jump-buffer stores CPU- and stackstate at the position
// where our octFunction() dispatcher actually starts executing
// the functions or subfunctions code. That is the point where
// a PsychErrorExit() or PsychErrorExitMsg() will return control...
jmp_buf jmpbuffer;

// Error exit handler: Replacement for Matlabs MEX-handler:
// Prints the error-string with Octaves error printing facilities,
// sets Octave error state and longjmp's to the cleanup routine
// at the end of our octFunction dispatcher...
void mexErrMsgTxt(const char* s) {
    if (s && strlen(s) > 0)
        printf("%s:%s: %s\n", PsychGetModuleName(), PsychGetFunctionName(), s);
    else
        printf("%s:%s\n", PsychGetModuleName(), PsychGetFunctionName());

    // Use the jump-buffer to unwind the stack...
    longjmp(jmpbuffer, 1);
}

// Interface to Octave's printf...
void mexPrintf(const char* fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vfprintf(stderr, fmt, args);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void* mxMalloc(int size)
{
    return(PsychMallocTemp((unsigned long) size));
}

void* mxCalloc(int size, int numelements)
{
    return(PsychCallocTemp((unsigned long) size, (unsigned long) numelements));
}

void mxFree(void* p)
{
    PsychFreeTemp(p);
}

int mexCallMATLAB(const int nargout, PyObject* argout[],
                  const int nargin, const PyObject* argin[],
                  const char* fname)
{
    /* TODO FIXME */
    PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: Internal call to mexCallMATLAB(), which is not yet implemented on GNU/Octave port!");
}

double mxGetNaN(void)
{
    /* TODO FIXME Stop-gap? Better solution in Python? */
    return(nan(""));
}

int mxIsLogical(const PyObject* a)
{
    return(PyBool_Check(a));
}

int mxIsCell(const PyObject* a)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__);
    return(0);
}

int mxIsStruct(const PyObject* a)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__);
    return(0);
}

int mxIsNumeric(const PyObject* a)
{
    return(PyFloat_Check(a) || PyLong_Check(a));
}

int mxIsChar(const PyObject* a)
{
    return(PyString_Check(a));
}

int mxIsSingle(const PyObject* a)
{
    return(0);
}

int mxIsDouble(const PyObject* a)
{
    return(PyFloat_Check(a));
}

int mxIsUint8(const PyObject* a)
{
    return(0);
}

int mxIsUint16(const PyObject* a)
{
    return(0);
}

int mxIsUint32(const PyObject* a)
{
    return(0);
}

int mxIsInt8(const PyObject* a)
{
    return(0);
}

int mxIsInt16(const PyObject* a)
{
    return(0);
}

int mxIsInt32(const PyObject* a)
{
    return(PyLong_Check(a) || PyInt_Check(a));
}

PyObject* mxCreateNumericArray(int numDims, ptbSize dimArray[], PsychArgFormatType arraytype)
{
    PyObject* retval;
    int rows, cols, layers;

    if (numDims > 3)
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxCreateNumericArray: Tried to create matrix with more than 3 dimensions!");

    rows = dimArray[0];
    cols = (numDims > 1) ? dimArray[1] : 1;
    layers = (numDims > 2) ? dimArray[2] : 1;

    // TODO FIXME stopgap! Allocate our PyObject-Struct:
    retval = (PyObject*) PsychMallocTemp(sizeof(PyObject));

/* TODO FIXME
    dim_vector mydims((numDims>2) ? dim_vector(rows, cols, layers) : dim_vector(rows, cols));

    // Allocate our PyObject-Struct:
    retval = (PyObject*) PsychMallocTemp(sizeof(PyObject));

    // Create corresponding octave_value object for requested type and size of matrix.
    // Retrieve raw pointer to contained data and store it in our PyObject struct as well...

    if (arraytype==PsychArgType_uint8) {
        if (DEBUG_PTBPYTHONGLUE) printf("NEW UINT8 MATRIX: %i,%i,%i\n", rows, cols, layers); fflush(NULL);
        // Create empty uint8ND-Array of type mxREAL...
        uint8NDArray m(mydims);
        // Retrieve a pointer to internal representation. As m is new
        // this won't trigger a deep-copy.
        retval->d = (void*) m.data();
        if (DEBUG_PTBPYTHONGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
        // Build a new oct_value object from Matrix m: This is a
        // shallow-copy.
        octave_value* ovp = new octave_value();
        *ovp = m;
        retval->o = (void*) ovp;
        // At this point we can safely destroy Matrix m, as the new
        // octave_object holds a reference to its representation.
    }
    else if (arraytype==PsychArgType_double && rows*cols*layers > 1) {
        if (DEBUG_PTBPYTHONGLUE) printf("NEW DOUBLE MATRIX: %i,%i,%i\n", rows, cols, layers); fflush(NULL);
        // Create empty ND-Array of type mxREAL...
        NDArray m(mydims);
        // Retrieve a pointer to internal representation. As m is new
        // this won't trigger a deep-copy.
        retval->d = (void*) m.data();
        if (DEBUG_PTBPYTHONGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
        // Build a new oct_value object from Matrix m: This is a
        // shallow-copy.
        octave_value* ovp = new octave_value();
        *ovp = m;
        retval->o = (void*) ovp;
        // At this point we can safely destroy Matrix m, as the new
        // octave_object holds a reference to its representation.
    }
    else if (arraytype==PsychArgType_double && rows*cols*layers == 1) {
        if (DEBUG_PTBPYTHONGLUE) printf("NEW SCALAR:\n"); fflush(NULL);
        // This is a scalar value:
        retval->o = (void*) new octave_value(0.0);
        double* dp = (double*) PsychMallocTemp(sizeof(double));
        retval->d = (void*) dp;
    }
    else if (arraytype==PsychArgType_double && rows*cols*layers == 0) {
        // Special case: Empty matrix.
        if (DEBUG_PTBPYTHONGLUE) printf("NEW EMPTY DOUBLE MATRIX:\n"); fflush(NULL);
        retval->o = (void*) new octave_value(Matrix(0,0));
        retval->d = NULL;
    }
    else if (arraytype==PsychArgType_boolean) {
        if (DEBUG_PTBPYTHONGLUE) printf("NEW BOOLMATRIX: %i, %i\n", rows, cols, layers); fflush(NULL);
        // Create empty double-matrix of type mxREAL...
        if (layers>1) PsychErrorExitMsg(PsychError_internal, "In mxCreateNumericArray: Tried to allocate a 3D psych_bool matrix!?! Unsupported.");

        boolMatrix m(rows, cols);
        // Retrieve a pointer to internal representation. As m is new
        // this won't trigger a deep-copy.
        retval->d = (void*) m.data();
        // Build a new oct_value object from Matrix m: This is a
        // shallow-copy.
        octave_value* ovp = new octave_value();
        *ovp = m;
        retval->o = (void*) ovp;
        // At this point we can safely destroy Matrix m, as the new
        // octave_object holds a reference to its representation.
    }
    else {
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxCreateNumericArray: Unknown matrix type requested!");
    }
*/
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__);
    return(retval);
}

PyObject* mxCreateDoubleMatrix(int rows, int cols)
{
    int dims[2];
    dims[0]=rows;
    dims[1]=cols;
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(NULL);
/* TODO FIXME
    return(mxCreateNumericArray(2, dims, PsychArgType_double));
*/
}

PyObject* mxCreateLogicalMatrix(int rows, int cols)
{
    ptbSize dims[2];
    dims[0]=rows;
    dims[1]=cols;
    return(mxCreateNumericArray(2, dims, PsychArgType_boolean));
}

PyObject* mxCreateString(const char* instring)
{
    return(PyString_FromString(instring));
}

void* mxGetData(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(NULL);
/* TODO FIXME    return(arrayPtr->d); */
}

double mxGetScalar(const PyObject* arrayPtr)
{
    return((double) PyFloat_AsDouble(arrayPtr));
}

double* mxGetPr(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(NULL);
/* TODO FIXME    return(arrayPtr->d); */
}

psych_bool* mxGetLogicals(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(NULL);
/* TODO FIXME    return(arrayPtr->d); */

/*    return((psych_bool*) mxGetData(arrayPtr)); */
}

ptbSize mxGetM(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(1);
}

ptbSize mxGetN(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(1);
}

ptbSize mxGetNumberOfDimensions(const PyObject* arrayPtr)
{
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(1);
}

ptbSize* mxGetDimensions(const PyObject* arrayPtr)
{
    ptbSize* dims = (ptbSize*) PsychMallocTemp(3 * sizeof(ptbSize));
    dims[0]=dims[1]=dims[2]=1;
/*
    dims[0] = vdim(0);
    dims[1] = vdim(1);
    if (GETOCTPTR(arrayPtr)->ndims()>2) dims[2] = vdim(2); else dims[2]=1;
*/
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__);
    return(dims);
}

int mxGetString(const PyObject* arrayPtr, char* outstring, int outstringsize)
{
    if (!mxIsChar(arrayPtr))
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: Tried to convert a non-string into a string!");

    return(((snprintf(outstring, outstringsize, "%s", PyString_AsString(arrayPtr))) >= 0) ? 0 : 1);
}

void mxDestroyArray(PyObject *arrayPtr)
{
    // Destroy a PyObject:
    if (arrayPtr == NULL) return;

    // We only need to destroy the octave_value object referenced by arrayPtr,
    // because possible data buffers referenced by the ->d field and the
    // PyObject struct itself are allocted via PsychMallocTemp() anyway, so
    // they get automatically released when exiting our octFile...
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__);
/* TODO FIXME
    octave_value* ov = (octave_value*) arrayPtr->o;
    if (ov) delete(ov);
    arrayPtr->o = NULL;
*/
    return;
}

PyObject* mxCreateStructArray(int numDims, ptbSize* ArrayDims, int numFields, const char** fieldNames)
{
    PyObject* retval;

    if (numDims>2 || numDims<1) PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: mxCreateStructArray: Anything else than 1D or 2D Struct-Arrays is not supported!");
    if (numFields<1) PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxCreateStructArray: numFields < 1 ?!?");
    fprintf(stderr, "WARN WARN UNIMPLEMENTED: %s\n", __PRETTY_FUNCTION__); return(NULL);
/*
    // Our dimension vector:
    dim_vector mydims((numDims>1) ? dim_vector(ArrayDims[0], ArrayDims[1]) : dim_vector(ArrayDims[0]));

    // Create cell array of requested dimensionality and size as template. This
    // will be cloned numFields - times to create one such array per field.
    class Cell myCell(mydims);

    // Create an Octave_map(): A Octave_map is an associative map that associates keys (namestrings)
    // with values (which are Cell-Arrays). For each named field in our struct array, we insert a
    // key->value pair into the Octave_map. The key is the field name, the value is a copy of our
    // cell array 'myCell', where myCell has the requested dimensions.
    // Lookup or assignment will work as follows: The fieldName is used as search-key to lookup the
    // cell array associated with that key. The fieldIndex is then used to index into that cell array
    // and get or set the octave_value object which encodes the value of that field at that index...
    Octave_map mymap(std::string(fieldNames[0]), myCell);  // Assign first key in constructor.

    // Assign all remaining key->Cell pairs...
    for (int i=1; i<numFields; i++) mymap.assign(std::string(fieldNames[i]), myCell);

    // Build our PyObject wrapper:
    retval = (PyObject*) PsychMallocTemp(sizeof(PyObject));

    // Fill it: Assign our map.
    octave_value* ovp = new octave_value(mymap);
    retval->o = (void*) ovp;
    retval->d = NULL;

    return(retval);
*/
}

int mxGetFieldNumber(const PyObject* structArray, const char* fieldName)
{
/*
    if (!mxIsStruct(structArray)) {
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxGetFieldNumber: Tried to manipulate something other than a struct-Array!");
    }

    // Retrieve map:
    octave_value* ov = (octave_value*) structArray->o;
    Octave_map om = ov->map_value();

    // Find idx of key.
    std::string mykey(fieldName);
    for (int i=0; i<om.length(); i++) if (mykey == om.keys()(i)) return(i);
*/
    // No such key :(
    return(-1);
}

void mxSetField(PyObject* pStructOuter, int index, const char* fieldName, PyObject* pStructInner)
{
/*
    if (!mxIsStruct(pStructOuter)) {
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxSetField: Tried to manipulate something other than a struct-Array!");
    }
    // Retrieve map:
    octave_value* ov = (octave_value*) pStructOuter->o;
    Octave_map om = ov->map_value();

    // Retrieve object:
    octave_value* iv = (octave_value*) pStructInner->o;
    if (iv->is_real_type() && iv->is_scalar_type()) {
        // Our special case. Do the extra work...
        *iv=octave_value(*((double*) pStructInner->d));
    }

    // Assign our object:
    om.contents(std::string(fieldName))(index) = *iv;

    // Reassign our map:
    *ov = octave_value(om);
*/
}

PyObject* mxCreateCellArray(int numDims, ptbSize* ArrayDims)
{
    PyObject* retval;

    // Allocate our PyObject-Struct:
    retval = (PyObject*) PsychMallocTemp(sizeof(PyObject));

    if (numDims > 2) PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: mxCreateCellArray: 3D Cell Arrays are not supported yet on GNU/Octave build!");
/*
    // Create dimension vector:
    dim_vector mydims((numDims>1) ? dim_vector(ArrayDims[0], ArrayDims[1]) : dim_vector(ArrayDims[0]));

    // Create Cell object:
    Cell myCell(mydims);
    retval->o = (void*) new octave_value(myCell);
    retval->d = NULL;
*/
    // Done.
    return(retval);
}

void mxSetCell(PsychGenericScriptType *cellVector, ptbIndex index, PyObject* mxFieldValue)
{
    if (!mxIsCell(cellVector)) {
        PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxSetCell: Tried to manipulate something other than a cell-vector!");
    }
/*
    // Get a local (shallow) copy of the current real cellVector:
    octave_value* cv = (octave_value*) cellVector->o;
    Cell mycell = cv->cell_value();

    // Assign new mxFieldValue:
    octave_value* ov = (octave_value*) mxFieldValue->o;
    if (ov->is_real_type() && ov->is_scalar_type()) {
        // Our special case. Do the extra work...
        *ov=octave_value(*((double*) mxFieldValue->d));
    }

    mycell(index)=*ov;

    // Assign modified vector:
    *cv = mycell;
*/
    return;
}


void mxSetLogical(PyObject* dummy)
{
    // This is a no-op on Octave build, because it is not needed anywhere...
    return;
}


////Static functions local to ScriptingGluePython.c.
// _____________________________________________________________________________________

#define MAX_CMD_NAME_LENGTH 100

//Static variables local to ScriptingGluePython.c.  The convention is to append a abbreviation in all
//caps of the C file name to the variable name.

// nameFirstGLUE, baseFunctionInvoked, nlhsGLUE, nrhsGLUE, plhsGLUE, prhsGLUE
// are state which has to be maintained for each invocation of a mex module.
// If a modules calls itself recursively, this state has to be maintained for
// each recursive call level. We implement a little homemade stack for those
// variables. Maximum stack depth and therefore maximum recursion level for
// reentrant recursive calls is MAX_RECURSIONLEVEL. The variable recLevel
// keeps track of the current call recursion level and acts as a "stack pointer".
// It gets incremented by each entry to mexFunction() and decremented by each
// regular exit from mexFunction(). On error abort or modules reload it needs
// to get reset to initial -1 state:
#define MAX_RECURSIONLEVEL 5
#define MAX_INPUT_ARGS 100
#define MAX_OUTPUT_ARGS 100

static int recLevel = -1;
static psych_bool psych_recursion_debug = FALSE;

static psych_bool nameFirstGLUE[MAX_RECURSIONLEVEL];
static psych_bool baseFunctionInvoked[MAX_RECURSIONLEVEL];

static int nlhsGLUE[MAX_RECURSIONLEVEL];  // Number of requested return arguments.
static int nrhsGLUE[MAX_RECURSIONLEVEL];  // Number of provided call arguments.

static PyObject* plhsGLUE[MAX_RECURSIONLEVEL][MAX_OUTPUT_ARGS];         // An array of pointers to the Python return arguments.
static const PyObject *prhsGLUE[MAX_RECURSIONLEVEL][MAX_INPUT_ARGS];    // An array of pointers to the Python call arguments.

static void PsychExitGlue(void);

//local function declarations
static psych_bool PsychIsDefaultMat(const PyObject *mat);
static ptbSize mxGetP(const PyObject *array_ptr);
static ptbSize mxGetNOnly(const PyObject *arrayPtr);
static PyObject *mxCreateDoubleMatrix3D(psych_int64 m, psych_int64 n, psych_int64 p);

// firstTime: This flag defines if this is the first invocation of the module
// since it was (re-)loaded:
static psych_bool firstTime = TRUE;

PsychError PsychExitPythonGlue(void);
static psych_bool jettisoned = FALSE;

void ScreenCloseAllWindows(void);

void PsychExitRecursion(void)
{
    if (recLevel < 0) {
        printf("PTB-CRITICAL: Recursion stack underflow in module %s! Brace for impact!\n", PsychGetModuleName());
        return;
    }

    if (psych_recursion_debug) printf("PTB-DEBUG: Module %s leaving recursive call level %i.\n", PsychGetModuleName(), recLevel);

    // Done with this call recursion level:
    recLevel--;
}

/*
 *
 *    Main entry point for Python runtime. Serves as a dispatch and handles
 *    first time initialization.
 *
 *    EXP is a macro defined within Psychtoolbox source to be nothing
 *    except on win where it is the declaration which tells the linker to
 *    make the function visible from outside the DLL.
 *
 *        The subfunction dispatcher can operate in either of two modes depending
 *        on whether the module has registed subfunctions, or only a single "base"
 *        function.
 *
 *        subfunction mode:
 *        The examines the  first and second
 *        arguments for a string naming a module subfunction.  If it finds in either of those
 *        two arguments a string naming a module subfunctoin, then it looks up the approproate
 *        function pointer and invokes that function.  Before invoking the function the dispatcher
 *        removes the function name argument form the list of argumnets which was passed to the
 *        module.
 *
 *        base mode:  The dispatcher always invokes the same one subfunction and without
 *        alterinng the list of arguments.
 *
 *        Modules should now register in subfunction mode to support the build-in 'version' command.
 *
 * EXP void mexFunction(int nlhs, PyObject *plhs[], int nrhs, const PyObject *prhs[])
 *
 */
PyObject* PsychScriptingGluePythonDispatch(PyObject* self, PyObject* args)
{
    const char*         name;
    psych_bool          isArgThere[2], isArgEmptyMat[2], isArgText[2], isArgFunction[2];
    PsychFunctionPtr    fArg[2], baseFunction;
    char                argString[2][MAX_CMD_NAME_LENGTH];
    int                 i;
    psych_bool          errorcondition = FALSE;
    const PyObject*     tmparg = NULL; // PyObject is PyObject under MATLAB but #defined to octave_value on OCTAVE build.
    int                 nrhs = PyTuple_Size(args);
    const PyObject*     prhs = args;
    int                 nlhs = 0;
    PyObject*           plhs = NULL;

    // plhs is our octave_value_list of return values:
    //octave_value tmpval;      // Temporary, needed in parser below...
    //octave_value_list plhs;   // Our list of left-hand-side return values...


    // Child protection: Is someone trying to call us after we've shut down already?
    if (jettisoned) {
        // Yep! Stupido...
        printf("%s: Tried to call the module after it has been jettisoned!!! No op!\n", PsychGetModuleName());
        return (NULL);
    }

    #if 1
    // Save CPU-state and stack at this position in 'jmpbuffer'. If any further code
    // calls an error-exit function like PsychErrorExit() or PsychErrorExitMsg() then
    // the corresponding longjmp() call in our mexErrMsgTxt() implementation (see top of file)
    // will unwind the stack and restore stack-state and CPU state to the saved values in
    // jmpbuffer --> We will end up at this setjmp() call again, with a cleaned up stack and
    // CPU state, but setjmp will return a non-zero error code, signaling the abnormal abortion.
    if (setjmp(jmpbuffer) != 0) {
        // PsychErrorExit() or friends called! The CPU and stack are restored to a sane state.
        // Call our cleanup-routine to release memory that is PsychMallocTemp()'ed and to other
        // error-handling...
        errorcondition = TRUE;
        goto PythonFunctionCleanup;
    }
    #endif

    // Initialization
    if (firstTime) {
        // Reset call recursion level to startup default:
        recLevel = -1;
        psych_recursion_debug = TRUE;

        if (getenv("PSYCH_RECURSION_DEBUG")) psych_recursion_debug = TRUE;

        //call the Psychtoolbox init function, which inits the Psychtoolbox and calls the project init.
        PsychInit();

        // Hard to believe, but apparently true: Python-2 does not allow unloading extension modules!
        // So we register a subfunction "JettisonModuleHelper" that allows to trigger a manual module
        // shutdown, although not an unload.
        PsychRegister("JettisonModuleHelper",  &PsychExitPythonGlue);

        // Lock ourselves into Python-2 runtime environment so we can't get clear'ed out easily:
        // FIXME No such thing as locking in Python-2?

        // Register hidden helper function: This one dumps all registered subfunctions of
        // a module into a struct array of text strings. Needed by our automatic documentation
        // generator script to find out about subfunctions of a module:
        PsychRegister((char*) "DescribeModuleFunctionsHelper",  &PsychDescribeModuleFunctions);

        firstTime = FALSE;
    }

    // Increment call recursion level for this invocation of the module:
    recLevel++;
    if (recLevel >= MAX_RECURSIONLEVEL) {
        // Maximum level exceeded!
        printf("PTB-CRITICAL: Maximum recursion level %i for recursive calls into module '%s' exceeded!\n", recLevel, PsychGetModuleName());
        printf("PTB-CRITICAL: Aborting call sequence. Check code for recursion bugs!\n");
        recLevel--;
        PsychErrorExitMsg(PsychError_internal, "Module call recursion limit exceeded");
    }

    if (psych_recursion_debug) printf("PTB-DEBUG: Module %s entering recursive call level %i.\n", PsychGetModuleName(), recLevel);

    if (!PyTuple_Check(args)) {
        printf("FAIL FAIL FAIL!\n");
        return(NULL);
    }

    printf("nrhs: %i\n", nrhs);

    nrhsGLUE[recLevel] = nrhs;

    for (i = 0; i < nrhs; i++) {
        prhsGLUE[recLevel][i] = PyTuple_GetItem(args, i);

        if (PyString_Check(prhsGLUE[recLevel][i])) printf("%i: %s\n", i, PyString_AsString(prhsGLUE[recLevel][i]));
    }

    // Set number of output arguments to "unknown" == -1, as we don't know yet:
    nlhsGLUE[recLevel] = -1;

    // NULL-out our pointer array of return value pointers plhsGLUE[recLevel]:
    memset(&plhsGLUE[recLevel][0], 0, sizeof(plhsGLUE[recLevel]));

/*
    #if PSYCH_LANGUAGE == PSYCH_OCTAVE

            // NULL-init our pointer array of call value pointers prhsGLUE[recLevel]:
            memset(&prhsGLUE[recLevel][0], 0, sizeof(prhsGLUE[recLevel]));

            // Setup our prhsGLUE[recLevel] array of call argument pointers:
            // We make copies of prhs to simplify the rest of PsychScriptingGluePython. This copy is not
            // as expensive as it might look, because Octave objects are all implemented via
            // "Copy-on-write" --> Only a pointer is copied as long as we don't modify the data.
            // MK: TODO FIXME -- Should we keep an extra array octave_value dummy[MAX_INPUT_ARGS];
            // around, assign to that dummy[i]=prhs(i); and set ptrs to it prhsGLUE[recLevel][i]=&dummy[i];
            // This would require more memory, but prevent possible heap-fragmentation due to
            // lots of new()/delete() calls on each invocation of the OCT-Function --> possible
            // slow-down over time, could be confused with memory leaks???
            for (int i=0; i<nrhs && i<MAX_INPUT_ARGS; i++) {
                // Create and assign our PyObject-Struct:
                prhsGLUE[recLevel][i] = (PyObject*) PsychMallocTemp(sizeof(PyObject));

                // Extract data-pointer to each prhs(i) octave_value and store a type-casted version
                // which is optimal for us.
                if (prhs(i).is_string() || prhs(i).is_char_matrix()) {
                    // A string object:
                    if (DEBUG_PTBPYTHONGLUE) printf("INPUT %i: STRING\n", i); fflush(NULL);

                    // Strings do not have a need for a data-ptr. Just copy the octave_value object...
                    prhsGLUE[recLevel][i]->d = NULL;
                    prhsGLUE[recLevel][i]->o = (void*) new octave_value(prhs(i));  // Refcont now >= 2
                    // Done.
                }
                else if (prhs(i).is_real_type() && !prhs(i).is_scalar_type()) {
                    // A N-Dimensional Array:
                    if (DEBUG_PTBPYTHONGLUE) printf("TYPE NAME %s\n", prhs(i).type_name().c_str()); fflush(NULL);

                    // Is it an uint8 or int8 NDArray?
                    if (strstr(prhs(i).type_name().c_str(), "int8")) {
                        // Seems to be an uint8 or int8 NDArray: Create an optimized uint8 object of it:
                        if (DEBUG_PTBPYTHONGLUE) printf("INPUT %i: UINT8-MATRIX\n", i); fflush(NULL);

                        // Create intermediate representation m: This is a shallow-copy...
                        const uint8NDArray m(prhs(i).uint8_array_value()); // Refcount now >=2

                        // Get internal dataptr from it:        // This triggers a deep-copy :(
                        prhsGLUE[recLevel][i]->d = (void*) m.data();      // Refcount now == 1

                        // Create a shallow backup copy of corresponding octave_value...
                        octave_value* ovptr = new octave_value();
                        *ovptr = m;
                        prhsGLUE[recLevel][i]->o = (void*) ovptr;  // Refcont now == 2

                        // As soon as m gets destructed by leaving this if-branch,
                        // the refcount will drop to == 1...

                        // Done.
                    }
                    else {
                        // Seems to be a non-uint8 NDArray, i.e. psych_bool type or double type.
                        if (DEBUG_PTBPYTHONGLUE) printf("INPUT %i: DOUBLE-MATRIX\n", i); fflush(NULL);

                        // We create a generic double NDArray from it...

                        // Create intermediate representation m: This is a shallow-copy...
                        const NDArray m(prhs(i).array_value()); // Refcount now >=2

                        // Get internal dataptr from it:        // This triggers a deep-copy :(
                        prhsGLUE[recLevel][i]->d = (void*) m.data();      // Refcount now == 1

                        // Create a shallow backup copy of corresponding octave_value...
                        octave_value* ovptr = new octave_value();
                        *ovptr = m;
                        prhsGLUE[recLevel][i]->o = (void*) ovptr;  // Refcont now == 2

                        // As soon as m gets destructed by leaving this if-branch,
                        // the refcount will drop to == 1...

                        // Done.
                    }
                } else if (prhs(i).is_real_type() && prhs(i).is_scalar_type()) {
                    // A double or integer scalar value:
                    if (DEBUG_PTBPYTHONGLUE) printf("INPUT %i: SCALAR\n", i); fflush(NULL);
                    prhsGLUE[recLevel][i]->o = (void*) new octave_value(prhs(i));
                    // Special case: We allocate our own double value and store a
                    // copy of the value in it.
                    double* m = (double*) PsychMallocTemp(sizeof(double));
                    *m = prhs(i).double_value();
                    prhsGLUE[recLevel][i]->d = (void*) m;
                }
                else {
                    // Unkown argument type that we can't handle :(
                    // We abort with a reasonable error message:
                    prhsGLUE[recLevel][i]=NULL;
                    // We do, however, give an extra warning, as this could be Octave related...
                    printf("PTB-WARNING: One of the values in the argument list was not recognized.\n");
                    printf("PTB-WARNING: If your script runs well on Matlab then this may be a limitation or\n");
                    printf("PTB-WARNING: bug in the GNU/Octave version of Psychtoolbox :( ...\n");
                    PsychErrorExitMsg(PsychError_unimplemented, "Unrecognized argument in list of command parameters.");
                }
            }

            // NULL-out our pointer array of return value pointers plhsGLUE[recLevel]:
            memset(&plhsGLUE[recLevel][0], 0, sizeof(plhsGLUE[recLevel]));

            nrhsGLUE[recLevel] = nrhs;
            nlhsGLUE[recLevel] = nlhs;
    #endif
*/
    baseFunctionInvoked[recLevel] = FALSE;

    // If no subfunctions have been registered by the project then just invoke the project base function
    // If one of those has been registered.
    if (!PsychAreSubfunctionsEnabled()) {
        baseFunction = PsychGetProjectFunction(NULL);
        if (baseFunction != NULL) {
            baseFunctionInvoked[recLevel] = TRUE;
            (*baseFunction)();  //invoke the unnamed function
        } else
            PrintfExit("Project base function invoked but no base function registered");
    } else { // Subfunctions are enabled so pull out the function name string and invoke it.
        // Assess the nature of first and second arguments for finding the name of the sub function.
        for (i = 0; i < 2; i++) {
            isArgThere[i] = (nrhs > i) && (prhsGLUE[recLevel][i]);
            if (isArgThere[i]) {
                tmparg = prhsGLUE[recLevel][i];
            } else {
                tmparg = NULL;
            }
 
            isArgEmptyMat[i] = isArgThere[i] ? PsychIsDefaultMat(tmparg) : FALSE;
            isArgText[i] = isArgThere[i] ? mxIsChar(tmparg) : FALSE;
            if (isArgText[i]) {
                mxGetString(tmparg, argString[i], sizeof(argString[i]));
                // Only consider 2nd arg as subfunction if 1st arg isn't already a subfunction:
                if ((i == 0) || (!isArgFunction[0])) {
                    fArg[i] = PsychGetProjectFunction(argString[i]);
                }
                else fArg[i] = NULL; // 1st arg is subfunction, so 2nd arg can't be as well.
            }
            isArgFunction[i] = isArgText[i] ? fArg[i] != NULL : FALSE;
        }

        // Figure out which of the two arguments might be the function name and either invoke it or exit with error
        // if we can't find one.
        if (!isArgThere[0] && !isArgThere[1]) { //no arguments passed so execute the base function
            baseFunction = PsychGetProjectFunction(NULL);
            if (baseFunction != NULL) {
                baseFunctionInvoked[recLevel]=TRUE;
                (*baseFunction)();
            } else
                PrintfExit("Project base function invoked but no base function registered");
        }
        // (!isArgThere[0] && isArgEmptyMat[1]) --disallowed
        // (!isArgThere[0] && isArgText[1])     --disallowed
        // (!isArgThere[0] && !isArgText[1]     --disallowed except in case of !isArgThere[0] caught above.
        else if (isArgEmptyMat[0] && !isArgThere[1])
            PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state A)");
        else if (isArgEmptyMat[0] && isArgEmptyMat[1])
            PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state B)");
        else if (isArgEmptyMat[0] && isArgText[1]) {
            if (isArgFunction[1]) {
                nameFirstGLUE[recLevel] = FALSE;
                (*(fArg[1]))();
            }
            else
                PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state C)");
        }
        else if (isArgEmptyMat[0] && !isArgText[1])
            PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state D)");
        else if (isArgText[0] && !isArgThere[1]) {
            if (isArgFunction[0]) {
                nameFirstGLUE[recLevel] = TRUE;
                (*(fArg[0]))();
            } else { //when we receive a first argument  wich is a string and it is  not recognized as a function name then call the default function
                baseFunction = PsychGetProjectFunction(NULL);
                if (baseFunction != NULL) {
                    baseFunctionInvoked[recLevel]=TRUE;
                    (*baseFunction)();
                } else
                    PrintfExit("Project base function invoked but no base function registered");
            }
        }
        else if (isArgText[0] && isArgEmptyMat[1]) {
            if (isArgFunction[0]) {
                nameFirstGLUE[recLevel] = TRUE;
                (*(fArg[0]))();
            }
            else
                PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state F)");
        }
        else if (isArgText[0] && isArgText[1]) {
            if (isArgFunction[0] && !isArgFunction[1]) { //the first argument is the function name
                nameFirstGLUE[recLevel] = TRUE;
                (*(fArg[0]))();
            }
            else if (!isArgFunction[0] && isArgFunction[1]) { //the second argument is the function name
                nameFirstGLUE[recLevel] = FALSE;
                (*(fArg[1]))();
            }
            else if (!isArgFunction[0] && !isArgFunction[1]) { //neither argument is a function name
                //PrintfExit("Invalid command (error state G)");
                baseFunction = PsychGetProjectFunction(NULL);
                if (baseFunction != NULL) {
                    baseFunctionInvoked[recLevel]=TRUE;
                    (*baseFunction)();
                } else
                    PrintfExit("Project base function invoked but no base function registered");
            }
            else if (isArgFunction[0] && isArgFunction[1]) //both arguments are function names
                PrintfExit("Passed two function names");
        }
        else if (isArgText[0] && !isArgText[1]) {
            if (isArgFunction[0]) {
                nameFirstGLUE[recLevel] = TRUE;
                (*(fArg[0]))();
            }
            else
                PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state H)");
        }
        else if (!isArgText[0] && !isArgThere[1]) {  //this was modified for MODULEVersion with WaitSecs.
            //PrintfExit("Invalid command (error state H)");
            baseFunction = PsychGetProjectFunction(NULL);
            if (baseFunction != NULL) {
                baseFunctionInvoked[recLevel]=TRUE;
                (*baseFunction)();  //invoke the unnamed function
            } else
                PrintfExit("Project base function invoked but no base function registered");
        }
        else if (!isArgText[0] && isArgEmptyMat[1])
            PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state I)");
        else if (!isArgText[0] && isArgText[1])
        {
            if (isArgFunction[1]) {
                nameFirstGLUE[recLevel] = FALSE;
                (*(fArg[1]))();
            } else
                PrintfExit("Unknown or invalid subfunction name - Typo? Check spelling of the function name.  (error state J)");
        }
        else if (!isArgText[0] && !isArgText[1]) {  //this was modified for Priority.
            //PrintfExit("Invalid command (error state K)");
            baseFunction = PsychGetProjectFunction(NULL);
            if (baseFunction != NULL) {
                baseFunctionInvoked[recLevel]=TRUE;
                (*baseFunction)();  //invoke the unnamed function
            } else
                PrintfExit("Project base function invoked but no base function registered");
        }
    } //close else

    // If we reach this point of execution under Matlab, then we're successfully done.

#if 0
    // If we reach this point of execution under Octave, then we're done, possibly due to
    // error abort. Let's first do the memory management cleanup work necessary on Octave.
    // This is either done due to successfull execution or via jump to PythonFunctionCleanup:
    // in case of error-abort:
    PythonFunctionCleanup:

    // Release our own prhsGLUE[recLevel] array...
    // Release memory for scalar types is done by PsychFreeAllTempMemory();
    for (int i = 0; i < nrhs && i < MAX_INPUT_ARGS; i++) {
        if (prhsGLUE[recLevel][i]) {
            delete(((octave_value*)(prhsGLUE[recLevel][i]->o)));
            prhsGLUE[recLevel][i]=NULL;
        }
    }

    // "Copy" our octave-value's into the output array: If nlhs should be
    // zero (Octave-Script does not expect any return arguments), but our
    // subfunction has assigned a return argument in slot 0 anyway, then
    // we return that argument and release our own temp-memory. This
    // provides Matlab-semantic, where unsolicited return arguments are
    // printed anyway as content of the "ans" variable.
    for (i = 0; (i == 0 && plhsGLUE[recLevel][0] != NULL) || (i < nlhs && i < MAX_OUTPUT_ARGS); i++) {
        if (plhsGLUE[recLevel][i]) {
            plhs(i) = *((octave_value*)(plhsGLUE[recLevel][i]->o));
            if (plhs(i).is_scalar_type()) {
                // Special case: Scalar. Need to override with our double-ptrs value:
                double* svalue = (double*) plhsGLUE[recLevel][i]->d;
                plhs(i) = octave_value((double) *svalue);
            }

            // Delete our own octave_value object. All relevant data has been
            // copied via "copy-on-write" into plhs(i) already:
            delete(((octave_value*)(plhsGLUE[recLevel][i]->o)));

            // We don't need to free() the PsychMallocTemp()'ed object pointed to
            // by the d-Ptr, nor do we need to free the PyObject-Struct. This is done
            // below in PsychFreeAllTempMemory(). Just NULL-out the array slot:
            plhsGLUE[recLevel][i]=NULL;
        }
    }

    // Release all memory allocated via PsychMallocTemp():
    PsychFreeAllTempMemory();

    // Is this a successfull return?
    if (errorcondition) {
        // Nope - Error return, either due to some PTB detected error or due to
        // the user pressing the CTRL+C key combo. Try to call PTB's
        // Screen('CloseAll') command to close the display, at least if this is
        // the Screen module.

        // Is this the Screen() module?
        if (strcmp(PsychGetModuleName(), "Screen")==0) {
            // Yes. We directly call our close and cleanup routine:
            #ifdef PTBMODULE_Screen
            ScreenCloseAllWindows();
            #endif
        } else {
            // Nope. This is a Psychtoolbox OCT file other than Screen.
            // We can't call directly, but we can call the 'sca' command
            // from Octave:
            PsychRuntimeEvaluateString("Screen('CloseAll');");
        }
    }

    PsychExitRecursion();

    // Return our octave_value_list of returned values in any case and yield control
    // back to Octave:
    return(plhs);
#else

    // Find the true number of arguments to return in the return tuple:
    if (nlhsGLUE[recLevel] < 0) {
        for (i = 0; i < MAX_OUTPUT_ARGS; i++) {
            if (plhsGLUE[recLevel][i])
                nlhsGLUE[recLevel] = i + 1;
        }
    }

    // Multi-value return?
    if (nlhsGLUE[recLevel] > 1) {
        // Create an output tuple of suitable size:
        plhs = PyTuple_New((Py_ssize_t) nlhsGLUE[recLevel]);
        if (NULL == plhs) {
            printf("PTB-CRITICAL: Failed to create output arg return tuple!!\n");
            return(NULL);
        }

        printf("PTB-DEBUG: Output arg tuple with %i elements created.\n", nlhsGLUE[recLevel]);

        // "Copy" our return values into the output tuple: If nlhs should be
        // zero (Python-Script does not expect any return arguments), but our
        // subfunction has assigned a return argument in slot 0 anyway, then
        // we return that argument and release our own temp-memory. This
        // provides "Matlab"-semantic, where a first unsolicited return argument
        // is printed anyway to the console for diagnostic purpose:
        for (i = 0; (i == 0 && plhsGLUE[recLevel][0] != NULL) || (i < nlhsGLUE[recLevel]); i++) {
            if (plhsGLUE[recLevel][i]) {
                // Assign return argument to proper slot of tuple:
                if (PyTuple_SetItem(plhs, (Py_ssize_t) i, plhsGLUE[recLevel][i]))
                    printf("PTB-CRITICAL: Could not insert return argument for slot %i of output tuple!\n", i);

                // NULL-out the array slot, so only the output plhs tuple has a reference to
                // the output PyObject argument in slot i:
                plhsGLUE[recLevel][i] = NULL;
            }
            else {
                printf("PTB-DEBUG: Return argument for slot %i of output tuple not defined!\n", i);
                // Ref and assign empty return argument to proper slot of tuple:
                Py_INCREF(Py_None);
                if (PyTuple_SetItem(plhs, (Py_ssize_t) i, Py_None))
                    printf("PTB-CRITICAL: Could not insert return argument for slot %i of output tuple!\n", i);
            }
        }
    }
    else if ((nlhsGLUE[recLevel] == 1) && (plhsGLUE[recLevel][0] != NULL)) {
        // Single return argument:
        plhs = plhsGLUE[recLevel][0];
        plhsGLUE[recLevel][0] = NULL;
    }
    else {
        // No return value at all:
        Py_INCREF(Py_None);
        plhs = Py_None;
    }

    PsychExitRecursion();

PythonFunctionCleanup:

    // Release all memory allocated via PsychMallocTemp():
    PsychFreeAllTempMemory();

#endif

    // Return PyObject tuple with all return arguments:
    return(plhs);
}


/*  Call PsychExitGlue(), followed by unlocking the module:
 *  Needed to safely remove modules in Python 2.
 */
PsychError PsychExitPythonGlue(void)
{
    // Debug output:
    if (DEBUG_PTBPYTHONGLUE) printf("PTB-INFO: Jettisoning submodule %s ...\n", PsychGetModuleName());

    // Call our regular exit routines to clean up and release all ressources:
    PsychExitGlue();

    // Mark ourselves (via global variable "jettisoned") as shut-down. Any
    // further invocations of the module without previously clear'ing and
    // reloading it will be prevented.
    jettisoned = TRUE;

    // Unlock ourselves from Python runtime environment so we can get clear'ed out:
    // FIXME No such thing in Python 2?

    // Done. Return control to Octave - It will now remove us from its process-space - RIP.
    return(PsychError_none);
}


/*
 *    Just call the abstracted PsychExit function.  This might seem dumb, but its necessary to
 *    isolate the scripting language dependent stuff from the rest of the toolbox.
 *
 */
void PsychExitGlue(void)
{
    // Perform platform independent shutdown:
    PsychErrorExitMsg(PsychExit(),NULL);

    // And we are dead. Now the runtime will flush us from process memory.
    // No further invocation will happen until reload.
}


/*
 *    Return the PyObject pointer to the specified position. Note that we have some special rules for
 *    numbering the positions:
 *
 *    0 - This is always the command string or NULL if the project does not register a
 *        dispatch function and does accept subcommands.  If the function does accept sub
 *        commands, those may be passed in either the first or second position, but
 *        PsychGetArgPtr() will always return the command as the 0th.
 *
 *    1 - This is the first argument among the arguments which are not the subfunction name itself.
 *        It can occur in either the first or second position of the argument list, depending on
 *        in which of those two positions the function name itself appears.
 *
 *    2.. These positions are numbered correctly
 *
 *    TO DO:
 *
 *    2 - this function should be used by the one which gets the function name.
 *
 *
 *    Arguments are numbered 0..n.
 *
 *        - The 0th argument is a pointer to the PyObject holding
 *          the subfunction name string if we are in subfunction mode.
 *
 *        - The 0th argument is undefined if not in subfunction mode.
 *
 *        - The 1st argument is the argument of the 1st and 2nd which is not
 *          the subfunction name if in subfunction mode.
 *
 *        - The 1st argument is the first argument if not in subfunction mode.
 *
 *        - The 2nd-nth arguments are always the 2nd-nth arguments.
 */
//we return NULL if a position without an arg is specified.
const PyObject *PsychGetInArgPyPtr(int position)
{
    if (PsychAreSubfunctionsEnabled() && !baseFunctionInvoked[recLevel]) { //when in subfunction mode
        if (position < nrhsGLUE[recLevel]) { //an argument was passed in the correct position.
            if (position == 0) { //caller wants the function name argument.
                if (nameFirstGLUE[recLevel])
                    return(prhsGLUE[recLevel][0]);
                else
                    return(prhsGLUE[recLevel][1]);
            } else if (position == 1) { //they want the "first" argument.
                if (nameFirstGLUE[recLevel])
                    return(prhsGLUE[recLevel][1]);
                else
                    return(prhsGLUE[recLevel][0]);
            } else
                return(prhsGLUE[recLevel][position]);
        } else
            return(NULL);
    } else { //when not in subfunction mode and the base function is not invoked.
        if (position <= nrhsGLUE[recLevel])
            return(prhsGLUE[recLevel][position-1]);
        else
            return(NULL);
    }
}


PyObject **PsychGetOutArgPyPtr(int position)
{
    // Output argument PyObject
    if ((position == 1) ||
        ((position > 0) && ((position <= nlhsGLUE[recLevel]) || (nlhsGLUE[recLevel] == -1)))) {
        return(&(plhsGLUE[recLevel][position-1]));
    } else {
        printf("PTB-CRITICAL: PsychGetOutArgPyPtr() invalid position %i referenced [nlhs=%i], returning NULL!\n", position, nlhsGLUE[recLevel]);
        return(NULL);
    }
}


/* PsychCheckSizeLimits(size_t m, size_t n, size_t p)
 *
 * Makes sure matrix/vector dimensions stay within the limits imposed
 * by a specific Psychtoolbox build for a specific 32 bit or 64 bit
 * version of Python.
 *
 * 32 bit builds are always limited to 2^31-1 elements per dimension at
 * most. In reality the limit is much lower, this is an upper bound.
 *
 * 64 bit builds may or may not allow bigger limits, depending if
 * the Python runtime supports a 64 bit ptbSize type. Even
 * then there are some limits imposed by available system memory and
 * processor architecture specific limits, e.g., many cpu's only
 * truly support 48 bit of memory, not the true 64 bit.
 *
 */
void PsychCheckSizeLimits(psych_int64 m, psych_int64 n, psych_int64 p)
{
    // No problem if ptbSize has capacity to contain size_t:
    if (sizeof(ptbSize) >= sizeof(size_t)) return;

    // Mismatch: ptbSize too small to contain size_t. This means
    // ptbSize is only defined as a 32-bit int on a system where
    // size_t is a 64 bit unsigned int. As long as the provided
    // size_t values are smaller than 2^31 we're good and don't
    // screw up when downcasting. Otherwis we'd overflow and
    // disaster would happen:
    if ((m < INT_MAX) && (n < INT_MAX) && (p < INT_MAX)) return;

    // Ok, this is a no-go :( Abort with some hopefully helpful
    // error message:
    printf("PTB-ERROR: Tried to return a vector or matrix whose size along at least one dimension\n");
    printf("PTB-ERROR: exceeds the maximum supported size of 2^31 - 1 elements.\n");
    if (sizeof(size_t) == 4) {
        printf("PTB-ERROR: This is a limitation of all 32 bit versions of Psychtoolbox.\n");
        printf("PTB-ERROR: You'd need to use a Psychtoolbox for 64-bit Matlab or 64-bit Octave\n");
        printf("PTB-ERROR: on a 64-bit operating system to get rid of this limit.\n");
    }
    else {
        printf("PTB-ERROR: This is a limitation of your version of Octave or Matlab.\n");
        printf("PTB-ERROR: You'd need to use a Psychtoolbox for 64-bit Matlab or 64-bit Octave\n");
        printf("PTB-ERROR: on a 64-bit operating system to get rid of this limit.\n");
    }
    PsychErrorExitMsg(PsychError_user, "One of the dimensions of a returned matrix or vector exceeds 2^31-1 elements. This is not supported on your setup!");
}


/*
 *    Get the third array dimension which we call "P".  mxGetP should act just like mxGetM and mxGetN.
 *
 *    The abstracted Psychtoolbox API supports matrices with up to 3 dimensions.
 */
static ptbSize mxGetP(const PyObject *arrayPtr)
{
    const ptbSize *dimArray;

    if (mxGetNumberOfDimensions(arrayPtr) < 3) {
        printf("P %i\n", 1);
        return(1);
    }

    dimArray = (const ptbSize*) mxGetDimensions(arrayPtr);
    printf("P %i\n", dimArray[2]);

    return dimArray[2];
}


/*
 *    Get the 2nd array dimension.
 *
 *    The Mex API's mxGetN is sometimes undersirable because it returns the product of all dimensions above 1.  Our mxGetNOnly only returns N, for when you need that.
 *
 *    The abstracted Psychtoolbox API supports matrices with up to 3 dimensions.
 */
static ptbSize mxGetNOnly(const PyObject *arrayPtr)
{
    const ptbSize *dimArray;

    dimArray = (const ptbSize*) mxGetDimensions(arrayPtr);
    printf("NOnly %i\n", dimArray[1]);
    return dimArray[1];
}


/*
 *    mxCreateDoubleMatrix3D()
 *
 *    Create a 2D or 3D matrix of doubles.
 *
 *    Requirements are that m>0, n>0, p>=0.
 */
PyObject *mxCreateDoubleMatrix3D(psych_int64 m, psych_int64 n, psych_int64 p)
{
    int numDims;
    ptbSize dimArray[3];

    if (m == 0 || n == 0) {
        dimArray[0] = 0; dimArray[1] = 0; dimArray[2] = 0;    //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims = (p==0 || p==1) ? 2 : 3;

    return mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_double);
}


/*
 *    mxCreateFloatMatrix3D()
 *
 *    Create a 2D or 3D matrix of floats.
 *
 *    Requirements are that m>0, n>0, p>=0.
 */
PyObject *mxCreateFloatMatrix3D(size_t m, size_t n, size_t p)
{
    int numDims;
    ptbSize dimArray[3];

    if (m==0 || n==0 ) {
        dimArray[0]=0;dimArray[1]=0;dimArray[2]=0;    //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims= (p==0 || p==1) ? 2 : 3;

    return mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_single);
}


/*
 *    mxCreateNativeBooleanMatrix3D()
 *
 *    Create a 2D or 3D matrix of native psych_bool types.
 *
 *    Requirements are that m>0, n>0, p>=0.
 */
PyObject *mxCreateNativeBooleanMatrix3D(size_t m, size_t n, size_t p)
{
    int         numDims;
    ptbSize      dimArray[3];
    PyObject     *newArray;

    if (m==0 || n==0 ) {
        dimArray[0]=0;dimArray[1]=0;dimArray[2]=0;    //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims = (p==0 || p==1) ? 2 : 3;
    newArray = mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_boolean);

    return(newArray);
}


/*
 *    Create a 2D or 3D matrix of ubytes.
 *
 *    Requirements are that m>0, n>0, p>=0.
 */
PyObject *mxCreateByteMatrix3D(size_t m, size_t n, size_t p)
{
    int numDims;
    ptbSize dimArray[3];

    if (m==0 || n==0 ) {
        dimArray[0]=0;dimArray[1]=0;dimArray[2]=0; //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims= (p==0 || p==1) ? 2 : 3;
    return mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_uint8);
}


/*
 *    Print string s and return return control to the calling environment.
 */
void PsychErrMsgTxt(char *s)
{
    // Is this the Screen() module?
    if (strcmp(PsychGetModuleName(), "Screen")==0) {
        // Yes. We directly call our close and cleanup routine:
        #ifdef PTBMODULE_Screen
            ScreenCloseAllWindows();
        #endif
    } else {
        // Nope. This is a Psychtoolbox MEX file other than Screen.
        // We can't call directly, but we can call the 'sca' command
        // from Matlab:
        PsychRuntimeEvaluateString("Screen('CloseAll');");
    }

    PsychExitRecursion();

    // Call the Matlab- or Octave error printing and error handling facilities:
    mexErrMsgTxt((s && (strlen(s) > 0)) ? s : "See error message printed above.");
}


/*
 *    classify the PyObject element format using Pyschtoolbox argument type names
 *
 */
static PsychArgFormatType PsychGetTypeFromPyPtr(const PyObject *ppyPtr)
{
    PsychArgFormatType format;

    if (PsychIsDefaultMat(ppyPtr))
        format = PsychArgType_default;
    else if (mxIsUint8(ppyPtr))
        format = PsychArgType_uint8;
    else if (mxIsUint16(ppyPtr))
        format = PsychArgType_uint16;
    else if (mxIsUint32(ppyPtr))
        format = PsychArgType_uint32;
    else if (mxIsInt8(ppyPtr))
        format = PsychArgType_int8;
    else if (mxIsInt16(ppyPtr))
        format = PsychArgType_int16;
    else if (mxIsInt32(ppyPtr))
        format = PsychArgType_int32;
    else if (mxIsDouble(ppyPtr))
        format = PsychArgType_double;
    else if (mxIsSingle(ppyPtr))
        format = PsychArgType_single;
    else if (mxIsChar(ppyPtr))
        format = PsychArgType_char;
    else if (mxIsCell(ppyPtr))
        format = PsychArgType_cellArray;
    else if (mxIsLogical(ppyPtr))
        // This is tricky because MATLAB abstracts "logicals" conditionally on platform.
        // Depending on OS, MATLAB implements booleans with either 8-bit or 64-bit values.
        format = PsychArgType_boolean;
    else
        format = PsychArgType_unclassified;

    return format;
}


/*
 *    PsychSetReceivedArgDescriptor()
 *
 *    Accept an argument number and direction value (input or output).  Examine the specified argument and fill in an argument
 *    descriptor struture.  Ask a retainer function to store the descriptor.
 *
 */
PsychError PsychSetReceivedArgDescriptor(int argNum, psych_bool allow64BitSizes, PsychArgDirectionType direction)
{
    PsychArgDescriptorType  d;
    int                     numNamedOutputs, numOutputs;
    const PyObject          *ppyPtr;

    d.position = argNum;
    d.direction = direction;
    if (direction == PsychArgIn) {
        ppyPtr = PsychGetInArgPyPtr(argNum);
        d.isThere = (ppyPtr && !PsychIsDefaultMat(ppyPtr)) ? kPsychArgPresent : kPsychArgAbsent;
        if (d.isThere == kPsychArgPresent) { //the argument is there so fill in the rest of the description
            d.numDims = (int) mxGetNumberOfDimensions(ppyPtr);

            // If the calling function doesn't allow 64 bit sized input argument dimensions, then we check if
            // the input has a size within the positive signed integer range, i.e., at most INT_MAX elements
            // per dimension. Functions which can handle bigger inputs need to declare this explicitely by
            // setting allow64BitSizes == TRUE:
            if ((!allow64BitSizes) && ((mxGetM(ppyPtr) >= INT_MAX) || (mxGetNOnly(ppyPtr) >= INT_MAX) || (mxGetP(ppyPtr) >= INT_MAX))) {
                printf("PTB-ERROR: %i. Input argument exceeds allowable maximum size of 2^31 - 1 elements\n", argNum);
                printf("PTB-ERROR: in at least one dimension. Psychtoolbox can't handle such huge matrices or vectors.\n");
                PsychErrorExitMsg(PsychError_user, "Input argument exceeds maximum supported count of 2^31 - 1 elements!");
            }

            d.mDimMin = d.mDimMax = (psych_int64) mxGetM(ppyPtr);
            d.nDimMin = d.nDimMax = (psych_int64) mxGetNOnly(ppyPtr);
            d.pDimMin = d.pDimMax = (psych_int64) mxGetP(ppyPtr);
            d.type = PsychGetTypeFromPyPtr(ppyPtr);
        }
    }
    else { //(direction == PsychArgOut)
        numNamedOutputs = PsychGetNumNamedOutputArgs();
        numOutputs = PsychGetNumOutputArgs();
        if (numNamedOutputs >=argNum)
            d.isThere = kPsychArgPresent;
        else if (numOutputs >=argNum)
            d.isThere = kPsychArgFixed;
        else
            d.isThere = kPsychArgAbsent;
    }

    PsychStoreArgDescriptor(NULL,&d);
    return(PsychError_none);
}


//local function definitions for ScriptingGluePython.c
//___________________________________________________________________________________________


psych_bool PsychIsDefaultMat(const PyObject *mat)
{
    return (mat == Py_None);
}


//functions for project access to module call arguments (MATLAB)
//___________________________________________________________________________________________


//functions which query the number and nature of supplied arguments

/*
 *    PsychGetNumInputArgs()
 *
 *    -The count excludes the command argument and includes ALL arguments supplied, including
 *    default arguments.
 *
 *    -For the time being, the only way to check if all required arguments are supplied in the
 *    general case of mixed required and optional arguments is to check each individually. Probably
 *    the best way to to fix this is to employ a description of which are required and which optional
 *    and compare that against what was passed to the subfunction.
 */
int PsychGetNumInputArgs(void)
{
    if (PsychAreSubfunctionsEnabled() && !baseFunctionInvoked[recLevel]) //this should probably be just baseFunctionInvoked[recLevel] wo PsychSubfunctionEnabled.
        return(nrhsGLUE[recLevel]-1);
    else
        return(nrhsGLUE[recLevel]);
}


// Careful! Here's a catch: I don't know of a straightforward way to actually
// find out how many return arguments (output args) the calling Python runtime
// expects from us. Apparently that's already a major problem in the Python
// language itself for native Python scripting code.
int PsychGetNumOutputArgs(void)
{
    return((nlhsGLUE[recLevel] <= 0) ? ((nlhsGLUE[recLevel] == 0) ? 1 : MAX_OUTPUT_ARGS) : nlhsGLUE[recLevel]);
}


int PsychGetNumNamedOutputArgs(void)
{
    return(nlhsGLUE[recLevel]);
}


PsychError PsychCapNumOutputArgs(int maxNamedOutputs)
{
    // Number of output args already known?
    if ((PsychGetNumNamedOutputArgs() < 0) && (maxNamedOutputs < MAX_OUTPUT_ARGS)) {
        // No. We use the maxNamedOutputs passed in here as the clue to what the
        // correct value is, given that at least the Python 2 api may be incapable
        // of easily telling us this magic value:
        // TODO FIXME Don't use this as hint, always detect true number at function exit, always succeed: nlhsGLUE[recLevel] = maxNamedOutputs;
        return(PsychError_none);
    }

    // Maximum number of expected outputs known, so enforce them:
    if (PsychGetNumNamedOutputArgs() > maxNamedOutputs)
        return(PsychError_extraOutputArg);
    else
        return(PsychError_none);
}


/*
 *    The argument is not present if a default m*n=0 matrix was supplied, '' or []
 */
psych_bool PsychIsArgPresent(PsychArgDirectionType direction, int position)
{
    int numArgs;

    if (direction==PsychArgOut) {
        return((psych_bool)(PsychGetNumOutputArgs()>=position));
    } else {
        if ((numArgs=PsychGetNumInputArgs())>=position)
            return(!(PsychIsDefaultMat(PsychGetInArgPyPtr(position)))); //check if its default
        else
            return(FALSE);
    }
}


PsychArgFormatType PsychGetArgType(int position) //this is for inputs because outputs are unspecified by the calling environment.
{
    if (!(PsychIsArgReallyPresent(PsychArgIn, position)))
        return(PsychArgType_none);

    return(PsychGetTypeFromPyPtr(PsychGetInArgPyPtr(position)));
}


size_t PsychGetArgM(int position)
{
    if (!(PsychIsArgPresent(PsychArgIn, position)))
        PsychErrorExitMsg(PsychError_invalidArgRef,NULL);
    return( mxGetM(PsychGetInArgPyPtr(position)));
}


size_t PsychGetArgN(int position)
{
    if (!(PsychIsArgPresent(PsychArgIn, position)))
        PsychErrorExitMsg(PsychError_invalidArgRef,NULL);
    return( mxGetNOnly(PsychGetInArgPyPtr(position)));
}


size_t PsychGetArgP(int position)
{
    if (!(PsychIsArgPresent(PsychArgIn, position)))
        PsychErrorExitMsg(PsychError_invalidArgRef,NULL);
    return( mxGetP(PsychGetInArgPyPtr(position)));
}


/*functions which output arguments.
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * -Naming scheme:
 *    -Outputing return arguments:
 *        - "PsychAllocOut*Arg" : allocate and set a pointer to volatile memory to be filled with returned information by the caller.
 *        - "PsychCopyOut*Arg : accept a pointer to ouput values and fill in the return matrix memory with the values.
 *    -Reading input arguments:
 *        - "PsychAllocIn*Arg" : set a pointer to volatile memory allocated by "PsychAllocIn*Arg" and holding the input value.
 *        - "PsychCopyIn*Arg" : accept a pointer to memory and fill in that memory with the input argument values.
 *
 * -These all supply their own dynamic memory now, even functions which return arguments, and, in the case of
 * Put functions,  even when those arguments are not present !  If you don't want the function to go allocating
 * memory for an unsupplied return argument, detect the presense of that argument from within your script and
 * conditionally invoke PsychPut*Arg.  This is a feature which allows you to ignore the presense of a return
 * argument in the case where memory which holds a return argument serves other purposes.
 *
 * -All dynamic memory provided by these functions is volatile, that is, it is lost when the mex module exits and
 * returns control to the Matlab environemnt.  To make it non volatile, call Psych??? on it.
 *
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */

psych_bool PsychCopyOutDoubleArg(int position, PsychArgRequirementType isRequired, double value)
{
    PyObject        **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_double,  isRequired, 1,1,1,1,0,0);
    matchError = PsychMatchDescriptors();
    putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = PyFloat_FromDouble(value);
        // *mxpp = mxCreateDoubleMatrix(1, 1);
        // mxGetPr(*mxpp)[0] = value;
    }
    return(putOut);
}

psych_bool PsychAllocOutDoubleArg(int position, PsychArgRequirementType isRequired, double **value)
{
    PyObject        **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_double, isRequired, 1,1,1,1,0,0);
    matchError = PsychMatchDescriptors();
    putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        printf("I: mxpp %p\n", mxpp);
        *mxpp = mxCreateDoubleMatrix3D(1,1,0);
        printf("II: mxpp %p\n", *mxpp);
        *value = mxGetPr(*mxpp);
        printf("III: value %p\n", *value);
    } else {
        mxpp = PsychGetOutArgPyPtr(position);
        *value= (double *) mxMalloc(sizeof(double));
    }
    return(putOut);
}


/*
 * PsychAllocOutDoubleMatArg()
 *
 * A)return argument mandatory:
 *    1)return argument not present:     exit with an error.
 *    2)return argument present:         allocate an output matrix and set return arg pointer. Set *array to the array within the new matrix. Return TRUE.
 * B)return argument optional:
 *    1)return argument not present:     return FALSE to indicate absent return argument.  Create an array.   Set *array to the new array.
 *    2)return argument present:         allocate an output matrix and set return arg. pointer. Set *array to the array within the new matrix.  Return TRUE.
 */
psych_bool PsychAllocOutDoubleMatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, double **array)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_double, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateDoubleMatrix3D(m,n,p);
        *array = mxGetPr(*mxpp);
    }else
        *array= (double *) mxMalloc(sizeof(double) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    return(putOut);
}


/*
 * PsychAllocOutFloatMatArg()
 *
 * This function allocates out a matrix of single precision floating point type,
 * that is C data type 32-bit float or Matlab/Octave data type single().
 *
 * A)return argument mandatory:
 *    1)return argument not present:         exit with an error.
 *    2)return argument present:         allocate an output matrix and set return arg pointer. Set *array to the array within the new matrix. Return TRUE.
 * B)return argument optional:
 *    1)return argument not present:      return FALSE to indicate absent return argument.  Create an array.   Set *array to the new array.
 *    2)return argument present:         allocate an output matrix and set return arg. pointer. Set *array to the array within the new matrix.  Return TRUE.
 */
psych_bool PsychAllocOutFloatMatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, float **array)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_single, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateFloatMatrix3D((size_t) m, (size_t) n, (size_t) p);
        *array = (float*) mxGetData(*mxpp);
    }else
        *array = (float*) mxMalloc(sizeof(float) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    return(putOut);
}


/*
 *    PsychCopyOutBooleanArg()
 */
psych_bool PsychCopyOutBooleanArg(int position, PsychArgRequirementType isRequired, PsychNativeBooleanType value)
{
    PyObject        **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_boolean, isRequired, 1,1,1,1,0,0);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = PyBool_FromLong((long) value);
        // (*mxpp) = mxCreateLogicalMatrix(1,1);
        // mxGetLogicals((*mxpp))[0] = value;
    }
    return(putOut);
}


/*    CURRENTLY UNUSED
 *    PsychAllocOutBooleanArg()
psych_bool PsychAllocOutBooleanArg(int position, PsychArgRequirementType isRequired, PsychNativeBooleanType **value)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_boolean, isRequired, 1,1,1,1,0,0);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        (*mxpp) = mxCreateLogicalMatrix(1,1);
        *value = mxGetLogicals((*mxpp));
    } else {
        mxpp = PsychGetOutArgPyPtr(position);
        *value= (PsychNativeBooleanType *)mxMalloc(sizeof(PsychNativeBooleanType));
    }
    return(putOut);
}
*/


/*
 *    PsychAllocOutBooleanMatArg()
 *
 *    A)return argument mandatory:
 *    1)return argument not present:         exit with an error.
 *    2)return argument present:         allocate an output matrix and set return arg pointer. Set *array to the array within the new matrix. Return TRUE.
 *    B)return argument optional:
 *    1)return argument not present:      return FALSE to indicate absent return argument.  Create an array.   Set *array to the new array.
 *    2)return argument present:         allocate an output matrix and set return arg. pointer. Set *array to the array within the new matrix.  Return TRUE.
 */
psych_bool PsychAllocOutBooleanMatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, PsychNativeBooleanType **array)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_boolean, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateNativeBooleanMatrix3D((size_t) m, (size_t) n, (size_t) p);
        *array = (PsychNativeBooleanType *)mxGetLogicals(*mxpp);
    } else {
        *array= (PsychNativeBooleanType *) mxMalloc(sizeof(PsychNativeBooleanType) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    }
    return(putOut);
}


/*
 *    PsychAllocOutUnsignedByteMatArg()
 *
 *    Like PsychAllocOutDoubleMatArg() execept for unsigned bytes instead of doubles.
 */
psych_bool PsychAllocOutUnsignedByteMatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, psych_uint8 **array)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_uint8, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateByteMatrix3D((size_t) m, (size_t) n, (size_t) p);
        *array = (psych_uint8 *)mxGetData(*mxpp);
    } else {
        *array= (psych_uint8 *) mxMalloc(sizeof(psych_uint8) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    }
    return(putOut);
}


/*
 *    PsychAllocOutUnsignedInt16MatArg()
 *
 *    Like PsychAllocOutDoubleMatArg() execept for unsigned shorts instead of doubles.
 */
psych_bool PsychAllocOutUnsignedInt16MatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, psych_uint16 **array)
{
    PyObject         **mxpp;
    PsychError      matchError;
    psych_bool      putOut;
    ptbSize          dimArray[3];
    int             numDims;

    // Compute output array dimensions:
    if (m<=0 || n<=0) {
        dimArray[0] = 0; dimArray[1] = 0; dimArray[2] = 0;  //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims = (p == 0 || p == 1) ? 2 : 3;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_uint16, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_uint16);
        *array = (psych_uint16 *)mxGetData(*mxpp);
    } else {
        *array= (psych_uint16 *) mxMalloc(sizeof(psych_uint16) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    }
    return(putOut);
}

/* TODO FIXME */
psych_bool PsychCopyOutDoubleMatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, double *fromArray)
{
    PyObject     **mxpp;
    double      *toArray;
    PsychError  matchError;
    psych_bool  putOut;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_double, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateDoubleMatrix3D(m,n,p);
        toArray = mxGetPr(*mxpp);
        //copy the input array to the output array now
        memcpy(toArray, fromArray, sizeof(double) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    }
    return(putOut);
}

/* TODO FIXME */
psych_bool PsychCopyOutUnsignedInt16MatArg(int position, PsychArgRequirementType isRequired, psych_int64 m, psych_int64 n, psych_int64 p, psych_uint16 *fromArray)
{
    PyObject         **mxpp;
    psych_uint16    *toArray;
    PsychError      matchError;
    psych_bool      putOut;
    ptbSize          dimArray[3];
    int             numDims;

    // Compute output array dimensions:
    if (m<=0 || n<=0) {
        dimArray[0] = 0; dimArray[1] = 0; dimArray[2] = 0;    //this prevents a 0x1 or 1x0 empty matrix, we want 0x0 for empty matrices.
    } else {
        PsychCheckSizeLimits(m,n,p);
        dimArray[0] = (ptbSize) m; dimArray[1] = (ptbSize) n; dimArray[2] = (ptbSize) p;
    }
    numDims = (p == 0 || p == 1) ? 2 : 3;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_uint16, isRequired, m,m,n,n,p,p);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateNumericArray(numDims, (ptbSize*) dimArray, PsychArgType_uint16);
        toArray = (psych_uint16*) mxGetData(*mxpp);

        //copy the input array to the output array now
        memcpy(toArray, fromArray, sizeof(psych_uint16) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    }
    return(putOut);
}


/*
 *    PsychCopyOutCharArg()
 *
 *    Accept a null terminated string and return it in the specified position.
 *
 */
psych_bool PsychCopyOutCharArg(int position, PsychArgRequirementType isRequired, const char *str)
{
    PyObject    **mxpp;
    PsychError  matchError;
    psych_bool  putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_char, isRequired, 0, strlen(str),0,strlen(str),0,0);
    matchError=PsychMatchDescriptors();
    putOut=PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = mxCreateString(str);
    }
    return(putOut);
}



/*functions with input arguments.
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 */

/*
 * A)input argument mandatory:
 *
 *    1)input argument not present:         exit with error.
 *    2)input argument present:             set *array to the input matrix, *m, *n, and *p to its dimensions, return TRUE.
 * B)input argument optional:
 *
 *    1)input argument not present:         return FALSE
 *    2)input argument present:             set *array to the input matrix, *m, *n, and *p to its dimensions, return TRUE.
 *
 */
// TO DO: Needs to be updated for kPsychArgAnything
psych_bool PsychAllocInDoubleMatArg(int position, PsychArgRequirementType isRequired, int *m, int *n, int *p, double **array)
{
    const PyObject   *ppyPtr;
    PsychError      matchError;
    psych_bool      acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,-1,1,-1,0,-1);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *m = (int) mxGetM(ppyPtr);
        *n = (int) mxGetNOnly(ppyPtr);
        *p = (int) mxGetP(ppyPtr);
        *array=mxGetPr(ppyPtr);
    }
    return(acceptArg);
}


/* Alloc-in double matrix, but allow for 64-bit dimension specs. */
psych_bool PsychAllocInDoubleMatArg64(int position, PsychArgRequirementType isRequired, psych_int64 *m, psych_int64 *n, psych_int64 *p, double **array)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,-1,1,-1,0,-1);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *m = (psych_int64) mxGetM(ppyPtr);
        *n = (psych_int64) mxGetNOnly(ppyPtr);
        *p = (psych_int64) mxGetP(ppyPtr);
        *array=mxGetPr(ppyPtr);
    }
    return(acceptArg);
}


/* Like PsychAllocInFloatMatArg64, but with 32-Bit int type size return-arguments. */
psych_bool PsychAllocInFloatMatArg(int position, PsychArgRequirementType isRequired, int *m, int *n, int *p, float **array)
{
    psych_int64 mb, nb, pb;
    psych_bool rc = PsychAllocInFloatMatArg64(position, isRequired, &mb, &nb, &pb, array);
    *m = (int) mb;
    *n = (int) nb;
    *p = (int) pb;
    return(rc);
}


/*
 *
 * Alloc-In a single precision floating point matrix, i.e. a matrix of
 * C data type 32 bit float, aka Matlab/Octave data type single().
 * This function allows to alloc in matrices with more than 2^32 elements
 * per matrix dimension on 64 bit systems. Therefore the returned size
 * descriptors must be psych_int64 variables, not int variables or bad things
 * will happen.
 *
 * If the function receives a double() precision input matrix instead of the
 * expected single() precision matrix, it will automatically create a temporary
 * copy, with all values copied/converted from double to single data type, aka
 * double -> float cast. This is transparent to the caller, so it can always
 * operate on a returned float matrix -- at a performance penalty for the extra
 * copy of course.
 *
 * A)input argument mandatory:
 *
 *    1)input argument not present:         exit with error.
 *    2)input argument present:             set *array to the input matrix, *m, *n, and *p to its dimensions, return TRUE.
 * B)input argument optional:
 *
 *    1)input argument not present:         return FALSE
 *    2)input argument present:             set *array to the input matrix, *m, *n, and *p to its dimensions, return TRUE.
 *
 */
psych_bool PsychAllocInFloatMatArg64(int position, PsychArgRequirementType isRequired, psych_int64 *m, psych_int64 *n, psych_int64 *p, float **array)
{
    const PyObject   *ppyPtr;
    PsychError      matchError;
    psych_bool      acceptArg;
    double*         arrayD;
    float*          arrayF;
    psych_int64     i;

    PsychSetReceivedArgDescriptor(position, TRUE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_single, isRequired, 1,-1,1,-1,0,-1);
    matchError=PsychMatchDescriptors();

    // Argument provided, but not of required float type?
    if (matchError == PsychError_invalidArg_type) {
        // If the input type is double precision floating point, then we convert
        // it here into single precision floating point via a temporary buffer.
        // This is used for functions which absolutely need float input, e.g.,
        // OpenGL-ES rendering code, but should accept double input from usercode
        // so usercode doesn not need to be specifically ported for OpenGL-ES platforms.
        // Performance may suffer somwehat though...
        PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,-1,1,-1,0,-1);
        matchError=PsychMatchDescriptors();
        acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
        if (acceptArg) {
            ppyPtr = PsychGetInArgPyPtr(position);
            *m = (psych_int64) mxGetM(ppyPtr);
            *n = (psych_int64) mxGetNOnly(ppyPtr);
            *p = (psych_int64) mxGetP(ppyPtr);

            // Get a double pointer to the double input data matrix:
            arrayD = (double*) mxGetData(ppyPtr);

            // Allocate temporary float input matrix. It will get deallocated
            // automatically at return to runtime:
            *array = (float*) PsychMallocTemp(sizeof(float) * (*m) * (*n) * (*p));
            arrayF = *array;

            // Copy/Convert loop:
            for (i = (*m) * (*n) * (*p); i > 0; i--) *(arrayF++) = (float) *(arrayD++);
        }

        return(acceptArg);
    }

    // Regular path: Matching float (aka single()) matrix/vector provided:
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *m = (psych_int64) mxGetM(ppyPtr);
        *n = (psych_int64) mxGetNOnly(ppyPtr);
        *p = (psych_int64) mxGetP(ppyPtr);
        *array = (float*) mxGetData(ppyPtr);
    }
    return(acceptArg);
}


/*
 *    PsychAllocInIntegerListArg()
 *
 *    In a scriptiong language such as MATLAB where numbers are almost always stored as doubles, this function is useful to check
 *    that the value input is an integer value stored within a double type.
 *
 *    Otherwise it just here to imitate the version written for other scripting languages.
 */
psych_bool PsychAllocInIntegerListArg(int position, PsychArgRequirementType isRequired, int *numElements, int **array)
{
    int         m, n, p, i;
    double      *doubleMatrix;
    psych_bool  isThere;

    isThere=PsychAllocInDoubleMatArg(position, isRequired, &m, &n, &p, &doubleMatrix);
    if (!isThere)
        return(FALSE);
    p= (p==0) ? 1 : p;

    if ((psych_uint64) m * (psych_uint64) n * (psych_uint64) p >= INT_MAX) {
        printf("PTB-ERROR: %i th input argument has more than 2^31 - 1 elements! This is not supported.\n", position);
        return(FALSE);
    }

    *numElements = m * n * p;
    *array = (int*) mxMalloc((size_t) *numElements * sizeof(int));
    for (i=0; i < *numElements; i++) {
        if (!PsychIsIntegerInDouble(doubleMatrix+i))
            PsychErrorExit(PsychError_invalidIntegerArg);
        (*array)[i] = (int) doubleMatrix[i];
    }
    return(TRUE);
}


/*
 *    PsychAllocInByteMatArg()
 *
 *    Like PsychAllocInDoubleMatArg() except it returns an array of unsigned bytes.
 */
psych_bool PsychAllocInUnsignedByteMatArg(int position, PsychArgRequirementType isRequired, int *m, int *n, int *p, unsigned char **array)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_uint8, isRequired, 1,-1,1,-1,0,-1);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *m = (int) mxGetM(ppyPtr);
        *n = (int) mxGetNOnly(ppyPtr);
        *p = (int) mxGetP(ppyPtr);
        *array=(unsigned char *)mxGetData(ppyPtr);
    }
    return(acceptArg);
}


/*
 *    PsychCopyInDoubleArg()
 *
 *    For 1x1 double.
 *
 *    Return in *value a double passed in the specified position, or signal an error if there is no
 *    double there and the argument is required, or don't change "value" if the argument is optional
 *    and none is supplied.
 *
 *    Note that if the argument is optional and ommitted PsychGetDoubleArg won't overwrite *value, allowing
 *    for specification of default values within project files without checking for their
 *    presense and conditinally filing in values.
 */
psych_bool PsychCopyInDoubleArg(int position, PsychArgRequirementType isRequired, double *value)
{
    const PyObject  *ppyPtr;
    PsychError      matchError;
    psych_bool      acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,1,1,1,1,1);
    matchError = PsychMatchDescriptors();

    acceptArg = PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *value = PyFloat_AsDouble(ppyPtr);

        if (PyErr_Occurred())
            PsychErrorExit(PsychError_invalidArg_type);
    }

    return(acceptArg);
}


/*
 *    Like PsychCopyInDoubleArg() with the additional restriction that the passed value not have a fractoinal componenet
 *    and that the it fit within thebounds of a C integer
 *
 *    We could also accept matlab native integer types by specifying a conjunction of those as the third argument
 *    in the PsychSetSpecifiedArgDescriptor() call, but why bother ?
 */
psych_bool PsychCopyInIntegerArg(int position,  PsychArgRequirementType isRequired, int *value)
{
    const PyObject    *ppyPtr;
    double            tempDouble;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double | PsychArgType_int32, isRequired, 1,1,1,1,1,1);
    matchError = PsychMatchDescriptors();
    acceptArg = PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);

        if (PyLong_Check(ppyPtr)) {
            *value = (int) PyLong_AsLong(ppyPtr);
            if (PyErr_Occurred())
                PsychErrorExit(PsychError_invalidIntegerArg);
        }
        else {
            tempDouble = PyFloat_AsDouble(ppyPtr);
            if (PyErr_Occurred() || !PsychIsIntegerInDouble(&tempDouble))
                PsychErrorExit(PsychError_invalidIntegerArg);
            *value = (int) tempDouble;
        }
    }

    return(acceptArg);
}


psych_bool PsychCopyInIntegerArg64(int position,  PsychArgRequirementType isRequired, psych_int64 *value)
{
    const PyObject    *ppyPtr;
    double            tempDouble;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,1,1,1,1,1);
    matchError = PsychMatchDescriptors();
    acceptArg = PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);

        if (PyLong_Check(ppyPtr)) {
            *value = (psych_int64) PyLong_AsLongLong(ppyPtr);
            if (PyErr_Occurred())
                PsychErrorExit(PsychError_invalidIntegerArg);
        }
        else {
            tempDouble = PyFloat_AsDouble(ppyPtr);
            if (PyErr_Occurred() || !PsychIsInteger64InDouble(&tempDouble))
                PsychErrorExit(PsychError_invalidIntegerArg);
            *value = (psych_int64) tempDouble;
        }
    }

    return(acceptArg);
}


/*
 *    PsychAllocInDoubleArg()
 *      TODO FIXME
 */
psych_bool PsychAllocInDoubleArg(int position, PsychArgRequirementType isRequired, double **value)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_double, isRequired, 1,1,1,1,1,1);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *value=mxGetPr(ppyPtr);
    }
    return(acceptArg);
}


/*
 *    PsychAllocInCharArg()
 *
 *    Reads in a string and sets *str to point to the string.
 *
 *    This function violates the rule for AllocIn fuctions that if the argument is optional and absent we allocate
 *    space.  That turns out to be an unuseful feature anyway, so we should probably get ride of it.
 *
 *    The second argument has been modified to passively accept, without error, an argument in the specified position of non-character type.
 *
 *        0    kPsychArgOptional  Permit either an argument of the specified type or no argument.  An argument of any a different type is an error.
 *        1    kPsychArgRequired  Permit only an argument of the specifed type.  If no argument is present, exit with error.
 *        2    kPsychArgAnything  Permit any argument type without error, but only read the specified type.
 *
 */
psych_bool PsychAllocInCharArg(int position, PsychArgRequirementType isRequired, char **str)
{
    const PyObject  *ppyPtr;
    int             status;
    psych_uint64    strLen;
    PsychError      matchError;
    psych_bool      acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_char, isRequired, 0, kPsychUnboundedArraySize ,0, kPsychUnboundedArraySize, 0 , 1);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        strLen = (psych_uint64) PyString_Size(ppyPtr) + 1;
        if (strLen >= INT_MAX)
            PsychErrorExitMsg(PsychError_user, "Tried to pass in a string with more than 2^31 - 1 characters. Unsupported!");

        *str = (char *) PsychCallocTemp((size_t) strLen, sizeof(char));
        status = mxGetString(ppyPtr, *str, (ptbSize) strLen);
        if (status != 0)
            PsychErrorExitMsg(PsychError_internal, "mxGetString failed to get the string");
    }
    return(acceptArg);
}


/*
 *    Get a psych_bool flag from the specified argument position.  The matlab type can be be psych_bool, uint8, or
 *    char.  If the numerical value is equal to zero or if its empty then the flag is FALSE, otherwise the
 *    flag is TRUE.
 *
 *    PsychGetFlagArg returns TRUE if the argument was present and false otherwise:
 *
 *    A- Argument required
 *        1- Argument is present: load *argVal and return TRUE
 *        2- Argument is absent: exit with an error
 *    B- Argument is optional
 *        1- Argument is present: load *argVal and return TRUE
 *        2- Argument is absent: leave *argVal alone and return FALSE
 *
 *    Note: if we modify PsychGetDoubleArg to accept all types and coerce them, then we could simplify by
 *    calling that instead of doing all of this stuff...
 * TODO FIXME
 */
psych_bool PsychAllocInFlagArg(int position,  PsychArgRequirementType isRequired, psych_bool **argVal)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, (PsychArgFormatType)(PsychArgType_double|PsychArgType_char|PsychArgType_uint8|PsychArgType_boolean),
                                   isRequired, 1,1,1,1,kPsychUnusedArrayDimension,kPsychUnusedArrayDimension);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        //unlike other PsychAllocIn* functions, here we allocate new memory and copy the input to it rather than simply returning a pointer into the received array.
        //That's because we want the booleans returned to the caller by PsychAllocInFlagArg() to alwyas be 8-bit booleans, yet we accept as flags either 64-bit double, char,
        //or logical type.  Restricting to logical type would be a nuisance in the MATLAB environment and does not solve the problem because on some platforms MATLAB
        //uses for logicals 64-bit doubles and on others 8-bit booleans (check your MATLAB mex/mx header files).
        *argVal = (psych_bool *)mxMalloc(sizeof(psych_bool));
        ppyPtr = PsychGetInArgPyPtr(position);
        if (mxIsLogical(ppyPtr)) {
            if (mxGetLogicals(ppyPtr)[0])
                **argVal=(psych_bool)1;
            else
                **argVal=(psych_bool)0;
        } else {
            if (mxGetScalar(ppyPtr))
                **argVal=(psych_bool)1;
            else
                **argVal=(psych_bool)0;
        }
    }
    return(acceptArg);    //the argument was not present (and optional).
}

/* TODO FIXME */
psych_bool PsychAllocInFlagArgVector(int position,  PsychArgRequirementType isRequired, int *numElements, psych_bool **argVal)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;
    int               i;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    // MK: Disabled. Doesn't work without conversion of mxGetData into many subcases...
    // PsychSetSpecifiedArgDescriptor(position, PsychArgIn, (PsychArgFormatType)(PsychArgType_double | PsychArgType_char | PsychArgType_uint8 | PsychArgType_boolean),
    //               isRequired, 1, kPsychUnboundedArraySize, 1, kPsychUnboundedArraySize, kPsychUnusedArrayDimension, kPsychUnusedArrayDimension);

    // Ok. Let's see if anybody ever complains about this...
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, (PsychArgFormatType)(PsychArgType_double | PsychArgType_boolean),
                                   isRequired, 1, kPsychUnboundedArraySize, 1, kPsychUnboundedArraySize, kPsychUnusedArrayDimension, kPsychUnusedArrayDimension);
    matchError=PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        if ((psych_uint64) mxGetM(ppyPtr) * (psych_uint64) mxGetN(ppyPtr) >= INT_MAX) {
            printf("PTB-ERROR: %i th input argument has more than 2^31 - 1 elements! This is not supported.\n", position);
            *numElements = 0;
            return(FALSE);
        }

        *numElements = (int) (mxGetM(ppyPtr) * mxGetN(ppyPtr));

        //unlike other PsychAllocIn* functions, here we allocate new memory and copy the input to it rather than simply returning a pointer into the received array.
        //That's because we want the booleans returned to the caller by PsychAllocInFlagArgVector() to alwyas be 8-bit booleans, yet we accept as flags either 64-bit double, char,
        //or logical type.  Restricting to logical type would be a nuisance in the MATLAB environment and does not solve the problem because on some platforms MATLAB
        //uses for logicals 64-bit doubles and on others 8-bit booleans (check your MATLAB mex/mx header files).
        *argVal = (psych_bool *)mxMalloc(sizeof(psych_bool) * ((size_t) *numElements));
        for (i=0; i< *numElements;i++) {
            if (mxIsLogical(ppyPtr)) {
                if (mxGetLogicals(ppyPtr)[i])
                    (*argVal)[i]=(psych_bool)1;
                else
                    (*argVal)[i]=(psych_bool)0;
            } else {
                if (mxGetPr(ppyPtr)[i])
                    (*argVal)[i]=(psych_bool)1;
                else
                    (*argVal)[i]=(psych_bool)0;
            }
        }
    }
    return(acceptArg);    //the argument was not present (and optional).
}


/*
 *    PsychCopyInFlagArg()
 */
psych_bool PsychCopyInFlagArg(int position, PsychArgRequirementType isRequired, psych_bool *argVal)
{
    const PyObject     *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, (PsychArgFormatType)(PsychArgType_double|PsychArgType_char|PsychArgType_uint8|PsychArgType_boolean),
                                   isRequired, 1,1,1,1,kPsychUnusedArrayDimension,kPsychUnusedArrayDimension);
    matchError = PsychMatchDescriptors();
    acceptArg = PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        if (mxIsLogical(ppyPtr)) {
            if (ppyPtr == Py_True)
                *argVal = (psych_bool) 1;
            else
                *argVal = (psych_bool) 0;
        } else {
            if (mxGetScalar(ppyPtr))
                *argVal = (psych_bool) 1;
            else
                *argVal = (psych_bool) 0;
        }
    }

    return(acceptArg);
}


psych_bool PsychCopyOutFlagArg(int position, PsychArgRequirementType isRequired, psych_bool argVal)
{
    return(PsychCopyOutDoubleArg(position, isRequired, (double) argVal));
}


// functions which allocate native types without assigning them to return arguments.
// this is useful for embedding native structures within each other.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 *    PsychAllocateNativeDoubleMat()
 *
 *    Create an opaque native matrix.   Return both
 *        - Its handle,  which is specified when nesting the native matrix nesting withing other native types.
 *        - A handle to the C array of doubles enclosed by the native type.
 *
 *    If (*cArray != NULL) we copy m*n*p elements from cArray into the native matrix, otherwise not.
 *    In any case, *cArray will point to the C array of doubles enclosed by the native type in the end.
 * TODO FIXME
 */
void PsychAllocateNativeDoubleMat(psych_int64 m, psych_int64 n, psych_int64 p, double **cArray, PsychGenericScriptType **nativeElement)
{
    double *cArrayTemp;

    PsychCheckSizeLimits(m, n, p);
    *nativeElement = mxCreateDoubleMatrix3D(m,n,p);
    cArrayTemp = mxGetPr(*nativeElement);
    if (*cArray != NULL) memcpy(cArrayTemp, *cArray, sizeof(double) * (size_t) m * (size_t) n * (size_t) maxInt(1,p));
    *cArray=cArrayTemp;
}


double PsychGetNanValue(void)
{
    return(mxGetNaN());
}

/* UNUSED: PsychAllocInCharFromNativeArg()
 *
 * Given a pointer to a native PsychGenericScriptType datatype which represents character
 * strings in the runtime's native encoding, try to extract a standard char-string of it and
 * return it in the referenced char *str. Return TRUE on success, FALSE on failure, e.g., because
 * the nativeCharElement didn't contain a parseable string.
 *
psych_bool PsychAllocInCharFromNativeArg(PsychGenericScriptType *nativeCharElement, char **str)
{
    PyObject         *ppyPtr;
    int             status;
    psych_uint64    strLen;

    *str = NULL;
    ppyPtr  = (PyObject*) nativeCharElement;
    strLen = ((psych_uint64) mxGetM(ppyPtr) * (psych_uint64) mxGetNOnly(ppyPtr) * (psych_uint64) sizeof(mxChar)) + 1;
    if (strLen >= INT_MAX) PsychErrorExitMsg(PsychError_user, "Tried to pass in a string with more than 2^31 - 1 characters. Unsupported!");

    *str   = (char *) PsychCallocTemp((size_t) strLen, sizeof(char));
    status = mxGetString(ppyPtr, *str, (ptbSize) strLen);
    if (status!=0) return(FALSE);
    return(TRUE);
}
*/

/* PsychRuntimeGetPsychtoolboxRoot()
 *
 * Try to retrieve filesystem path to Psychtoolbox root folder (the result from PsychtoolboxRoot() in Matlab/Octave)
 * or users configuration folder (the result from PsychtoolboxConfigDir() in Matlab/Octave) from runtime. The result,
 * if any, will be cached for later fast lookup.
 *
 * getConfigDir = TRUE => Return PsychtoolboxConfigDir().
 * getConfigDir = FALSE => Return PsychtoolboxRoot().
 *
 * This function may fail to retrieve the path, in which case it returns an empty null-terminated string, i.e., strlen() == 0.
 * On successfull recovery of the path, returns a const char* to a readonly string which encodes the path.
 *
 */
const char* PsychRuntimeGetPsychtoolboxRoot(psych_bool getConfigDir)
{
    static psych_bool   firstTime = TRUE;
    static char         psychtoolboxRootPath[FILENAME_MAX+1];
    static char         psychtoolboxConfigPath[FILENAME_MAX+1];
    char*               myPathvarChar = NULL;
    PyObject             *plhs[1]; // Capture the runtime result of PsychtoolboxRoot/ConfigDir
/*
    if (firstTime) {
        // Reset firstTime flag:
        firstTime = FALSE;

        // Init to null-terminated empty strings, so it is well-defined in case of error:
        psychtoolboxRootPath[0] = 0;
        psychtoolboxConfigPath[0] = 0;

        // We could have used mexCallMATLABWithTrap below, but Octave doesn't support it.
        // Unfortunately, MATLAB plans to deprecate mexSetTrapFlag in lieu of *WithTrap, so
        // we'll need to patch Octave or make some sort of wrapper function in here.

        // Call into runtime to get the path to the root folder: This will return 0 on success.
        // A non-zero return value probably means that the script wasn't in the path. When that
        // happens, there will be an error in the command window, but control stays with the mex
        // file (thanks to mexSetTrapFlag(1) above) and it'll continue to run.
        if (0 == mexCallMATLAB(1, plhs, 0, NULL, "PsychtoolboxRoot")) {
            myPathvarChar = mxArrayToString(plhs[0]);
            if (myPathvarChar) {
                strncpy(psychtoolboxRootPath, myPathvarChar, FILENAME_MAX);
                mxFree(myPathvarChar);
            }
        }
        mxDestroyArray(plhs[0]);

        // At this point we did our best and psychtoolboxRootPath is valid: Either a path string,
        // or an empty string signalling failure to get the path.

        // Same game again for PsychtoolboxConfigDir:
        if (0 == mexCallMATLAB(1, plhs, 0, NULL, "PsychtoolboxConfigDir")) {
            myPathvarChar = mxArrayToString(plhs[0]);
            if (myPathvarChar) {
                strncpy(psychtoolboxConfigPath, myPathvarChar, FILENAME_MAX);
                mxFree(myPathvarChar);
            }
        }
        mxDestroyArray(plhs[0]);
    }
*/
    // Return whatever we've got:
    return((getConfigDir) ? &psychtoolboxConfigPath[0] : &psychtoolboxRootPath[0]);
}


/* PsychCopyInPointerArg() - Copy in a void* memory pointer. */
psych_bool PsychCopyInPointerArg(int position, PsychArgRequirementType isRequired, void **ptr)
{
    const PyObject    *ppyPtr;
    PsychError        matchError;
    psych_bool        acceptArg;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgIn);
    PsychSetSpecifiedArgDescriptor(position, PsychArgIn, PsychArgType_unclassified, isRequired, 1,1,1,1,1,1);
    matchError = PsychMatchDescriptors();
    acceptArg=PsychAcceptInputArgumentDecider(isRequired, matchError);
    if (acceptArg) {
        ppyPtr = PsychGetInArgPyPtr(position);
        *ptr = (void*) PyLong_AsVoidPtr(ppyPtr);
    }

    return(acceptArg);
}


/* PsychCopyOutPointerArg() - Copy out a void* memory pointer. */
psych_bool PsychCopyOutPointerArg(int position, PsychArgRequirementType isRequired, void* ptr)
{
    PyObject        **mxpp;
    PsychError      matchError;
    psych_bool      putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_unclassified, isRequired, 1,1,1,1,0,0);
    matchError = PsychMatchDescriptors();
    putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxpp = PsychGetOutArgPyPtr(position);
        *mxpp = PyLong_FromVoidPtr(ptr);
    }

    return(putOut);
}


/* PsychRuntimePutVariable() TODO FIXME
 *
 * Copy a given native variable of type PsychGenericScriptType, e.g., as created by PsychAllocateNativeDoubleMat()
 * in case of a double matrix, as a new variable into a specified workspace.
 *
 * 'workspace'    Namestring of workspace: "base" copy to base workspace. "caller" copy into calling functions workspace,
 *                'global' create new global variable with given name.
 *
 *                CAUTION:    Matlab and Octave show different behaviour when using the "caller" workspace! It is strongly
 *                            recommended to avoid the "caller" workspace to avoid ugly compatibility bugs!!
 *
 * 'variable'    Name of the new variable.
 *
 * 'pcontent'    The actual content that should be copied into the variable.
 *
 *
 * Example: You want to create a double matrix with (m,n,p) rows/cols/layers as a variable 'myvar' in the base
 *          workspace and initialize it with content from the double array mycontent:
 *
 *          PsychGenericScriptType* newvar = NULL;
 *            double* newvarcontent = mycontent; // mycontent is double* onto existing data.
 *          PsychAllocateNativeDoubleMat(m, n, p, &newvarcontent, &newvar);
 *            At this point, newvar contains the content of 'mycontent' and 'newvarcontent' points to
 *            the copy. You could alter mycontent now without affecting the content of newvarcontent or newvar.
 *
 *            Create the corresponding variable in the base workspace:
 *            PsychRuntimePutVariable("base", "myvar", newvar);
 *
 *          The calling M-File etc. can access the content newvarcontent under the variable name 'myvar'.
 *
 *            As usual, the double matrix newvarcontent will be auto-destroyed when returning to the runtime,
 *            but the variable 'myvar' will remain valid until it goes out of scope.
 *
 * Returns zero on success, non-zero on failure.
 */
int PsychRuntimePutVariable(const char* workspace, const char* variable, PsychGenericScriptType* pcontent)
{
    #if PSYCH_LANGUAGE == PSYCH_MATLAB
        return(mexPutVariable(workspace, variable, pcontent));
    #else
        PsychErrorExitMsg(PsychError_unimplemented, "Function PsychRuntimePutVariable() not yet supported for this runtime system!");
    #endif
}


/* PsychRuntimeGetVariable() TODO FIXME
 *
 * Copy a given native variable of type PsychGenericScriptType, as a new variable from a specified workspace.
 *
 * 'workspace'    Namestring of workspace: "base" copy from base workspace. "caller" copy from calling functions workspace,
 *                'global' get global variable with given name.
 *
 *                CAUTION:    Matlab and Octave show different behaviour when using the "caller" workspace! It is strongly
 *                            recommended to avoid the "caller" workspace to avoid ugly compatibility bugs!!
 *
 * 'variable'    Name of the variable to get a copy of.
 *
 * 'pcontent'    Pointer to a PsychGenericScriptType* where the location of the new variables content should be stored.
 *                The pointed to pointer will be set to NULL on failure.
 *
 * Returns TRUE on success, FALSE on failure.
 */
psych_bool PsychRuntimeGetVariable(const char* workspace, const char* variable, PsychGenericScriptType** pcontent)
{
    // Init to empty default:
    *pcontent = NULL;

    #if PSYCH_LANGUAGE == PSYCH_MATLAB
        *pcontent = mexGetVariable(workspace, variable);

        // Return true on success, false on failure:
        return((*pcontent) ? TRUE : FALSE);
    #else
        PsychErrorExitMsg(PsychError_unimplemented, "Function PsychRuntimeGetVariable() not yet supported for this runtime system!");
    #endif
}


/* PsychRuntimeGetVariablePtr() TODO FIXME
 *
 * Retrieve a *read-only* pointer to a given native variable of type PsychGenericScriptType in the specified workspace.
 * The variable is not copied, just referenced, so you *must not modify/write to the location* only perform read access!
 *
 * 'workspace'    Namestring of workspace: "base" get from base workspace. "caller" get from calling functions workspace,
 *                'global' get global variable with given name.
 *
 *                CAUTION:    Matlab and Octave show different behaviour when using the "caller" workspace! It is strongly
 *                            recommended to avoid the "caller" workspace to avoid ugly compatibility bugs!!
 *
 * 'variable'    Name of the variable to get a reference.
 *
 * 'pcontent'    Pointer to a PsychGenericScriptType* where the location of the variables content should be stored.
 *                The pointed to pointer will be set to NULL on failure.
 *
 * Returns TRUE on success, FALSE on failure.
 */
psych_bool PsychRuntimeGetVariablePtr(const char* workspace, const char* variable, PsychGenericScriptType** pcontent)
{
    // Init to empty default:
    *pcontent = NULL;

    #if PSYCH_LANGUAGE == PSYCH_MATLAB
        *pcontent = (PsychGenericScriptType*) mexGetVariablePtr(workspace, variable);

        // Return true on success, false on failure:
        return((*pcontent) ? TRUE : FALSE);
    #else
        PsychErrorExitMsg(PsychError_unimplemented, "Function PsychRuntimeGetVariablePtr() not yet supported for this runtime system!");
    #endif
}

/* PsychRuntimeEvaluateString() TODO FIXME
 *
 * Simple function evaluation by scripting environment via feval() style functions.
 * This asks the runtime environment to execute/evaluate the given string 'cmdstring',
 * passing no return arguments back, except an error code.
 *
 */
int PsychRuntimeEvaluateString(const char* cmdstring)
{
    #if PSYCH_LANGUAGE == PSYCH_MATLAB
        return(mexEvalString(cmdstring));
    #else
        printf("Function PsychRuntimeEvaluateString() not yet supported for this runtime system!\n");
    #endif
}

// functions for outputting structs
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    PsychAllocOutStructArray() TODO FIXME

    -If argument is optional we allocate the structure even if the argument is not present.  If this bothers you,
    then check within the subfunction for the presense of a return argument before creating the struct array.  We
    allocate space regardeless of whether the argument is present because this is consistant with other "PsychAllocOut*"
    functions which behave this way because in some situations subfunctions might derive returned results from values
    stored in an optional argument.

    -If position is -1 then don't attempt to return the created structure to the calling environment.  Instead just
    allocate the structure and return it in pStruct.  This is how to create a structure which is embeded within another
    structure using PsychSetStructArrayStructArray().  Note that we use -1 as the flag and not NULL because NULL is 0 and
    0 is reserved for future use as a reference to the subfunction name, of if none then the function name.
*/
psych_bool PsychAllocOutStructArray(int position, 
                                    PsychArgRequirementType isRequired, 
                                    int numElements,
                                    int numFields, 
                                    const char **fieldNames,  
                                    PsychGenericScriptType **pStruct)
{
    PyObject **mxArrayOut;
    ptbSize structArrayNumDims = 2;
    ptbSize structArrayDims[2];
    PsychError matchError;
    psych_bool putOut;

    structArrayDims[0] = 1;
    structArrayDims[1] = numElements;

    if (position != kPsychNoArgReturn) {  //Return the result to both the C caller and the scripting environment.
        PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
        PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_structArray, isRequired, 1,1,numElements,numElements,0,0);
        *pStruct = mxCreateStructArray(structArrayNumDims, structArrayDims, numFields, fieldNames);
        matchError = PsychMatchDescriptors();
        putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
        if (putOut) {
            mxArrayOut = PsychGetOutArgPyPtr(position);
            *mxArrayOut = *pStruct;
        }
        return(putOut);
    } else{ //Return the result only to the C caller.  Ignore "required".    
        *pStruct = mxCreateStructArray(structArrayNumDims, structArrayDims, numFields, fieldNames);
        return(TRUE);
    }
}


/*  FONTS on OSX only.
    PsychAssignOutStructArray() TODO FIXME
    Accept a pointer to a struct array and Assign the struct array to be the designated return variable.
*/
psych_bool PsychAssignOutStructArray( int position,
                                      PsychArgRequirementType isRequired,
                                      PsychGenericScriptType *pStruct)
{
    PyObject **mxArrayOut;
    PsychError matchError;
    psych_bool putOut;

    PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
    PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_structArray, isRequired, 1,1,0,kPsychUnboundedArraySize,0,0);
    matchError = PsychMatchDescriptors();
    putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
    if (putOut) {
        mxArrayOut = PsychGetOutArgPyPtr(position);
        *mxArrayOut = pStruct;
    }
    return(putOut);
}

// functions for filling in struct elements by type 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
    PsychSetStructArrayStringElement() TODO FIXME
    The variable "index", the index of the element within the struct array, is zero-indexed.
*/
void PsychSetStructArrayStringElement(const char *fieldName,
                                      int index,
                                      char *text,
                                      PsychGenericScriptType *pStruct)
{
    int fieldNumber;
    size_t numElements;
    psych_bool isStruct;
    PyObject *mxFieldValue;
    char errmsg[256];

    //check for bogus arguments
    numElements = mxGetM(pStruct) * mxGetN(pStruct);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a structure field at an out-of-bounds index");

    fieldNumber = mxGetFieldNumber(pStruct, fieldName);
    if (fieldNumber == -1) {
        sprintf(errmsg, "Attempt to set a non-existent structure name field: %s", fieldName);
        PsychErrorExitMsg(PsychError_internal, errmsg);
    }

    isStruct = mxIsStruct(pStruct);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a field within a non-existent structure.");

    //do stuff
    mxFieldValue = mxCreateString(text);
    mxSetField(pStruct, (ptbIndex) index, fieldName, mxFieldValue);
    if (PSYCH_LANGUAGE == PSYCH_OCTAVE) mxDestroyArray(mxFieldValue);
}


/*
    PsychSetStructArrayDoubleElement() TODO FIXME
    Note: The variable "index" is zero-indexed.
*/
void PsychSetStructArrayDoubleElement(const char *fieldName,
                                      int index,
                                      double value,
                                      PsychGenericScriptType *pStruct)
{
    int fieldNumber;
    size_t numElements;
    psych_bool isStruct;
    PyObject *mxFieldValue;
    char errmsg[256];

    //check for bogus arguments
    numElements = mxGetM(pStruct) * mxGetN(pStruct);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a structure field at an out-of-bounds index");

    fieldNumber = mxGetFieldNumber(pStruct, fieldName);
    if (fieldNumber == -1) {
        sprintf(errmsg, "Attempt to set a non-existent structure name field: %s", fieldName);
        PsychErrorExitMsg(PsychError_internal, errmsg);
    }

    isStruct = mxIsStruct(pStruct);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a field within a non-existent structure.");

    //do stuff
    mxFieldValue = mxCreateDoubleMatrix(1, 1);
    mxGetPr(mxFieldValue)[0] = value;
    mxSetField(pStruct, (ptbIndex) index, fieldName, mxFieldValue);
    if (PSYCH_LANGUAGE == PSYCH_OCTAVE) mxDestroyArray(mxFieldValue);
}


/*
    PsychSetStructArrayBooleanElement()
TODO FIXME
    Note: The variable "index" is zero-indexed.
*/
void PsychSetStructArrayBooleanElement( const char *fieldName,
                                        int index,
                                        psych_bool state,
                                        PsychGenericScriptType *pStruct)
{
    int fieldNumber;
    size_t numElements;
    psych_bool isStruct;
    PyObject *mxFieldValue;
    char errmsg[256];

    //check for bogus arguments
    numElements = mxGetM(pStruct) * mxGetN(pStruct);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a structure field at an out-of-bounds index");

    fieldNumber = mxGetFieldNumber(pStruct, fieldName);
    if (fieldNumber == -1) {
        sprintf(errmsg, "Attempt to set a non-existent structure name field: %s", fieldName);
        PsychErrorExitMsg(PsychError_internal, errmsg);
    }

    isStruct = mxIsStruct(pStruct);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a field within a non-existent structure.");

    //do stuff
    mxFieldValue = mxCreateLogicalMatrix(1, 1);
    mxGetLogicals(mxFieldValue)[0] = state;
    mxSetField(pStruct, (ptbIndex) index, fieldName, mxFieldValue);
    if (PSYCH_LANGUAGE == PSYCH_OCTAVE) mxDestroyArray(mxFieldValue);
}


/*
    PsychSetStructArrayStructElement() TODO FIXME
*/
void PsychSetStructArrayStructElement(const char *fieldName,
                                      int index,
                                      PsychGenericScriptType *pStructInner,
                                      PsychGenericScriptType *pStructOuter)
{
    int fieldNumber;
    size_t numElements;
    psych_bool isStruct;
    char errmsg[256];

    //check for bogus arguments
    numElements = mxGetM(pStructOuter) * mxGetN(pStructOuter);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a structure field at an out-of-bounds index");

    fieldNumber = mxGetFieldNumber(pStructOuter, fieldName);
    if (fieldNumber == -1) {
        sprintf(errmsg, "Attempt to set a non-existent structure name field: %s", fieldName);
        PsychErrorExitMsg(PsychError_internal, errmsg);
    }

    isStruct = mxIsStruct(pStructInner);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a struct field to a non-existent structure.");

    isStruct = mxIsStruct(pStructOuter);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a field within a non-existent structure.");

    //do stuff
    mxSetField(pStructOuter, (ptbIndex) index, fieldName, pStructInner); 
    if (PSYCH_LANGUAGE == PSYCH_OCTAVE) mxDestroyArray(pStructInner);
}


/*
    PsychSetStructArrayNativeElement() TODO FIXME
 */
void PsychSetStructArrayNativeElement(const char *fieldName,
                                      int index,
                                      PsychGenericScriptType *pNativeElement,
                                      PsychGenericScriptType *pStructArray)
{
    int fieldNumber;
    size_t numElements;
    psych_bool isStruct;
    char errmsg[256];

    //check for bogus arguments
    numElements = mxGetM(pStructArray) * mxGetN(pStructArray);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a structure field at an out-of-bounds index");

    fieldNumber = mxGetFieldNumber(pStructArray, fieldName);
    if (fieldNumber == -1) {
        sprintf(errmsg, "Attempt to set a non-existent structure name field: %s", fieldName);
        PsychErrorExitMsg(PsychError_internal, errmsg);
    }

    isStruct = mxIsStruct(pStructArray);
    if (!isStruct)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a field within a non-existent structure.");

    //do stuff
    mxSetField(pStructArray, (ptbIndex) index, fieldName, pNativeElement);
}


/*
 *    PsychAllocOutCellVector() TODO FIXME
 *
 *    -If argument is optional we allocate the structure even if the argument is not present.  If this behavior bothers you,
 *    then check within your code for the presense of a return argument before creating the struct array.  We
 *    allocate space regardeless of whether the argument is present because this is consistant with other "PsychAllocOut*"
 *    functions which behave this way because in some situations subfunctions might derive returned results from values
 *    stored in an optional argument.
 *
 *    -If position is -1 then don't attempt to return the created structure to the calling environment.  Instead just
 *    allocate the structure and return it in pStruct.  This is how to create a structure which is embeded within another
 *    structure using PsychSetStructArrayStructArray().  Note that we use -1 as the flag and not NULL because NULL is 0 and
 *    0 is reserved for future use as a reference to the subfunction name, of if none then the function name.
 */
psych_bool PsychAllocOutCellVector(int position,
                                   PsychArgRequirementType isRequired,
                                   int numElements,
                                   PsychGenericScriptType **pCell)
{
    PyObject **mxArrayOut;
    ptbSize cellArrayNumDims = 2;
    ptbSize cellArrayDims[2];
    PsychError matchError;
    psych_bool putOut;

    cellArrayDims[0] = 1;
    cellArrayDims[1] = numElements;

    if (position != kPsychNoArgReturn) {  //Return the result to both the C caller and the scripting environment.
        PsychSetReceivedArgDescriptor(position, FALSE, PsychArgOut);
        PsychSetSpecifiedArgDescriptor(position, PsychArgOut, PsychArgType_cellArray, isRequired, 1,1,numElements,numElements,0,0);
        *pCell = mxCreateCellArray(cellArrayNumDims, cellArrayDims);
        mxArrayOut = PsychGetOutArgPyPtr(position);
        matchError = PsychMatchDescriptors();
        putOut = PsychAcceptOutputArgumentDecider(isRequired, matchError);
        if (putOut)
            *mxArrayOut=*pCell;
        return(putOut);
    } else { //Return the result only to the C caller, not to the calling environment.  Ignore "required".
        *pCell = mxCreateCellArray(cellArrayNumDims, cellArrayDims);
        return(TRUE);
    }
}


/*
 *    PsychSetCellVectorStringElement() TODO FIXME
 *
 *    The variable "index", the index of the element within the struct array, is zero-indexed.
 */
void PsychSetCellVectorStringElement(int index,
                                     const char *text,
                                     PsychGenericScriptType *cellVector)
{
    size_t numElements;
    psych_bool isCell;
    PyObject *mxFieldValue;

    //check for bogus arguments
    numElements = mxGetM(cellVector) * mxGetN(cellVector);
    if ((size_t) index >= numElements)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a cell array field at an out-of-bounds index");

    isCell = mxIsCell(cellVector);
    if (!isCell)
        PsychErrorExitMsg(PsychError_internal, "Attempt to set a cell within a non-existent cell array.");

    //do stuff
    mxFieldValue = mxCreateString(text);
    mxSetCell(cellVector, (ptbIndex) index, mxFieldValue);
    if (PSYCH_LANGUAGE == PSYCH_OCTAVE) mxDestroyArray(mxFieldValue);
}

// End of Python only stuff.
