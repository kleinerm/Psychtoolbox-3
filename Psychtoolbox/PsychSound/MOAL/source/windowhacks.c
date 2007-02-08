/* These are hacks to get the Windows version running - to be replaced by a
   proper solution(TM) soon...
*/

#include "moaltypes.h"

mxArray* mxCreateNumericMatrix(int m, int n, int class, int complex)
{
 int dims[2];
 dims[0]=m;
 dims[1]=n;
 
 return(mxCreateNumericArray(2, dims, class, complex));
}
