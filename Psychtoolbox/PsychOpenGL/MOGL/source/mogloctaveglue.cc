/* mogloctaveglue.cc -- Glue code for interfacing moglcore to GNU/Octave:
 *
 * This code reimplements and exports a couple of Matlab MEX-API functions
 * that are needed by all the code in mogl in a way that is hopefully
 * efficient enough for our purpose.
 *
 * Most of the code is copied and adapted from Psychtoolbox file
 * PsychSourceGL/Source/Common/Base/PsychScriptingGlue.cc
 *
 */

// Include our own datatype and pseudo mex function definitions:
#include "mogltypes.h"

#include <string.h>

// This jump-buffer stores CPU- and stackstate at the position
// where our octFunction() dispatcher actually starts executing
// the functions or subfunctions code. That is the point where
// a mexErrMsgTxt() will return control...
extern jmp_buf jmpbuffer;

#define PsychErrorExitMsg(a,b) mexErrMsgTxt((b))

// Error exit handler: Replacement for Matlabs MEX-handler:
// Prints the error-string with Octaves error printing facilities,
// sets Octave error state and longjmp's to the cleanup routine
// at the end of our octFunction dispatcher...
void mexErrMsgTxt(const char* s) {
  if (s && strlen(s)>0) {
    error("moglcore: %s",s);
  }
  else error("");

  // Use the jump-buffer to unwind the stack...
  longjmp(jmpbuffer, 1);
}

// Interface to Octave's printf...
void mexPrintf(const char* fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  octave_vformat(octave_diary, fmt, args);
  octave_vformat(octave_stdout, fmt, args);
  va_end(args);
}

void* mxMalloc(int size)
{
  return(PsychMallocTemp((unsigned long) size, 0));
}

void* mxCalloc(int size, int numelements)
{
  return(PsychCallocTemp((unsigned long) size, (unsigned long) numelements,0));
}

void mxFree(void* p)
{
  PsychFreeTemp(p, 0);
}

int mexCallMATLAB(const int nargout, mxArray* argout[], 
		  const int nargin, const mxArray* argin[],
		  const char* fname)
{
  PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: Internal call to mexCallMATLAB(), which is not yet implemented on GNU/Octave port!");
}

double mxGetNaN(void)
{
  PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: Internal call to mxGetNan(), which is not yet implemented on GNU/Octave port!");
}

