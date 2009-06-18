// Author: Paul Kienzle, 2001-03-22
// I grant this code to the public domain.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// 2001-06-21 Paul Kienzle <pkienzle@users.sf.net>
// * use unsigned short for mxChar rather than char

/* mex.h is for use in C-programs only; do NOT include it in mex.cc */

#ifndef MEX_H
#define MEX_H

#define HAVE_OCTAVE
typedef struct mxArray {
  void* o;  // Pointer to octave_value...
  void* d;  // Pointer to data in octave_value
} mxArray;

typedef unsigned short mxChar;
enum mxComplexity { mxREAL=0, mxCOMPLEX=1 };

/* -V4 stuff */
#if defined(V4)
#define Matrix mxArray
#define REAL mxREAL
#endif

#define mxMAXNAME 64

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(V4)
  void mexFunction(int nlhs, mxArray* plhs[], int nrhs, mxArray* prhs[]);
#else
  void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]);
#endif
  
  /* Floating point representation */
  psych_bool mxIsNaN(double v);
  psych_bool mxIsFinite(double v);
  psych_bool mxIsInf(double v);
  double mxGetEps(void);
  double mxGetInf(void);
  double mxGetNaN(void);
  
  /* V4 floating point routines renamed in V5 */
#define mexIsNaN mxIsNaN
#define mexIsFinite mxIsFinite
#define mexIsInf mxIsInf
#define mexGetEps mxGetEps
#define mexGetInf mxGetInf
#define mexGetNaN mxGetNan
  
  /* Interface to the interpreter */
  extern const char *mexFunctionName;
  int mexCallMATLAB(const int nargout, mxArray* argout[], 
		    const int nargin, const mxArray* argin[],
		    const char* fname);
  void mexSetTrapFlag(int flag);
  int mexEvalString (const char *s);
  void mexErrMsgTxt (const char *s);
  void mexWarnMsgTxt (const char *s);
  void mexPrintf (const char *fmt, ...);
  
  mxArray* mexGetArray(const char *name, const char *space);
  mxArray* mexGetArrayPtr(const char *name, const char *space);
#define mexGetGlobal(nm) mexGetArray(nm,"global")
#define mexGetMatrix(nm) mexGetArray(nm,"caller")
#define mexGetMatrixPtr(nm) mexGetArrayPtr(nm,"caller")
  int mexPutArray(mxArray* ptr, const char *space);
#define mexPutMatrix(nm) mexPutArray(nm,"caller")
  
  
  /* Memory */
  void *mxMalloc(int n);
  void *mxCalloc(int n, int size);
  void mxFree(void *ptr);
  void mexMakeArrayPersistent(mxArray *ptr);
  void mexMakeMemoryPersistent(void *ptr);
  
  /* interpreter values */
  mxArray* mxCreateDoubleMatrix(int nr, int nc, int iscomplex);
#define mxCreateFull mxCreateDoubleMatrix
  void mxDestroyArray(mxArray *v);
#define mxFreeMatrix mxDestroyArray
  int mxIsChar (const mxArray* ptr);
#define mxIsString mxIsChar
  int mxIsSparse (const mxArray* ptr);
  int mxIsStruct (const mxArray* ptr);
  int mxIsFull (const mxArray* ptr);
  int mxIsDouble (const mxArray* ptr);
  int mxIsNumeric (const mxArray* ptr);
  int mxIsComplex (const mxArray* ptr);
  int mxIsEmpty (const mxArray* ptr);
  int mxIsCell (const mxArray* ptr);

  int mxGetM (const mxArray* ptr);
  int mxGetN (const mxArray* ptr);
  int mxGetNumberOfDimensions (const mxArray* ptr);
  int mxGetNumberOfElements (const mxArray* ptr);
  double* mxGetPr (const mxArray* ptr);
  void* mxGetData(const mxArray* arrayPtr);
  mxArray* mxCreateLogicalMatrix(int rows, int cols);
#if PSYCH_SYSTEM == PSYCH_LINUX
  psych_bool* mxGetLogicals(const mxArray* arrayPtr);
#else
  psych_bool* mxGetLogicals(const mxArray* arrayPtr);
#endif

  /* structure support */
  int mxIsStruct (const mxArray* ptr);
  mxArray* mxGetField(const mxArray* ptr, int index, const char *key);
  void mxSetField(mxArray* ptr, int index, const char *key, mxArray* val);
  int mxGetNumberOfFields(const mxArray* ptr);
  const char* mxGetFieldNameByNumber(const mxArray* ptr, int key_num);
  //  int mxGetFieldNumber(const mxArray* ptr, const char *key);
  int mxGetFieldNumber(const mxArray* ptr, const char *key);
  mxArray* mxGetFieldByNumber(const mxArray* ptr, int index, int key_num);
  void mxSetFieldByNumber(mxArray* ptr, int index, int key_num, mxArray* val);
  mxArray* mxCreateStructMatrix (int rows, int cols,
				 int num_keys, const char **keys);
#if 0
  mxArray* mxCreateStructArray (int num_dims, const int * dims, 
				int numkeys, const char **keys);
#endif

  void mxSetCell(mxArray *cellVector, int index, mxArray* mxFieldValue);
  mxArray* mxCreateCellArray(int numDims, int* ArrayDims);
  mxArray* mxCreateStructArray(int numDims, int* ArrayDims, int numFields, const char** fieldNames);

#if 0
  /* The following cannot be supported in Octave without incurring
   * the large runtime penalty of copying arrays to/from matlab format */
   double* mxGetPi (const mxArray* ptr);
   void mxSetM (mxArray* ptr, const int M);
   void mxSetN (mxArray* ptr, const int N);
   void mxSetPr (mxArray* ptr, double* pr);
   void mxSetPi (mxArray* ptr, double* pi);
#endif
  
  
  
  int mxGetString (const mxArray* ptr, char *buf, int buflen);
  char *mxArrayToString (const mxArray* ptr);
  mxArray *mxCreateString (const char *str);
  
  double mxGetScalar (const mxArray* ptr);
  
#if defined(__cplusplus)
}
#endif

#endif /* !defined(MEX_H) */

