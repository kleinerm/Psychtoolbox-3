/* These are hacks to get the Windows version running - to be replaced by a
   proper solution(TM) soon...
*/

#include "mogltypes.h"

double gluCheckExtension(const GLubyte* a, const GLubyte* b) {
	printf("MOGL-WARNING: gluCheckExtension() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}
double gluUnProject4(double a1, double a2, double a3, double a4, double* a5, double* a6, int* a7, double a8, double a9, double* a10, double* a11, double* a12, double* a13)
{
	printf( "MOGL-WARNING: gluUnproject4() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}
double gluBuild1DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, void* a9)
{
	printf( "MOGL-WARNING: gluBuild1DMipmapLevels() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}
double gluBuild2DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, double a9, void* a10)
{
	printf( "MOGL-WARNING: gluBuild2DMipmapLevels() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}
double gluBuild3DMipmapLevels(double a1, double a2, double a3, double a4, double a5, double a6, double a7, double a8, double a9, double a10, void* a11)
{
	printf( "MOGL-WARNING: gluBuild3DMipmapLevels() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}
double gluBuild3DMipmaps(double a1, double a2, double a3, double a4, double a5, double a6, double a7, void* a8)
{
	printf( "MOGL-WARNING: gluBuild3DMipmaps() called - Unsupported on Windows for now!!!\n");
	fflush(NULL);
   return(0);
}

mxArray* mxCreateNumericMatrix(int m, int n, int class, int complex)
{
 int dims[2];
 dims[0]=m;
 dims[1]=n;
 
 return(mxCreateNumericArray(2, dims, class, complex));
}