mxArray* mxCreateNumericArray(int numDims, int dimArray[], int arraytype, int realorcomplex)
{
  mxArray* retval;
  int rows, cols, layers;
  if (numDims>3) PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxCreateNumericArray: Tried to create matrix with more than 3 dimensions!");
  rows = dimArray[0];
  cols = (numDims>1) ? dimArray[1] : 1;
  layers = (numDims>2) ? dimArray[2] : 1;
  dim_vector mydims((numDims>2) ? dim_vector(rows, cols, layers) : dim_vector(rows, cols));

  // Allocate our mxArray-Struct:
  retval = (mxArray*) PsychMallocTemp(sizeof(mxArray),0);

  // Create corresponding octave_value object for requested type and size of matrix.
  // Retrieve raw pointer to contained data and store it in our mxArray struct as well...

  if (arraytype==mxUINT32_CLASS) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW UINT32 MATRIX: %i,%i,%i\n", rows, cols, layers); fflush(NULL);
    // Create empty uint32ND-Array of type mxREAL...
    uint32NDArray m(mydims);
    // Retrieve a pointer to internal representation. As m is new
    // this won't trigger a deep-copy.
    retval->d = (void*) m.data();
    if (DEBUG_PTBOCTAVEGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
    // Build a new oct_value object from Matrix m: This is a
    // shallow-copy.
    octave_value* ovp = new octave_value();
    *ovp = m;
    retval->o = (void*) ovp;
    // At this point we can safely destroy Matrix m, as the new
    // octave_object holds a reference to its representation.    
  }
  else if (arraytype==mxUINT8_CLASS) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW UINT8 MATRIX: %i,%i,%i\n", rows, cols, layers); fflush(NULL);
    // Create empty uint8ND-Array of type mxREAL...
    uint8NDArray m(mydims);
    // Retrieve a pointer to internal representation. As m is new
    // this won't trigger a deep-copy.
    retval->d = (void*) m.data();
    if (DEBUG_PTBOCTAVEGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
    // Build a new oct_value object from Matrix m: This is a
    // shallow-copy.
    octave_value* ovp = new octave_value();
    *ovp = m;
    retval->o = (void*) ovp;
    // At this point we can safely destroy Matrix m, as the new
    // octave_object holds a reference to its representation.    
  }
  else if (arraytype==mxDOUBLE_CLASS && rows*cols*layers > 1) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW DOUBLE MATRIX: %i,%i,%i\n", rows, cols, layers); fflush(NULL);
    // Create empty ND-Array of type mxREAL...
    NDArray m(mydims);
    // Retrieve a pointer to internal representation. As m is new
    // this won't trigger a deep-copy.
    retval->d = (void*) m.data();
    if (DEBUG_PTBOCTAVEGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
    // Build a new oct_value object from Matrix m: This is a
    // shallow-copy.
    octave_value* ovp = new octave_value();
    *ovp = m;
    retval->o = (void*) ovp;
    // At this point we can safely destroy Matrix m, as the new
    // octave_object holds a reference to its representation.
  }
  else if (arraytype==mxUINT32_CLASS && rows*cols*layers == 1) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW UINT32 SCALAR:\n"); fflush(NULL);
    // Create empty uint32ND-Array of type mxREAL...
    uint32NDArray m(mydims);
    // Retrieve a pointer to internal representation. As m is new
    // this won't trigger a deep-copy.
    unsigned int* ip = (unsigned int*) PsychMallocTemp(sizeof(unsigned int),0);
    retval->d = (void*) ip;
    if (DEBUG_PTBOCTAVEGLUE) printf("M-DATA %p\n", retval->d); fflush(NULL);
    // Build a new oct_value object from Matrix m: This is a
    // shallow-copy.
    octave_value* ovp = new octave_value();
    *ovp = m;
    retval->o = (void*) ovp;
    // At this point we can safely destroy Matrix m, as the new
    // octave_object holds a reference to its representation.    
  }
  else if (arraytype==mxDOUBLE_CLASS && rows*cols*layers == 1) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW SCALAR:\n"); fflush(NULL);
    // This is a scalar value:
    retval->o = (void*) new octave_value(0.0);
    double* dp = (double*) PsychMallocTemp(sizeof(double),0);
    retval->d = (void*) dp;
  }
  else if (arraytype==mxDOUBLE_CLASS && rows*cols*layers == 0) {
    // Special case: Empty matrix.
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW EMPTY DOUBLE MATRIX:\n"); fflush(NULL);
    retval->o = (void*) new octave_value(Matrix(0,0));
    retval->d = NULL;
  }
  else if (arraytype==mxLOGICAL_CLASS) {
    if (DEBUG_PTBOCTAVEGLUE) printf("NEW BOOLMATRIX: %i, %i\n", rows, cols, layers); fflush(NULL);
    // Create empty double-matrix of type mxREAL...
    if (layers>1) PsychErrorExitMsg(PsychError_internal, "In mxCreateNumericArray: Tried to allocate a 3D boolean matrix!?! Unsupported.");

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

  return(retval);
}

mxArray* mxCreateNumericMatrix(int rows, int cols, int arraytype, int iscomplex)
{
  int dims[2];
  dims[0]=rows;
  dims[1]=cols;
  return(mxCreateNumericArray(2, dims, arraytype, iscomplex));  
}

mxArray* mxCreateDoubleMatrix(int rows, int cols, int realorcomplex)
{
  int dims[2];
  dims[0]=rows;
  dims[1]=cols;
  return(mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL));  
}

mxArray* mxCreateLogicalMatrix(int rows, int cols)
{
  int dims[2];
  dims[0]=rows;
  dims[1]=cols;
  return(mxCreateNumericArray(2, dims, mxLOGICAL_CLASS, mxREAL));
}

mxArray* mxCreateString(const char* instring)
{
  mxArray* retval;

  // Allocate our mxArray-Struct:
  retval = (mxArray*) PsychMallocTemp(sizeof(mxArray),0);
  
  retval->d = NULL;
  retval->o = (void*) new octave_value(instring);
  return(retval);
}

