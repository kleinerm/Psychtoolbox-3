/* These are hacks to get the Windows version running - to be replaced by a
   proper solution(TM) soon...

   27-Mar-2011 -- Make 64-bit clean (MK).
*/

#include "moaltypes.h"

#ifndef PTBOCTAVE3MEX
#ifndef TARGET_OS_WIN32

mxArray* mxCreateNumericMatrix(int m, int n, int class, int complex)
{
/* On Matlab R11 builds, we use int for the dims array.
 * On R2007a and later, we use the defined mwSize type, which
 * will properly adapt to 32-bit on 32-bit builds and 64-bit on
 * 64-bit builds of PTB. The TARGET_OS_WIN32 is only defined on >= R2007a.
 */
#ifndef TARGET_OS_WIN32
 /* R11 build: Legacy int definition: */
 int dims[2];
 dims[0]=m;
 dims[1]=n;
#else
 /* R2007a and later build: Proper 64-bit clean definition: */
 mwSize dims[2];
 dims[0] = (mwSize) m;
 dims[1] = (mwSize) n;
#endif

 return(mxCreateNumericArray(2, dims, class, complex));
}
#endif
#endif