void* mxGetData(const mxArray* arrayPtr)
{
  return(arrayPtr->d);
}

double* mxGetPr(const mxArray* arrayPtr)
{
  return((double*) mxGetData(arrayPtr));
}

bool* mxGetLogicals(const mxArray* arrayPtr)
{
  return((bool*) mxGetData(arrayPtr));
}
#define GETOCTPTR(x) ((octave_value*) (x)->o)

int mxGetM(const mxArray* arrayPtr)
{
  return(GETOCTPTR(arrayPtr)->rows());
}

int mxGetN(const mxArray* arrayPtr)
{
  return(GETOCTPTR(arrayPtr)->columns());
}

int mxGetNumberOfDimensions(const mxArray* arrayPtr)
{
  return(GETOCTPTR(arrayPtr)->ndims());
}

int* mxGetDimensions(const mxArray* arrayPtr)
{
  dim_vector vdim = GETOCTPTR(arrayPtr)->dims();
  int* dims = (int*) PsychMallocTemp(3*sizeof(int),0);
  dims[0] = vdim(0);
  dims[1] = vdim(1);
  if (GETOCTPTR(arrayPtr)->ndims()>2) dims[2] = vdim(2); else dims[2]=1;
  return(dims);
}

double mxGetScalar (const mxArray* ptr)
{
  octave_value* p = GETOCTPTR(ptr);
  if (p->is_scalar_type() && !(strstr(p->type_name().c_str(), "uint32"))) return(*((double*) (ptr->d)));
  return(p->double_value());
}

int mxGetString(const mxArray* arrayPtr, char* outstring, int outstringsize)
{
  if (!mxIsChar(arrayPtr)) PsychErrorExitMsg(PsychError_internal, "FATAL Error: Tried to convert a non-string into a string!");
  return(((snprintf(outstring, outstringsize, "%s", GETOCTPTR(arrayPtr)->string_value().c_str()))>0) ? 0 : 1);
}

void mxDestroyArray(mxArray *arrayPtr)
{
  // Destroy a mxArray:
  if (arrayPtr == NULL) return;

  // We only need to destroy the octave_value object referenced by arrayPtr,
  // because possible data buffers referenced by the ->d field and the
  // mxArray struct itself are allocted via PsychMallocTemp() anyway, so
  // they get automatically released when exiting our octFile...
  octave_value* ov = (octave_value*) arrayPtr->o;
  if (ov) delete(ov);
  arrayPtr->o = NULL;
  return;
}

mxArray* mxCreateStructArray(int numDims, int* ArrayDims, int numFields, const char** fieldNames)
{
  mxArray* retval;
#if PSYCH_SYSTEM != PSYCH_OSX
  if (numDims>2 || numDims<1) PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: mxCreateStructArray: Anything else than 1D or 2D Struct-Arrays is not supported!");
  if (numFields<1) PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxCreateStructArray: numFields < 1 ?!?");

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

  // Build our mxArray wrapper:
  retval = (mxArray*) PsychMallocTemp(sizeof(mxArray),0);

  // Fill it: Assign our map.
  octave_value* ovp = new octave_value(mymap);
  retval->o = (void*) ovp;
  retval->d = NULL;
#else
  PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: mxCreateStructArray: Not implemented on OS-X!");
#endif
  
  return(retval);
}

int mxGetFieldNumber(const mxArray* structArray, const char* fieldName)
{
  if(!mxIsStruct(structArray)) {
    PsychErrorExitMsg(PsychError_internal, "FATAL Error: mxGetFieldNumber: Tried to manipulate something other than a struct-Array!");
  }

  // Retrieve map:
  octave_value* ov = (octave_value*) structArray->o;
  Octave_map om = ov->map_value();

  // Find idx of key.
  std::string mykey(fieldName);
  for(int i=0; i<om.length(); i++) if (mykey == om.keys()(i)) return(i);

  // No such key :(
  return(-1);
}

void mxSetField(mxArray* pStructOuter, int index, const char* fieldName, mxArray* pStructInner)
{
  if(!mxIsStruct(pStructOuter)) {
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
}

mxArray* mxCreateCellArray(int numDims, int* ArrayDims)
{
  mxArray* retval;

  // Allocate our mxArray-Struct:
  retval = (mxArray*) PsychMallocTemp(sizeof(mxArray),0);

  if (numDims>2) PsychErrorExitMsg(PsychError_unimplemented, "FATAL Error: mxCreateCellArray: 3D Cell Arrays are not supported yet on GNU/Octave build!");

  // Create dimension vector:
  dim_vector mydims((numDims>1) ? dim_vector(ArrayDims[0], ArrayDims[1]) : dim_vector(ArrayDims[0]));
  
  // Create Cell object:
  Cell myCell(mydims);
  retval->o = (void*) new octave_value(myCell);
  retval->d = NULL;

  // Done.
  return(retval);
}

void mxSetLogical(mxArray* dummy)
{
  // This is a no-op on Octave build, because it is not needed anywhere...
  return;
}

int mxIsLogical(const mxArray* a)
{
  return(GETOCTPTR(a)->is_bool_type());
}

int mxIsCell(const mxArray* a)
{
  return(GETOCTPTR(a)->is_cell());
}

int mxIsStruct(const mxArray* a)
{
  return(GETOCTPTR(a)->is_map());
}

int mxIsNumeric(const mxArray* a)
{
  return(GETOCTPTR(a)->is_numeric_type());
}

int mxIsChar(const mxArray* a)
{
  return(GETOCTPTR(a)->is_char_matrix() || GETOCTPTR(a)->is_string());
}

int mxIsDouble(const mxArray* a)
{
  return(GETOCTPTR(a)->is_real_type());
}

int mxIsUint8(const mxArray* a)
{
  if (DEBUG_PTBOCTAVEGLUE) printf("BYTESIZE %i NUMEL %i --> mxIsUint8 = %s\n", GETOCTPTR(a)->byte_size(), GETOCTPTR(a)->numel(),
	 ((mxIsNumeric(a) && (GETOCTPTR(a)->byte_size() / GETOCTPTR(a)->numel() == 1))) ? "TRUE":"FALSE");

  return((mxIsNumeric(a) && (GETOCTPTR(a)->byte_size() / GETOCTPTR(a)->numel() == 1)) ? true : false);
}

int mxIsUint16(const mxArray* a)
{
  return((mxIsNumeric(a) && (GETOCTPTR(a)->byte_size() / GETOCTPTR(a)->numel() == 2)) ? true : false);
  //  int t=GETOCTPTR(a)->type_id();
  //return(t==octave_uint16_matrix::static_type_id());
}

int mxIsUint32(const mxArray* a)
{
  return((mxIsNumeric(a) && (GETOCTPTR(a)->byte_size() / GETOCTPTR(a)->numel() == 4)) ? true : false);

  //  int t=GETOCTPTR(a)->type_id();
  // return(t==octave_uint32_matrix::static_type_id());
}

int mxIsInt8(const mxArray* a)
{
  return(mxIsUint8(a));
  //  int t=GETOCTPTR(a)->type_id();
  //return(t==octave_int8_matrix::static_type_id());
}

int mxIsInt16(const mxArray* a)
{
  return(mxIsUint16(a));
  //  int t=GETOCTPTR(a)->type_id();
  //return(t==octave_int16_matrix::static_type_id());
}

int mxIsInt32(const mxArray* a)
{
  return(mxIsUint32(a));
  //int t=GETOCTPTR(a)->type_id();
  //return(t==octave_int32_matrix::static_type_id());
}

char* mxArrayToString(mxArray* arrayPtr)
{
  char* outstr = (char*) mxCalloc(mxGetM(arrayPtr)*mxGetN(arrayPtr)+1, 1);
  char* inp = (char*) arrayPtr->d;

  if (mxIsChar(arrayPtr)) {
    mxGetString(arrayPtr, outstr, mxGetM(arrayPtr)*mxGetN(arrayPtr)+1);
  }
  else {
    memcpy(outstr, inp, mxGetM(arrayPtr)*mxGetN(arrayPtr));
  }

  return(outstr);
}
