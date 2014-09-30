
/*
 * gl_auto.c
 *
 * 30-Sep-2014 -- created (moglgen)
 *
 */

#include "mogltypes.h"

void gl_accum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAccum) mogl_glunsupported("glAccum");
	glAccum((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_alphafunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAlphaFunc) mogl_glunsupported("glAlphaFunc");
	glAlphaFunc((GLenum)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]));

}

void gl_aretexturesresident( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAreTexturesResident) mogl_glunsupported("glAreTexturesResident");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glAreTexturesResident((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_arrayelement( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glArrayElement) mogl_glunsupported("glArrayElement");
	glArrayElement((GLint)mxGetScalar(prhs[0]));

}

void gl_begin( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBegin) mogl_glunsupported("glBegin");
	glBegin((GLenum)mxGetScalar(prhs[0]));

}

void gl_bindtexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTexture) mogl_glunsupported("glBindTexture");
	glBindTexture((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_bitmap( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBitmap) mogl_glunsupported("glBitmap");
	glBitmap((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(const GLubyte*)mxGetData(prhs[6]));

}

void gl_blendcolor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendColor) mogl_glunsupported("glBlendColor");
	glBlendColor((GLclampf)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]),
		(GLclampf)mxGetScalar(prhs[2]),
		(GLclampf)mxGetScalar(prhs[3]));

}

void gl_blendequation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquation) mogl_glunsupported("glBlendEquation");
	glBlendEquation((GLenum)mxGetScalar(prhs[0]));

}

void gl_blendequationseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationSeparate) mogl_glunsupported("glBlendEquationSeparate");
	glBlendEquationSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFunc) mogl_glunsupported("glBlendFunc");
	glBlendFunc((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_calllist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCallList) mogl_glunsupported("glCallList");
	glCallList((GLuint)mxGetScalar(prhs[0]));

}

void gl_calllists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCallLists) mogl_glunsupported("glCallLists");
	glCallLists((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_clear( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClear) mogl_glunsupported("glClear");
	glClear((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_clearaccum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearAccum) mogl_glunsupported("glClearAccum");
	glClearAccum((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_clearcolor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearColor) mogl_glunsupported("glClearColor");
	glClearColor((GLclampf)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]),
		(GLclampf)mxGetScalar(prhs[2]),
		(GLclampf)mxGetScalar(prhs[3]));

}

void gl_cleardepth( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearDepth) mogl_glunsupported("glClearDepth");
	glClearDepth((GLclampd)mxGetScalar(prhs[0]));

}

void gl_clearindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearIndex) mogl_glunsupported("glClearIndex");
	glClearIndex((GLfloat)mxGetScalar(prhs[0]));

}

void gl_clearstencil( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearStencil) mogl_glunsupported("glClearStencil");
	glClearStencil((GLint)mxGetScalar(prhs[0]));

}

void gl_clipplane( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClipPlane) mogl_glunsupported("glClipPlane");
	glClipPlane((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_color3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3b) mogl_glunsupported("glColor3b");
	glColor3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_color3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3bv) mogl_glunsupported("glColor3bv");
	glColor3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_color3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3d) mogl_glunsupported("glColor3d");
	glColor3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_color3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3dv) mogl_glunsupported("glColor3dv");
	glColor3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_color3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3f) mogl_glunsupported("glColor3f");
	glColor3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_color3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3fv) mogl_glunsupported("glColor3fv");
	glColor3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_color3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3i) mogl_glunsupported("glColor3i");
	glColor3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_color3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3iv) mogl_glunsupported("glColor3iv");
	glColor3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_color3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3s) mogl_glunsupported("glColor3s");
	glColor3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_color3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3sv) mogl_glunsupported("glColor3sv");
	glColor3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_color3ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3ub) mogl_glunsupported("glColor3ub");
	glColor3ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]));

}

void gl_color3ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3ubv) mogl_glunsupported("glColor3ubv");
	glColor3ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_color3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3ui) mogl_glunsupported("glColor3ui");
	glColor3ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_color3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3uiv) mogl_glunsupported("glColor3uiv");
	glColor3uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_color3us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3us) mogl_glunsupported("glColor3us");
	glColor3us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]));

}

void gl_color3usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3usv) mogl_glunsupported("glColor3usv");
	glColor3usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_color4b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4b) mogl_glunsupported("glColor4b");
	glColor4b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]),
		(GLbyte)mxGetScalar(prhs[3]));

}

void gl_color4bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4bv) mogl_glunsupported("glColor4bv");
	glColor4bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_color4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4d) mogl_glunsupported("glColor4d");
	glColor4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_color4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4dv) mogl_glunsupported("glColor4dv");
	glColor4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_color4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4f) mogl_glunsupported("glColor4f");
	glColor4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_color4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4fv) mogl_glunsupported("glColor4fv");
	glColor4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_color4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4i) mogl_glunsupported("glColor4i");
	glColor4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_color4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4iv) mogl_glunsupported("glColor4iv");
	glColor4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_color4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4s) mogl_glunsupported("glColor4s");
	glColor4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_color4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4sv) mogl_glunsupported("glColor4sv");
	glColor4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_color4ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ub) mogl_glunsupported("glColor4ub");
	glColor4ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]));

}

void gl_color4ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ubv) mogl_glunsupported("glColor4ubv");
	glColor4ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_color4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ui) mogl_glunsupported("glColor4ui");
	glColor4ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_color4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4uiv) mogl_glunsupported("glColor4uiv");
	glColor4uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_color4us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4us) mogl_glunsupported("glColor4us");
	glColor4us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]),
		(GLushort)mxGetScalar(prhs[3]));

}

void gl_color4usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4usv) mogl_glunsupported("glColor4usv");
	glColor4usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_colormask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorMask) mogl_glunsupported("glColorMask");
	glColorMask((GLboolean)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_colormaterial( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorMaterial) mogl_glunsupported("glColorMaterial");
	glColorMaterial((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_colorsubtable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorSubTable) mogl_glunsupported("glColorSubTable");
	glColorSubTable((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_colortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTable) mogl_glunsupported("glColorTable");
	glColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_colortableparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableParameterfv) mogl_glunsupported("glColorTableParameterfv");
	glColorTableParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_colortableparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableParameteriv) mogl_glunsupported("glColorTableParameteriv");
	glColorTableParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_convolutionfilter1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionFilter1D) mogl_glunsupported("glConvolutionFilter1D");
	glConvolutionFilter1D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_convolutionfilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionFilter2D) mogl_glunsupported("glConvolutionFilter2D");
	glConvolutionFilter2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_convolutionparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameterf) mogl_glunsupported("glConvolutionParameterf");
	glConvolutionParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_convolutionparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameterfv) mogl_glunsupported("glConvolutionParameterfv");
	glConvolutionParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_convolutionparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameteri) mogl_glunsupported("glConvolutionParameteri");
	glConvolutionParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_convolutionparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameteriv) mogl_glunsupported("glConvolutionParameteriv");
	glConvolutionParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_copycolorsubtable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyColorSubTable) mogl_glunsupported("glCopyColorSubTable");
	glCopyColorSubTable((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copycolortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyColorTable) mogl_glunsupported("glCopyColorTable");
	glCopyColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copyconvolutionfilter1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyConvolutionFilter1D) mogl_glunsupported("glCopyConvolutionFilter1D");
	glCopyConvolutionFilter1D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copyconvolutionfilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyConvolutionFilter2D) mogl_glunsupported("glCopyConvolutionFilter2D");
	glCopyConvolutionFilter2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copypixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyPixels) mogl_glunsupported("glCopyPixels");
	glCopyPixels((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_copyteximage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexImage1D) mogl_glunsupported("glCopyTexImage1D");
	glCopyTexImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_copyteximage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexImage2D) mogl_glunsupported("glCopyTexImage2D");
	glCopyTexImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]));

}

void gl_copytexsubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage1D) mogl_glunsupported("glCopyTexSubImage1D");
	glCopyTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copytexsubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage2D) mogl_glunsupported("glCopyTexSubImage2D");
	glCopyTexSubImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]));

}

void gl_copytexsubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage3D) mogl_glunsupported("glCopyTexSubImage3D");
	glCopyTexSubImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]));

}

void gl_cullface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCullFace) mogl_glunsupported("glCullFace");
	glCullFace((GLenum)mxGetScalar(prhs[0]));

}

void gl_deletelists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteLists) mogl_glunsupported("glDeleteLists");
	glDeleteLists((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_deletetextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteTextures) mogl_glunsupported("glDeleteTextures");
	glDeleteTextures((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_depthfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthFunc) mogl_glunsupported("glDepthFunc");
	glDepthFunc((GLenum)mxGetScalar(prhs[0]));

}

void gl_depthmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthMask) mogl_glunsupported("glDepthMask");
	glDepthMask((GLboolean)mxGetScalar(prhs[0]));

}

void gl_depthrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRange) mogl_glunsupported("glDepthRange");
	glDepthRange((GLclampd)mxGetScalar(prhs[0]),
		(GLclampd)mxGetScalar(prhs[1]));

}

void gl_disable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisable) mogl_glunsupported("glDisable");
	glDisable((GLenum)mxGetScalar(prhs[0]));

}

void gl_disableclientstate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableClientState) mogl_glunsupported("glDisableClientState");
	glDisableClientState((GLenum)mxGetScalar(prhs[0]));

}

void gl_drawarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArrays) mogl_glunsupported("glDrawArrays");
	glDrawArrays((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_drawbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawBuffer) mogl_glunsupported("glDrawBuffer");
	glDrawBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_drawpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawPixels) mogl_glunsupported("glDrawPixels");
	glDrawPixels((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const GLvoid*)mxGetData(prhs[4]));

}

void gl_edgeflag( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEdgeFlag) mogl_glunsupported("glEdgeFlag");
	glEdgeFlag((GLboolean)mxGetScalar(prhs[0]));

}

void gl_edgeflagpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEdgeFlagPointer) mogl_glunsupported("glEdgeFlagPointer");
	glEdgeFlagPointer((GLsizei)mxGetScalar(prhs[0]),
		(const GLvoid*)mxGetData(prhs[1]));

}

void gl_edgeflagv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEdgeFlagv) mogl_glunsupported("glEdgeFlagv");
	glEdgeFlagv((const GLboolean*)mxGetData(prhs[0]));

}

void gl_enable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnable) mogl_glunsupported("glEnable");
	glEnable((GLenum)mxGetScalar(prhs[0]));

}

void gl_enableclientstate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableClientState) mogl_glunsupported("glEnableClientState");
	glEnableClientState((GLenum)mxGetScalar(prhs[0]));

}

void gl_end( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnd) mogl_glunsupported("glEnd");
	glEnd();

}

void gl_endlist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndList) mogl_glunsupported("glEndList");
	glEndList();

}

void gl_evalcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord1d) mogl_glunsupported("glEvalCoord1d");
	glEvalCoord1d((GLdouble)mxGetScalar(prhs[0]));

}

void gl_evalcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord1dv) mogl_glunsupported("glEvalCoord1dv");
	glEvalCoord1dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_evalcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord1f) mogl_glunsupported("glEvalCoord1f");
	glEvalCoord1f((GLfloat)mxGetScalar(prhs[0]));

}

void gl_evalcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord1fv) mogl_glunsupported("glEvalCoord1fv");
	glEvalCoord1fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_evalcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord2d) mogl_glunsupported("glEvalCoord2d");
	glEvalCoord2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_evalcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord2dv) mogl_glunsupported("glEvalCoord2dv");
	glEvalCoord2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_evalcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord2f) mogl_glunsupported("glEvalCoord2f");
	glEvalCoord2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_evalcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalCoord2fv) mogl_glunsupported("glEvalCoord2fv");
	glEvalCoord2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_evalmesh1( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalMesh1) mogl_glunsupported("glEvalMesh1");
	glEvalMesh1((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_evalmesh2( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalMesh2) mogl_glunsupported("glEvalMesh2");
	glEvalMesh2((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_evalpoint1( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalPoint1) mogl_glunsupported("glEvalPoint1");
	glEvalPoint1((GLint)mxGetScalar(prhs[0]));

}

void gl_evalpoint2( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalPoint2) mogl_glunsupported("glEvalPoint2");
	glEvalPoint2((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_finish( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinish) mogl_glunsupported("glFinish");
	glFinish();

}

void gl_flush( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlush) mogl_glunsupported("glFlush");
	glFlush();

}

void gl_fogf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogf) mogl_glunsupported("glFogf");
	glFogf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_fogfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogfv) mogl_glunsupported("glFogfv");
	glFogfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_fogi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogi) mogl_glunsupported("glFogi");
	glFogi((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_fogiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogiv) mogl_glunsupported("glFogiv");
	glFogiv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_frontface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFrontFace) mogl_glunsupported("glFrontFace");
	glFrontFace((GLenum)mxGetScalar(prhs[0]));

}

void gl_frustum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFrustum) mogl_glunsupported("glFrustum");
	glFrustum((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_genlists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenLists) mogl_glunsupported("glGenLists");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenLists((GLsizei)mxGetScalar(prhs[0]));

}

void gl_gentextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenTextures) mogl_glunsupported("glGenTextures");
	glGenTextures((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getbooleanv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBooleanv) mogl_glunsupported("glGetBooleanv");
	glGetBooleanv((GLenum)mxGetScalar(prhs[0]),
		(GLboolean*)mxGetData(prhs[1]));

}

void gl_getclipplane( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetClipPlane) mogl_glunsupported("glGetClipPlane");
	glGetClipPlane((GLenum)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_getdoublev( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDoublev) mogl_glunsupported("glGetDoublev");
	glGetDoublev((GLenum)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_geterror( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetError) mogl_glunsupported("glGetError");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetError();

}

void gl_getfloatv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFloatv) mogl_glunsupported("glGetFloatv");
	glGetFloatv((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_getintegerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetIntegerv) mogl_glunsupported("glGetIntegerv");
	glGetIntegerv((GLenum)mxGetScalar(prhs[0]),
		(GLint*)mxGetData(prhs[1]));

}

void gl_getlightfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetLightfv) mogl_glunsupported("glGetLightfv");
	glGetLightfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getlightiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetLightiv) mogl_glunsupported("glGetLightiv");
	glGetLightiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getmapdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapdv) mogl_glunsupported("glGetMapdv");
	glGetMapdv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapfv) mogl_glunsupported("glGetMapfv");
	glGetMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getmapiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapiv) mogl_glunsupported("glGetMapiv");
	glGetMapiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getmaterialfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMaterialfv) mogl_glunsupported("glGetMaterialfv");
	glGetMaterialfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getmaterialiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMaterialiv) mogl_glunsupported("glGetMaterialiv");
	glGetMaterialiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getminmaxparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmaxParameterfv) mogl_glunsupported("glGetMinmaxParameterfv");
	glGetMinmaxParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getminmaxparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmaxParameteriv) mogl_glunsupported("glGetMinmaxParameteriv");
	glGetMinmaxParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpixelmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPixelMapfv) mogl_glunsupported("glGetPixelMapfv");
	glGetPixelMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_getpixelmapuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPixelMapuiv) mogl_glunsupported("glGetPixelMapuiv");
	glGetPixelMapuiv((GLenum)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getpixelmapusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPixelMapusv) mogl_glunsupported("glGetPixelMapusv");
	glGetPixelMapusv((GLenum)mxGetScalar(prhs[0]),
		(GLushort*)mxGetData(prhs[1]));

}

void gl_getpolygonstipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPolygonStipple) mogl_glunsupported("glGetPolygonStipple");
	glGetPolygonStipple((GLubyte*)mxGetData(prhs[0]));

}

void gl_gettexenvfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexEnvfv) mogl_glunsupported("glGetTexEnvfv");
	glGetTexEnvfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexenviv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexEnviv) mogl_glunsupported("glGetTexEnviv");
	glGetTexEnviv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettexgendv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexGendv) mogl_glunsupported("glGetTexGendv");
	glGetTexGendv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_gettexgenfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexGenfv) mogl_glunsupported("glGetTexGenfv");
	glGetTexGenfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexgeniv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexGeniv) mogl_glunsupported("glGetTexGeniv");
	glGetTexGeniv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexImage) mogl_glunsupported("glGetTexImage");
	glGetTexImage((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

}

void gl_gettexlevelparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexLevelParameterfv) mogl_glunsupported("glGetTexLevelParameterfv");
	glGetTexLevelParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_gettexlevelparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexLevelParameteriv) mogl_glunsupported("glGetTexLevelParameteriv");
	glGetTexLevelParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettexparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameterfv) mogl_glunsupported("glGetTexParameterfv");
	glGetTexParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameteriv) mogl_glunsupported("glGetTexParameteriv");
	glGetTexParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_hint( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glHint) mogl_glunsupported("glHint");
	glHint((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_histogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glHistogram) mogl_glunsupported("glHistogram");
	glHistogram((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_indexmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexMask) mogl_glunsupported("glIndexMask");
	glIndexMask((GLuint)mxGetScalar(prhs[0]));

}

void gl_indexpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexPointer) mogl_glunsupported("glIndexPointer");
	glIndexPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_indexd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexd) mogl_glunsupported("glIndexd");
	glIndexd((GLdouble)mxGetScalar(prhs[0]));

}

void gl_indexdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexdv) mogl_glunsupported("glIndexdv");
	glIndexdv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_indexf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexf) mogl_glunsupported("glIndexf");
	glIndexf((GLfloat)mxGetScalar(prhs[0]));

}

void gl_indexfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexfv) mogl_glunsupported("glIndexfv");
	glIndexfv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_indexi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexi) mogl_glunsupported("glIndexi");
	glIndexi((GLint)mxGetScalar(prhs[0]));

}

void gl_indexiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexiv) mogl_glunsupported("glIndexiv");
	glIndexiv((const GLint*)mxGetData(prhs[0]));

}

void gl_indexs( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexs) mogl_glunsupported("glIndexs");
	glIndexs((GLshort)mxGetScalar(prhs[0]));

}

void gl_indexsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexsv) mogl_glunsupported("glIndexsv");
	glIndexsv((const GLshort*)mxGetData(prhs[0]));

}

void gl_indexub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexub) mogl_glunsupported("glIndexub");
	glIndexub((GLubyte)mxGetScalar(prhs[0]));

}

void gl_indexubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexubv) mogl_glunsupported("glIndexubv");
	glIndexubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_initnames( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInitNames) mogl_glunsupported("glInitNames");
	glInitNames();

}

void gl_interleavedarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInterleavedArrays) mogl_glunsupported("glInterleavedArrays");
	glInterleavedArrays((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_isenabled( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsEnabled) mogl_glunsupported("glIsEnabled");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsEnabled((GLenum)mxGetScalar(prhs[0]));

}

void gl_islist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsList) mogl_glunsupported("glIsList");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsList((GLuint)mxGetScalar(prhs[0]));

}

void gl_istexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTexture) mogl_glunsupported("glIsTexture");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTexture((GLuint)mxGetScalar(prhs[0]));

}

void gl_lightmodelf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightModelf) mogl_glunsupported("glLightModelf");
	glLightModelf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_lightmodelfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightModelfv) mogl_glunsupported("glLightModelfv");
	glLightModelfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_lightmodeli( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightModeli) mogl_glunsupported("glLightModeli");
	glLightModeli((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_lightmodeliv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightModeliv) mogl_glunsupported("glLightModeliv");
	glLightModeliv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_lightf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightf) mogl_glunsupported("glLightf");
	glLightf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_lightfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightfv) mogl_glunsupported("glLightfv");
	glLightfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_lighti( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLighti) mogl_glunsupported("glLighti");
	glLighti((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_lightiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLightiv) mogl_glunsupported("glLightiv");
	glLightiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_linestipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLineStipple) mogl_glunsupported("glLineStipple");
	glLineStipple((GLint)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]));

}

void gl_linewidth( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLineWidth) mogl_glunsupported("glLineWidth");
	glLineWidth((GLfloat)mxGetScalar(prhs[0]));

}

void gl_listbase( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glListBase) mogl_glunsupported("glListBase");
	glListBase((GLuint)mxGetScalar(prhs[0]));

}

void gl_loadidentity( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadIdentity) mogl_glunsupported("glLoadIdentity");
	glLoadIdentity();

}

void gl_loadmatrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadMatrixd) mogl_glunsupported("glLoadMatrixd");
	glLoadMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_loadmatrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadMatrixf) mogl_glunsupported("glLoadMatrixf");
	glLoadMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_loadname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadName) mogl_glunsupported("glLoadName");
	glLoadName((GLuint)mxGetScalar(prhs[0]));

}

void gl_logicop( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLogicOp) mogl_glunsupported("glLogicOp");
	glLogicOp((GLenum)mxGetScalar(prhs[0]));

}

void gl_map1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMap1d) mogl_glunsupported("glMap1d");
	glMap1d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(const GLdouble*)mxGetData(prhs[5]));

}

void gl_map1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMap1f) mogl_glunsupported("glMap1f");
	glMap1f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(const GLfloat*)mxGetData(prhs[5]));

}

void gl_map2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMap2d) mogl_glunsupported("glMap2d");
	glMap2d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(const GLdouble*)mxGetData(prhs[9]));

}

void gl_map2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMap2f) mogl_glunsupported("glMap2f");
	glMap2f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(const GLfloat*)mxGetData(prhs[9]));

}

void gl_mapgrid1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapGrid1d) mogl_glunsupported("glMapGrid1d");
	glMapGrid1d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_mapgrid1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapGrid1f) mogl_glunsupported("glMapGrid1f");
	glMapGrid1f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_mapgrid2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapGrid2d) mogl_glunsupported("glMapGrid2d");
	glMapGrid2d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_mapgrid2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapGrid2f) mogl_glunsupported("glMapGrid2f");
	glMapGrid2f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_materialf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMaterialf) mogl_glunsupported("glMaterialf");
	glMaterialf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_materialfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMaterialfv) mogl_glunsupported("glMaterialfv");
	glMaterialfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_materiali( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMateriali) mogl_glunsupported("glMateriali");
	glMateriali((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_materialiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMaterialiv) mogl_glunsupported("glMaterialiv");
	glMaterialiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_matrixmode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixMode) mogl_glunsupported("glMatrixMode");
	glMatrixMode((GLenum)mxGetScalar(prhs[0]));

}

void gl_minmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMinmax) mogl_glunsupported("glMinmax");
	glMinmax((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]));

}

void gl_multmatrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultMatrixd) mogl_glunsupported("glMultMatrixd");
	glMultMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_multmatrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultMatrixf) mogl_glunsupported("glMultMatrixf");
	glMultMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_newlist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNewList) mogl_glunsupported("glNewList");
	glNewList((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_normal3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3b) mogl_glunsupported("glNormal3b");
	glNormal3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_normal3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3bv) mogl_glunsupported("glNormal3bv");
	glNormal3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_normal3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3d) mogl_glunsupported("glNormal3d");
	glNormal3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_normal3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3dv) mogl_glunsupported("glNormal3dv");
	glNormal3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_normal3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3f) mogl_glunsupported("glNormal3f");
	glNormal3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_normal3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3fv) mogl_glunsupported("glNormal3fv");
	glNormal3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_normal3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3i) mogl_glunsupported("glNormal3i");
	glNormal3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_normal3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3iv) mogl_glunsupported("glNormal3iv");
	glNormal3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_normal3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3s) mogl_glunsupported("glNormal3s");
	glNormal3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_normal3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3sv) mogl_glunsupported("glNormal3sv");
	glNormal3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_ortho( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glOrtho) mogl_glunsupported("glOrtho");
	glOrtho((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_passthrough( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPassThrough) mogl_glunsupported("glPassThrough");
	glPassThrough((GLfloat)mxGetScalar(prhs[0]));

}

void gl_pixelmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelMapfv) mogl_glunsupported("glPixelMapfv");
	glPixelMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_pixelmapuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelMapuiv) mogl_glunsupported("glPixelMapuiv");
	glPixelMapuiv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_pixelmapusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelMapusv) mogl_glunsupported("glPixelMapusv");
	glPixelMapusv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLushort*)mxGetData(prhs[2]));

}

void gl_pixelstoref( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelStoref) mogl_glunsupported("glPixelStoref");
	glPixelStoref((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pixelstorei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelStorei) mogl_glunsupported("glPixelStorei");
	glPixelStorei((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pixeltransferf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransferf) mogl_glunsupported("glPixelTransferf");
	glPixelTransferf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pixeltransferi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransferi) mogl_glunsupported("glPixelTransferi");
	glPixelTransferi((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pixelzoom( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelZoom) mogl_glunsupported("glPixelZoom");
	glPixelZoom((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointsize( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointSize) mogl_glunsupported("glPointSize");
	glPointSize((GLfloat)mxGetScalar(prhs[0]));

}

void gl_polygonmode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPolygonMode) mogl_glunsupported("glPolygonMode");
	glPolygonMode((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_polygonoffset( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPolygonOffset) mogl_glunsupported("glPolygonOffset");
	glPolygonOffset((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_polygonstipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPolygonStipple) mogl_glunsupported("glPolygonStipple");
	glPolygonStipple((const GLubyte*)mxGetData(prhs[0]));

}

void gl_popattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopAttrib) mogl_glunsupported("glPopAttrib");
	glPopAttrib();

}

void gl_popclientattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopClientAttrib) mogl_glunsupported("glPopClientAttrib");
	glPopClientAttrib();

}

void gl_popmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopMatrix) mogl_glunsupported("glPopMatrix");
	glPopMatrix();

}

void gl_popname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopName) mogl_glunsupported("glPopName");
	glPopName();

}

void gl_prioritizetextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPrioritizeTextures) mogl_glunsupported("glPrioritizeTextures");
	glPrioritizeTextures((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(const GLclampf*)mxGetData(prhs[2]));

}

void gl_pushattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushAttrib) mogl_glunsupported("glPushAttrib");
	glPushAttrib((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_pushclientattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushClientAttrib) mogl_glunsupported("glPushClientAttrib");
	glPushClientAttrib((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_pushmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushMatrix) mogl_glunsupported("glPushMatrix");
	glPushMatrix();

}

void gl_pushname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushName) mogl_glunsupported("glPushName");
	glPushName((GLuint)mxGetScalar(prhs[0]));

}

void gl_rasterpos2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2d) mogl_glunsupported("glRasterPos2d");
	glRasterPos2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_rasterpos2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2dv) mogl_glunsupported("glRasterPos2dv");
	glRasterPos2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2f) mogl_glunsupported("glRasterPos2f");
	glRasterPos2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_rasterpos2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2fv) mogl_glunsupported("glRasterPos2fv");
	glRasterPos2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2i) mogl_glunsupported("glRasterPos2i");
	glRasterPos2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_rasterpos2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2iv) mogl_glunsupported("glRasterPos2iv");
	glRasterPos2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2s) mogl_glunsupported("glRasterPos2s");
	glRasterPos2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_rasterpos2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos2sv) mogl_glunsupported("glRasterPos2sv");
	glRasterPos2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_rasterpos3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3d) mogl_glunsupported("glRasterPos3d");
	glRasterPos3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_rasterpos3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3dv) mogl_glunsupported("glRasterPos3dv");
	glRasterPos3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3f) mogl_glunsupported("glRasterPos3f");
	glRasterPos3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_rasterpos3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3fv) mogl_glunsupported("glRasterPos3fv");
	glRasterPos3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3i) mogl_glunsupported("glRasterPos3i");
	glRasterPos3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_rasterpos3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3iv) mogl_glunsupported("glRasterPos3iv");
	glRasterPos3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3s) mogl_glunsupported("glRasterPos3s");
	glRasterPos3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_rasterpos3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos3sv) mogl_glunsupported("glRasterPos3sv");
	glRasterPos3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_rasterpos4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4d) mogl_glunsupported("glRasterPos4d");
	glRasterPos4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rasterpos4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4dv) mogl_glunsupported("glRasterPos4dv");
	glRasterPos4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4f) mogl_glunsupported("glRasterPos4f");
	glRasterPos4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_rasterpos4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4fv) mogl_glunsupported("glRasterPos4fv");
	glRasterPos4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4i) mogl_glunsupported("glRasterPos4i");
	glRasterPos4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_rasterpos4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4iv) mogl_glunsupported("glRasterPos4iv");
	glRasterPos4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4s) mogl_glunsupported("glRasterPos4s");
	glRasterPos4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_rasterpos4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRasterPos4sv) mogl_glunsupported("glRasterPos4sv");
	glRasterPos4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_readbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReadBuffer) mogl_glunsupported("glReadBuffer");
	glReadBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_rectd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectd) mogl_glunsupported("glRectd");
	glRectd((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rectdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectdv) mogl_glunsupported("glRectdv");
	glRectdv((const GLdouble*)mxGetData(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_rectf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectf) mogl_glunsupported("glRectf");
	glRectf((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_rectfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectfv) mogl_glunsupported("glRectfv");
	glRectfv((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_recti( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRecti) mogl_glunsupported("glRecti");
	glRecti((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_rectiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectiv) mogl_glunsupported("glRectiv");
	glRectiv((const GLint*)mxGetData(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_rects( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRects) mogl_glunsupported("glRects");
	glRects((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_rectsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRectsv) mogl_glunsupported("glRectsv");
	glRectsv((const GLshort*)mxGetData(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_rendermode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderMode) mogl_glunsupported("glRenderMode");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glRenderMode((GLenum)mxGetScalar(prhs[0]));

}

void gl_resethistogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResetHistogram) mogl_glunsupported("glResetHistogram");
	glResetHistogram((GLenum)mxGetScalar(prhs[0]));

}

void gl_resetminmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResetMinmax) mogl_glunsupported("glResetMinmax");
	glResetMinmax((GLenum)mxGetScalar(prhs[0]));

}

void gl_rotated( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRotated) mogl_glunsupported("glRotated");
	glRotated((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rotatef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRotatef) mogl_glunsupported("glRotatef");
	glRotatef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_scaled( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScaled) mogl_glunsupported("glScaled");
	glScaled((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_scalef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScalef) mogl_glunsupported("glScalef");
	glScalef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_scissor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScissor) mogl_glunsupported("glScissor");
	glScissor((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_separablefilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSeparableFilter2D) mogl_glunsupported("glSeparableFilter2D");
	glSeparableFilter2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]),
		(const GLvoid*)mxGetData(prhs[7]));

}

void gl_shademodel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShadeModel) mogl_glunsupported("glShadeModel");
	glShadeModel((GLenum)mxGetScalar(prhs[0]));

}

void gl_stencilfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilFunc) mogl_glunsupported("glStencilFunc");
	glStencilFunc((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_stencilmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilMask) mogl_glunsupported("glStencilMask");
	glStencilMask((GLuint)mxGetScalar(prhs[0]));

}

void gl_stencilop( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilOp) mogl_glunsupported("glStencilOp");
	glStencilOp((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_texcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1d) mogl_glunsupported("glTexCoord1d");
	glTexCoord1d((GLdouble)mxGetScalar(prhs[0]));

}

void gl_texcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1dv) mogl_glunsupported("glTexCoord1dv");
	glTexCoord1dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1f) mogl_glunsupported("glTexCoord1f");
	glTexCoord1f((GLfloat)mxGetScalar(prhs[0]));

}

void gl_texcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1fv) mogl_glunsupported("glTexCoord1fv");
	glTexCoord1fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1i) mogl_glunsupported("glTexCoord1i");
	glTexCoord1i((GLint)mxGetScalar(prhs[0]));

}

void gl_texcoord1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1iv) mogl_glunsupported("glTexCoord1iv");
	glTexCoord1iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1s) mogl_glunsupported("glTexCoord1s");
	glTexCoord1s((GLshort)mxGetScalar(prhs[0]));

}

void gl_texcoord1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord1sv) mogl_glunsupported("glTexCoord1sv");
	glTexCoord1sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2d) mogl_glunsupported("glTexCoord2d");
	glTexCoord2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_texcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2dv) mogl_glunsupported("glTexCoord2dv");
	glTexCoord2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2f) mogl_glunsupported("glTexCoord2f");
	glTexCoord2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_texcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fv) mogl_glunsupported("glTexCoord2fv");
	glTexCoord2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2i) mogl_glunsupported("glTexCoord2i");
	glTexCoord2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_texcoord2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2iv) mogl_glunsupported("glTexCoord2iv");
	glTexCoord2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2s) mogl_glunsupported("glTexCoord2s");
	glTexCoord2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_texcoord2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2sv) mogl_glunsupported("glTexCoord2sv");
	glTexCoord2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3d) mogl_glunsupported("glTexCoord3d");
	glTexCoord3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_texcoord3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3dv) mogl_glunsupported("glTexCoord3dv");
	glTexCoord3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3f) mogl_glunsupported("glTexCoord3f");
	glTexCoord3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texcoord3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3fv) mogl_glunsupported("glTexCoord3fv");
	glTexCoord3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3i) mogl_glunsupported("glTexCoord3i");
	glTexCoord3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texcoord3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3iv) mogl_glunsupported("glTexCoord3iv");
	glTexCoord3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3s) mogl_glunsupported("glTexCoord3s");
	glTexCoord3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_texcoord3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord3sv) mogl_glunsupported("glTexCoord3sv");
	glTexCoord3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4d) mogl_glunsupported("glTexCoord4d");
	glTexCoord4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_texcoord4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4dv) mogl_glunsupported("glTexCoord4dv");
	glTexCoord4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4f) mogl_glunsupported("glTexCoord4f");
	glTexCoord4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_texcoord4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4fv) mogl_glunsupported("glTexCoord4fv");
	glTexCoord4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4i) mogl_glunsupported("glTexCoord4i");
	glTexCoord4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_texcoord4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4iv) mogl_glunsupported("glTexCoord4iv");
	glTexCoord4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4s) mogl_glunsupported("glTexCoord4s");
	glTexCoord4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_texcoord4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4sv) mogl_glunsupported("glTexCoord4sv");
	glTexCoord4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texenvf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexEnvf) mogl_glunsupported("glTexEnvf");
	glTexEnvf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texenvfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexEnvfv) mogl_glunsupported("glTexEnvfv");
	glTexEnvfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texenvi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexEnvi) mogl_glunsupported("glTexEnvi");
	glTexEnvi((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texenviv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexEnviv) mogl_glunsupported("glTexEnviv");
	glTexEnviv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texgend( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGend) mogl_glunsupported("glTexGend");
	glTexGend((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_texgendv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGendv) mogl_glunsupported("glTexGendv");
	glTexGendv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_texgenf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGenf) mogl_glunsupported("glTexGenf");
	glTexGenf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texgenfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGenfv) mogl_glunsupported("glTexGenfv");
	glTexGenfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texgeni( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGeni) mogl_glunsupported("glTexGeni");
	glTexGeni((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texgeniv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexGeniv) mogl_glunsupported("glTexGeniv");
	glTexGeniv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_teximage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage1D) mogl_glunsupported("glTexImage1D");
	glTexImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const GLvoid*)mxGetData(prhs[7]));

}

void gl_teximage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage3D) mogl_glunsupported("glTexImage3D");
	glTexImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const GLvoid*)mxGetData(prhs[9]));

}

void gl_texparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterf) mogl_glunsupported("glTexParameterf");
	glTexParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterfv) mogl_glunsupported("glTexParameterfv");
	glTexParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameteri) mogl_glunsupported("glTexParameteri");
	glTexParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameteriv) mogl_glunsupported("glTexParameteriv");
	glTexParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texsubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage1D) mogl_glunsupported("glTexSubImage1D");
	glTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_texsubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage2D) mogl_glunsupported("glTexSubImage2D");
	glTexSubImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const GLvoid*)mxGetData(prhs[8]));

}

void gl_texsubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage3D) mogl_glunsupported("glTexSubImage3D");
	glTexSubImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const GLvoid*)mxGetData(prhs[10]));

}

void gl_translated( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTranslated) mogl_glunsupported("glTranslated");
	glTranslated((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_translatef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTranslatef) mogl_glunsupported("glTranslatef");
	glTranslatef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertex2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2d) mogl_glunsupported("glVertex2d");
	glVertex2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertex2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2dv) mogl_glunsupported("glVertex2dv");
	glVertex2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2f) mogl_glunsupported("glVertex2f");
	glVertex2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertex2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2fv) mogl_glunsupported("glVertex2fv");
	glVertex2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2i) mogl_glunsupported("glVertex2i");
	glVertex2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertex2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2iv) mogl_glunsupported("glVertex2iv");
	glVertex2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2s) mogl_glunsupported("glVertex2s");
	glVertex2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertex2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex2sv) mogl_glunsupported("glVertex2sv");
	glVertex2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_vertex3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3d) mogl_glunsupported("glVertex3d");
	glVertex3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertex3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3dv) mogl_glunsupported("glVertex3dv");
	glVertex3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3f) mogl_glunsupported("glVertex3f");
	glVertex3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertex3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3fv) mogl_glunsupported("glVertex3fv");
	glVertex3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3i) mogl_glunsupported("glVertex3i");
	glVertex3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_vertex3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3iv) mogl_glunsupported("glVertex3iv");
	glVertex3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3s) mogl_glunsupported("glVertex3s");
	glVertex3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertex3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex3sv) mogl_glunsupported("glVertex3sv");
	glVertex3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_vertex4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4d) mogl_glunsupported("glVertex4d");
	glVertex4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertex4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4dv) mogl_glunsupported("glVertex4dv");
	glVertex4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4f) mogl_glunsupported("glVertex4f");
	glVertex4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertex4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4fv) mogl_glunsupported("glVertex4fv");
	glVertex4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4i) mogl_glunsupported("glVertex4i");
	glVertex4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_vertex4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4iv) mogl_glunsupported("glVertex4iv");
	glVertex4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4s) mogl_glunsupported("glVertex4s");
	glVertex4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertex4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertex4sv) mogl_glunsupported("glVertex4sv");
	glVertex4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_viewport( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glViewport) mogl_glunsupported("glViewport");
	glViewport((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_samplecoverage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleCoverage) mogl_glunsupported("glSampleCoverage");
	glSampleCoverage((GLclampf)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_loadtransposematrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadTransposeMatrixf) mogl_glunsupported("glLoadTransposeMatrixf");
	glLoadTransposeMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_loadtransposematrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadTransposeMatrixd) mogl_glunsupported("glLoadTransposeMatrixd");
	glLoadTransposeMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_multtransposematrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultTransposeMatrixf) mogl_glunsupported("glMultTransposeMatrixf");
	glMultTransposeMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_multtransposematrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultTransposeMatrixd) mogl_glunsupported("glMultTransposeMatrixd");
	glMultTransposeMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_compressedteximage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage3D) mogl_glunsupported("glCompressedTexImage3D");
	glCompressedTexImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const GLvoid*)mxGetData(prhs[8]));

}

void gl_compressedteximage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage2D) mogl_glunsupported("glCompressedTexImage2D");
	glCompressedTexImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const GLvoid*)mxGetData(prhs[7]));

}

void gl_compressedteximage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage1D) mogl_glunsupported("glCompressedTexImage1D");
	glCompressedTexImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_compressedtexsubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage3D) mogl_glunsupported("glCompressedTexSubImage3D");
	glCompressedTexSubImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]),
		(const GLvoid*)mxGetData(prhs[10]));

}

void gl_compressedtexsubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage2D) mogl_glunsupported("glCompressedTexSubImage2D");
	glCompressedTexSubImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const GLvoid*)mxGetData(prhs[8]));

}

void gl_compressedtexsubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage1D) mogl_glunsupported("glCompressedTexSubImage1D");
	glCompressedTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_getcompressedteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedTexImage) mogl_glunsupported("glGetCompressedTexImage");
	glGetCompressedTexImage((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLvoid*)mxGetData(prhs[2]));

}

void gl_activetexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveTexture) mogl_glunsupported("glActiveTexture");
	glActiveTexture((GLenum)mxGetScalar(prhs[0]));

}

void gl_clientactivetexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClientActiveTexture) mogl_glunsupported("glClientActiveTexture");
	glClientActiveTexture((GLenum)mxGetScalar(prhs[0]));

}

void gl_multitexcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1d) mogl_glunsupported("glMultiTexCoord1d");
	glMultiTexCoord1d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1dv) mogl_glunsupported("glMultiTexCoord1dv");
	glMultiTexCoord1dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1f) mogl_glunsupported("glMultiTexCoord1f");
	glMultiTexCoord1f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1fv) mogl_glunsupported("glMultiTexCoord1fv");
	glMultiTexCoord1fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1i) mogl_glunsupported("glMultiTexCoord1i");
	glMultiTexCoord1i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1iv) mogl_glunsupported("glMultiTexCoord1iv");
	glMultiTexCoord1iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1s) mogl_glunsupported("glMultiTexCoord1s");
	glMultiTexCoord1s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1sv) mogl_glunsupported("glMultiTexCoord1sv");
	glMultiTexCoord1sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2d) mogl_glunsupported("glMultiTexCoord2d");
	glMultiTexCoord2d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2dv) mogl_glunsupported("glMultiTexCoord2dv");
	glMultiTexCoord2dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2f) mogl_glunsupported("glMultiTexCoord2f");
	glMultiTexCoord2f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2fv) mogl_glunsupported("glMultiTexCoord2fv");
	glMultiTexCoord2fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2i) mogl_glunsupported("glMultiTexCoord2i");
	glMultiTexCoord2i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2iv) mogl_glunsupported("glMultiTexCoord2iv");
	glMultiTexCoord2iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2s) mogl_glunsupported("glMultiTexCoord2s");
	glMultiTexCoord2s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2sv) mogl_glunsupported("glMultiTexCoord2sv");
	glMultiTexCoord2sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3d) mogl_glunsupported("glMultiTexCoord3d");
	glMultiTexCoord3d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3dv) mogl_glunsupported("glMultiTexCoord3dv");
	glMultiTexCoord3dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3f) mogl_glunsupported("glMultiTexCoord3f");
	glMultiTexCoord3f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3fv) mogl_glunsupported("glMultiTexCoord3fv");
	glMultiTexCoord3fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3i) mogl_glunsupported("glMultiTexCoord3i");
	glMultiTexCoord3i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3iv) mogl_glunsupported("glMultiTexCoord3iv");
	glMultiTexCoord3iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3s) mogl_glunsupported("glMultiTexCoord3s");
	glMultiTexCoord3s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3sv) mogl_glunsupported("glMultiTexCoord3sv");
	glMultiTexCoord3sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4d) mogl_glunsupported("glMultiTexCoord4d");
	glMultiTexCoord4d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4dv) mogl_glunsupported("glMultiTexCoord4dv");
	glMultiTexCoord4dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4f) mogl_glunsupported("glMultiTexCoord4f");
	glMultiTexCoord4f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4fv) mogl_glunsupported("glMultiTexCoord4fv");
	glMultiTexCoord4fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4i) mogl_glunsupported("glMultiTexCoord4i");
	glMultiTexCoord4i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4iv) mogl_glunsupported("glMultiTexCoord4iv");
	glMultiTexCoord4iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4s) mogl_glunsupported("glMultiTexCoord4s");
	glMultiTexCoord4s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4sv) mogl_glunsupported("glMultiTexCoord4sv");
	glMultiTexCoord4sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_fogcoordf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordf) mogl_glunsupported("glFogCoordf");
	glFogCoordf((GLfloat)mxGetScalar(prhs[0]));

}

void gl_fogcoordfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordfv) mogl_glunsupported("glFogCoordfv");
	glFogCoordfv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_fogcoordd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordd) mogl_glunsupported("glFogCoordd");
	glFogCoordd((GLdouble)mxGetScalar(prhs[0]));

}

void gl_fogcoorddv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoorddv) mogl_glunsupported("glFogCoorddv");
	glFogCoorddv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_fogcoordpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordPointer) mogl_glunsupported("glFogCoordPointer");
	glFogCoordPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_secondarycolor3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3b) mogl_glunsupported("glSecondaryColor3b");
	glSecondaryColor3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3bv) mogl_glunsupported("glSecondaryColor3bv");
	glSecondaryColor3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3d) mogl_glunsupported("glSecondaryColor3d");
	glSecondaryColor3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3dv) mogl_glunsupported("glSecondaryColor3dv");
	glSecondaryColor3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_secondarycolor3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3f) mogl_glunsupported("glSecondaryColor3f");
	glSecondaryColor3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3fv) mogl_glunsupported("glSecondaryColor3fv");
	glSecondaryColor3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_secondarycolor3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3i) mogl_glunsupported("glSecondaryColor3i");
	glSecondaryColor3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3iv) mogl_glunsupported("glSecondaryColor3iv");
	glSecondaryColor3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3s) mogl_glunsupported("glSecondaryColor3s");
	glSecondaryColor3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3sv) mogl_glunsupported("glSecondaryColor3sv");
	glSecondaryColor3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_secondarycolor3ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ub) mogl_glunsupported("glSecondaryColor3ub");
	glSecondaryColor3ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ubv) mogl_glunsupported("glSecondaryColor3ubv");
	glSecondaryColor3ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ui) mogl_glunsupported("glSecondaryColor3ui");
	glSecondaryColor3ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3uiv) mogl_glunsupported("glSecondaryColor3uiv");
	glSecondaryColor3uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3us) mogl_glunsupported("glSecondaryColor3us");
	glSecondaryColor3us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3usv) mogl_glunsupported("glSecondaryColor3usv");
	glSecondaryColor3usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_secondarycolorpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColorPointer) mogl_glunsupported("glSecondaryColorPointer");
	glSecondaryColorPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_pointparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterf) mogl_glunsupported("glPointParameterf");
	glPointParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterfv) mogl_glunsupported("glPointParameterfv");
	glPointParameterfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_pointparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameteri) mogl_glunsupported("glPointParameteri");
	glPointParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pointparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameteriv) mogl_glunsupported("glPointParameteriv");
	glPointParameteriv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_blendfuncseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncSeparate) mogl_glunsupported("glBlendFuncSeparate");
	glBlendFuncSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_multidrawarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArrays) mogl_glunsupported("glMultiDrawArrays");
	glMultiDrawArrays((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(const GLsizei*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_windowpos2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2d) mogl_glunsupported("glWindowPos2d");
	glWindowPos2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_windowpos2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2dv) mogl_glunsupported("glWindowPos2dv");
	glWindowPos2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2f) mogl_glunsupported("glWindowPos2f");
	glWindowPos2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_windowpos2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2fv) mogl_glunsupported("glWindowPos2fv");
	glWindowPos2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2i) mogl_glunsupported("glWindowPos2i");
	glWindowPos2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_windowpos2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2iv) mogl_glunsupported("glWindowPos2iv");
	glWindowPos2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2s) mogl_glunsupported("glWindowPos2s");
	glWindowPos2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_windowpos2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2sv) mogl_glunsupported("glWindowPos2sv");
	glWindowPos2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_windowpos3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3d) mogl_glunsupported("glWindowPos3d");
	glWindowPos3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_windowpos3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3dv) mogl_glunsupported("glWindowPos3dv");
	glWindowPos3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3f) mogl_glunsupported("glWindowPos3f");
	glWindowPos3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_windowpos3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3fv) mogl_glunsupported("glWindowPos3fv");
	glWindowPos3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3i) mogl_glunsupported("glWindowPos3i");
	glWindowPos3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_windowpos3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3iv) mogl_glunsupported("glWindowPos3iv");
	glWindowPos3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3s) mogl_glunsupported("glWindowPos3s");
	glWindowPos3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_windowpos3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3sv) mogl_glunsupported("glWindowPos3sv");
	glWindowPos3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_genqueries( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenQueries) mogl_glunsupported("glGenQueries");
	glGenQueries((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deletequeries( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteQueries) mogl_glunsupported("glDeleteQueries");
	glDeleteQueries((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_isquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsQuery) mogl_glunsupported("glIsQuery");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsQuery((GLuint)mxGetScalar(prhs[0]));

}

void gl_beginquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginQuery) mogl_glunsupported("glBeginQuery");
	glBeginQuery((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_endquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndQuery) mogl_glunsupported("glEndQuery");
	glEndQuery((GLenum)mxGetScalar(prhs[0]));

}

void gl_getqueryiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryiv) mogl_glunsupported("glGetQueryiv");
	glGetQueryiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectiv) mogl_glunsupported("glGetQueryObjectiv");
	glGetQueryObjectiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectuiv) mogl_glunsupported("glGetQueryObjectuiv");
	glGetQueryObjectuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_bindbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBuffer) mogl_glunsupported("glBindBuffer");
	glBindBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteBuffers) mogl_glunsupported("glDeleteBuffers");
	glDeleteBuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenBuffers) mogl_glunsupported("glGenBuffers");
	glGenBuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsBuffer) mogl_glunsupported("glIsBuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsBuffer((GLuint)mxGetScalar(prhs[0]));

}

void gl_buffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferSubData) mogl_glunsupported("glBufferSubData");
	glBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_getbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferSubData) mogl_glunsupported("glGetBufferSubData");
	glGetBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_mapbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapBuffer) mogl_glunsupported("glMapBuffer");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) glMapBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1])));

}

void gl_unmapbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapBuffer) mogl_glunsupported("glUnmapBuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glUnmapBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_getbufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferParameteriv) mogl_glunsupported("glGetBufferParameteriv");
	glGetBufferParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_drawbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawBuffers) mogl_glunsupported("glDrawBuffers");
	glDrawBuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLenum*)mxGetData(prhs[1]));

}

void gl_vertexattrib1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1d) mogl_glunsupported("glVertexAttrib1d");
	glVertexAttrib1d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dv) mogl_glunsupported("glVertexAttrib1dv");
	glVertexAttrib1dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1f) mogl_glunsupported("glVertexAttrib1f");
	glVertexAttrib1f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fv) mogl_glunsupported("glVertexAttrib1fv");
	glVertexAttrib1fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1s) mogl_glunsupported("glVertexAttrib1s");
	glVertexAttrib1s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1sv) mogl_glunsupported("glVertexAttrib1sv");
	glVertexAttrib1sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2d) mogl_glunsupported("glVertexAttrib2d");
	glVertexAttrib2d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dv) mogl_glunsupported("glVertexAttrib2dv");
	glVertexAttrib2dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2f) mogl_glunsupported("glVertexAttrib2f");
	glVertexAttrib2f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fv) mogl_glunsupported("glVertexAttrib2fv");
	glVertexAttrib2fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2s) mogl_glunsupported("glVertexAttrib2s");
	glVertexAttrib2s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2sv) mogl_glunsupported("glVertexAttrib2sv");
	glVertexAttrib2sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3d) mogl_glunsupported("glVertexAttrib3d");
	glVertexAttrib3d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dv) mogl_glunsupported("glVertexAttrib3dv");
	glVertexAttrib3dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3f) mogl_glunsupported("glVertexAttrib3f");
	glVertexAttrib3f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fv) mogl_glunsupported("glVertexAttrib3fv");
	glVertexAttrib3fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3s) mogl_glunsupported("glVertexAttrib3s");
	glVertexAttrib3s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3sv) mogl_glunsupported("glVertexAttrib3sv");
	glVertexAttrib3sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nbv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nbv) mogl_glunsupported("glVertexAttrib4Nbv");
	glVertexAttrib4Nbv((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4niv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Niv) mogl_glunsupported("glVertexAttrib4Niv");
	glVertexAttrib4Niv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nsv) mogl_glunsupported("glVertexAttrib4Nsv");
	glVertexAttrib4Nsv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nub) mogl_glunsupported("glVertexAttrib4Nub");
	glVertexAttrib4Nub((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4nubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nubv) mogl_glunsupported("glVertexAttrib4Nubv");
	glVertexAttrib4Nubv((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nuiv) mogl_glunsupported("glVertexAttrib4Nuiv");
	glVertexAttrib4Nuiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4Nusv) mogl_glunsupported("glVertexAttrib4Nusv");
	glVertexAttrib4Nusv((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4bv) mogl_glunsupported("glVertexAttrib4bv");
	glVertexAttrib4bv((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4d) mogl_glunsupported("glVertexAttrib4d");
	glVertexAttrib4d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dv) mogl_glunsupported("glVertexAttrib4dv");
	glVertexAttrib4dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4f) mogl_glunsupported("glVertexAttrib4f");
	glVertexAttrib4f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fv) mogl_glunsupported("glVertexAttrib4fv");
	glVertexAttrib4fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4iv) mogl_glunsupported("glVertexAttrib4iv");
	glVertexAttrib4iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4s) mogl_glunsupported("glVertexAttrib4s");
	glVertexAttrib4s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4sv) mogl_glunsupported("glVertexAttrib4sv");
	glVertexAttrib4sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubv) mogl_glunsupported("glVertexAttrib4ubv");
	glVertexAttrib4ubv((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4uiv) mogl_glunsupported("glVertexAttrib4uiv");
	glVertexAttrib4uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4usv) mogl_glunsupported("glVertexAttrib4usv");
	glVertexAttrib4usv((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_enablevertexattribarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexAttribArray) mogl_glunsupported("glEnableVertexAttribArray");
	glEnableVertexAttribArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_disablevertexattribarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexAttribArray) mogl_glunsupported("glDisableVertexAttribArray");
	glDisableVertexAttribArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_getvertexattribdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribdv) mogl_glunsupported("glGetVertexAttribdv");
	glGetVertexAttribdv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getvertexattribfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribfv) mogl_glunsupported("glGetVertexAttribfv");
	glGetVertexAttribfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvertexattribiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribiv) mogl_glunsupported("glGetVertexAttribiv");
	glGetVertexAttribiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_deleteshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteShader) mogl_glunsupported("glDeleteShader");
	glDeleteShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_detachshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDetachShader) mogl_glunsupported("glDetachShader");
	glDetachShader((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_createshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateShader) mogl_glunsupported("glCreateShader");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateShader((GLenum)mxGetScalar(prhs[0]));

}

void gl_compileshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompileShader) mogl_glunsupported("glCompileShader");
	glCompileShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_createprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateProgram) mogl_glunsupported("glCreateProgram");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateProgram();

}

void gl_attachshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAttachShader) mogl_glunsupported("glAttachShader");
	glAttachShader((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_linkprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLinkProgram) mogl_glunsupported("glLinkProgram");
	glLinkProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_useprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUseProgram) mogl_glunsupported("glUseProgram");
	glUseProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_deleteprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteProgram) mogl_glunsupported("glDeleteProgram");
	glDeleteProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_validateprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glValidateProgram) mogl_glunsupported("glValidateProgram");
	glValidateProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_uniform1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1f) mogl_glunsupported("glUniform1f");
	glUniform1f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_uniform2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2f) mogl_glunsupported("glUniform2f");
	glUniform2f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_uniform3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3f) mogl_glunsupported("glUniform3f");
	glUniform3f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_uniform4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4f) mogl_glunsupported("glUniform4f");
	glUniform4f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_uniform1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1i) mogl_glunsupported("glUniform1i");
	glUniform1i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_uniform2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2i) mogl_glunsupported("glUniform2i");
	glUniform2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_uniform3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3i) mogl_glunsupported("glUniform3i");
	glUniform3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_uniform4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4i) mogl_glunsupported("glUniform4i");
	glUniform4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_uniform1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1fv) mogl_glunsupported("glUniform1fv");
	glUniform1fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2fv) mogl_glunsupported("glUniform2fv");
	glUniform2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3fv) mogl_glunsupported("glUniform3fv");
	glUniform3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4fv) mogl_glunsupported("glUniform4fv");
	glUniform4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1iv) mogl_glunsupported("glUniform1iv");
	glUniform1iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2iv) mogl_glunsupported("glUniform2iv");
	glUniform2iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3iv) mogl_glunsupported("glUniform3iv");
	glUniform3iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4iv) mogl_glunsupported("glUniform4iv");
	glUniform4iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniformmatrix2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2fv) mogl_glunsupported("glUniformMatrix2fv");
	glUniformMatrix2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3fv) mogl_glunsupported("glUniformMatrix3fv");
	glUniformMatrix3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4fv) mogl_glunsupported("glUniformMatrix4fv");
	glUniformMatrix4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_isshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsShader) mogl_glunsupported("glIsShader");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_isprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsProgram) mogl_glunsupported("glIsProgram");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_getshaderiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetShaderiv) mogl_glunsupported("glGetShaderiv");
	glGetShaderiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getprogramiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramiv) mogl_glunsupported("glGetProgramiv");
	glGetProgramiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getattachedshaders( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetAttachedShaders) mogl_glunsupported("glGetAttachedShaders");
	glGetAttachedShaders((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_getshaderinfolog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetShaderInfoLog) mogl_glunsupported("glGetShaderInfoLog");
	glGetShaderInfoLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_getprograminfolog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramInfoLog) mogl_glunsupported("glGetProgramInfoLog");
	glGetProgramInfoLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_getuniformlocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformLocation) mogl_glunsupported("glGetUniformLocation");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformLocation((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getactiveuniform( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniform) mogl_glunsupported("glGetActiveUniform");
	glGetActiveUniform((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_getuniformfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformfv) mogl_glunsupported("glGetUniformfv");
	glGetUniformfv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getuniformiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformiv) mogl_glunsupported("glGetUniformiv");
	glGetUniformiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getshadersource( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetShaderSource) mogl_glunsupported("glGetShaderSource");
	glGetShaderSource((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_bindattriblocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindAttribLocation) mogl_glunsupported("glBindAttribLocation");
	glBindAttribLocation((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getactiveattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveAttrib) mogl_glunsupported("glGetActiveAttrib");
	glGetActiveAttrib((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_getattriblocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetAttribLocation) mogl_glunsupported("glGetAttribLocation");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetAttribLocation((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_stencilfuncseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilFuncSeparate) mogl_glunsupported("glStencilFuncSeparate");
	glStencilFuncSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_stencilopseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilOpSeparate) mogl_glunsupported("glStencilOpSeparate");
	glStencilOpSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_stencilmaskseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilMaskSeparate) mogl_glunsupported("glStencilMaskSeparate");
	glStencilMaskSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_uniformmatrix2x3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2x3fv) mogl_glunsupported("glUniformMatrix2x3fv");
	glUniformMatrix2x3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3x2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3x2fv) mogl_glunsupported("glUniformMatrix3x2fv");
	glUniformMatrix3x2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix2x4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2x4fv) mogl_glunsupported("glUniformMatrix2x4fv");
	glUniformMatrix2x4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4x2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4x2fv) mogl_glunsupported("glUniformMatrix4x2fv");
	glUniformMatrix4x2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3x4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3x4fv) mogl_glunsupported("glUniformMatrix3x4fv");
	glUniformMatrix3x4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4x3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4x3fv) mogl_glunsupported("glUniformMatrix4x3fv");
	glUniformMatrix4x3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void glu_begincurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginCurve) mogl_glunsupported("gluBeginCurve");
	gluBeginCurve((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_beginsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginSurface) mogl_glunsupported("gluBeginSurface");
	gluBeginSurface((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_begintrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginTrim) mogl_glunsupported("gluBeginTrim");
	gluBeginTrim((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_build1dmipmaplevels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild1DMipmapLevels) mogl_glunsupported("gluBuild1DMipmapLevels");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild1DMipmapLevels((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void glu_build1dmipmaps( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild1DMipmaps) mogl_glunsupported("gluBuild1DMipmaps");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild1DMipmaps((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void glu_build2dmipmaplevels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild2DMipmapLevels) mogl_glunsupported("gluBuild2DMipmapLevels");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild2DMipmapLevels((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void glu_build2dmipmaps( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild2DMipmaps) mogl_glunsupported("gluBuild2DMipmaps");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild2DMipmaps((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void glu_build3dmipmaplevels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild3DMipmapLevels) mogl_glunsupported("gluBuild3DMipmapLevels");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild3DMipmapLevels((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void glu_build3dmipmaps( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBuild3DMipmaps) mogl_glunsupported("gluBuild3DMipmaps");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild3DMipmaps((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void glu_checkextension( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluCheckExtension) mogl_glunsupported("gluCheckExtension");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluCheckExtension((const GLubyte*)mxGetData(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void glu_cylinder( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluCylinder) mogl_glunsupported("gluCylinder");
	gluCylinder((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void glu_deletenurbsrenderer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDeleteNurbsRenderer) mogl_glunsupported("gluDeleteNurbsRenderer");
	gluDeleteNurbsRenderer((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_deletequadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDeleteQuadric) mogl_glunsupported("gluDeleteQuadric");
	gluDeleteQuadric((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_disk( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDisk) mogl_glunsupported("gluDisk");
	gluDisk((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void glu_endcurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndCurve) mogl_glunsupported("gluEndCurve");
	gluEndCurve((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_endsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndSurface) mogl_glunsupported("gluEndSurface");
	gluEndSurface((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_endtrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndTrim) mogl_glunsupported("gluEndTrim");
	gluEndTrim((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void glu_getnurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluGetNurbsProperty) mogl_glunsupported("gluGetNurbsProperty");
	gluGetNurbsProperty((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void glu_gettessproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluGetTessProperty) mogl_glunsupported("gluGetTessProperty");
	gluGetTessProperty((GLUtesselator*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void glu_loadsamplingmatrices( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluLoadSamplingMatrices) mogl_glunsupported("gluLoadSamplingMatrices");
	gluLoadSamplingMatrices((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void glu_lookat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluLookAt) mogl_glunsupported("gluLookAt");
	gluLookAt((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]),
		(GLdouble)mxGetScalar(prhs[7]),
		(GLdouble)mxGetScalar(prhs[8]));

}

void glu_newnurbsrenderer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNewNurbsRenderer) mogl_glunsupported("gluNewNurbsRenderer");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) gluNewNurbsRenderer());

}

void glu_newquadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNewQuadric) mogl_glunsupported("gluNewQuadric");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) gluNewQuadric());

}

void glu_nurbscurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsCurve) mogl_glunsupported("gluNurbsCurve");
	gluNurbsCurve((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]));

}

void glu_nurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsProperty) mogl_glunsupported("gluNurbsProperty");
	gluNurbsProperty((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void glu_nurbssurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsSurface) mogl_glunsupported("gluNurbsSurface");
	gluNurbsSurface((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLfloat*)mxGetData(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(GLenum)mxGetScalar(prhs[10]));

}

void glu_ortho2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluOrtho2D) mogl_glunsupported("gluOrtho2D");
	gluOrtho2D((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_partialdisk( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluPartialDisk) mogl_glunsupported("gluPartialDisk");
	gluPartialDisk((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void glu_perspective( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluPerspective) mogl_glunsupported("gluPerspective");
	gluPerspective((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_pickmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluPickMatrix) mogl_glunsupported("gluPickMatrix");
	gluPickMatrix((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void glu_project( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluProject) mogl_glunsupported("gluProject");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluProject((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]),
		(const GLint*)mxGetData(prhs[5]),
		(GLdouble*)mxGetData(prhs[6]),
		(GLdouble*)mxGetData(prhs[7]),
		(GLdouble*)mxGetData(prhs[8]));

}

void glu_pwlcurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluPwlCurve) mogl_glunsupported("gluPwlCurve");
	gluPwlCurve((GLUnurbs*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void glu_quadricdrawstyle( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricDrawStyle) mogl_glunsupported("gluQuadricDrawStyle");
	gluQuadricDrawStyle((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricnormals( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricNormals) mogl_glunsupported("gluQuadricNormals");
	gluQuadricNormals((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricorientation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricOrientation) mogl_glunsupported("gluQuadricOrientation");
	gluQuadricOrientation((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadrictexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricTexture) mogl_glunsupported("gluQuadricTexture");
	gluQuadricTexture((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLboolean)mxGetScalar(prhs[1]));

}

void glu_scaleimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluScaleImage) mogl_glunsupported("gluScaleImage");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluScaleImage((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLvoid*)mxGetData(prhs[8]));

}

void glu_sphere( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluSphere) mogl_glunsupported("gluSphere");
	gluSphere((GLUquadric*) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glu_unproject( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluUnProject) mogl_glunsupported("gluUnProject");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluUnProject((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]),
		(const GLint*)mxGetData(prhs[5]),
		(GLdouble*)mxGetData(prhs[6]),
		(GLdouble*)mxGetData(prhs[7]),
		(GLdouble*)mxGetData(prhs[8]));

}

void glu_unproject4( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluUnProject4) mogl_glunsupported("gluUnProject4");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluUnProject4((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]),
		(const GLdouble*)mxGetData(prhs[5]),
		(const GLint*)mxGetData(prhs[6]),
		(GLdouble)mxGetScalar(prhs[7]),
		(GLdouble)mxGetScalar(prhs[8]),
		(GLdouble*)mxGetData(prhs[9]),
		(GLdouble*)mxGetData(prhs[10]),
		(GLdouble*)mxGetData(prhs[11]),
		(GLdouble*)mxGetData(prhs[12]));

}

void glut_wiresphere( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireSphere) mogl_glunsupported("glutWireSphere");
	glutWireSphere((GLdouble)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void glut_solidsphere( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidSphere) mogl_glunsupported("glutSolidSphere");
	glutSolidSphere((GLdouble)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void glut_wirecone( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireCone) mogl_glunsupported("glutWireCone");
	glutWireCone((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_solidcone( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidCone) mogl_glunsupported("glutSolidCone");
	glutSolidCone((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_wirecube( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireCube) mogl_glunsupported("glutWireCube");
	glutWireCube((GLdouble)mxGetScalar(prhs[0]));

}

void glut_solidcube( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidCube) mogl_glunsupported("glutSolidCube");
	glutSolidCube((GLdouble)mxGetScalar(prhs[0]));

}

void glut_wiretorus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireTorus) mogl_glunsupported("glutWireTorus");
	glutWireTorus((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_solidtorus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidTorus) mogl_glunsupported("glutSolidTorus");
	glutSolidTorus((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_wiredodecahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireDodecahedron) mogl_glunsupported("glutWireDodecahedron");
	glutWireDodecahedron();

}

void glut_soliddodecahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidDodecahedron) mogl_glunsupported("glutSolidDodecahedron");
	glutSolidDodecahedron();

}

void glut_wireteapot( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireTeapot) mogl_glunsupported("glutWireTeapot");
	glutWireTeapot((GLdouble)mxGetScalar(prhs[0]));

}

void glut_solidteapot( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidTeapot) mogl_glunsupported("glutSolidTeapot");
	glutSolidTeapot((GLdouble)mxGetScalar(prhs[0]));

}

void glut_wireoctahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireOctahedron) mogl_glunsupported("glutWireOctahedron");
	glutWireOctahedron();

}

void glut_solidoctahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidOctahedron) mogl_glunsupported("glutSolidOctahedron");
	glutSolidOctahedron();

}

void glut_wiretetrahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireTetrahedron) mogl_glunsupported("glutWireTetrahedron");
	glutWireTetrahedron();

}

void glut_solidtetrahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidTetrahedron) mogl_glunsupported("glutSolidTetrahedron");
	glutSolidTetrahedron();

}

void glut_wireicosahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutWireIcosahedron) mogl_glunsupported("glutWireIcosahedron");
	glutWireIcosahedron();

}

void glut_solidicosahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glutSolidIcosahedron) mogl_glunsupported("glutSolidIcosahedron");
	glutSolidIcosahedron();

}

void gl_colormaski( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorMaski) mogl_glunsupported("glColorMaski");
	glColorMaski((GLuint)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLboolean)mxGetScalar(prhs[4]));

}

void gl_getbooleani_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBooleani_v) mogl_glunsupported("glGetBooleani_v");
	glGetBooleani_v((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_getintegeri_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetIntegeri_v) mogl_glunsupported("glGetIntegeri_v");
	glGetIntegeri_v((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_enablei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnablei) mogl_glunsupported("glEnablei");
	glEnablei((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_disablei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisablei) mogl_glunsupported("glDisablei");
	glDisablei((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_isenabledi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsEnabledi) mogl_glunsupported("glIsEnabledi");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsEnabledi((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_begintransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginTransformFeedback) mogl_glunsupported("glBeginTransformFeedback");
	glBeginTransformFeedback((GLenum)mxGetScalar(prhs[0]));

}

void gl_endtransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndTransformFeedback) mogl_glunsupported("glEndTransformFeedback");
	glEndTransformFeedback();

}

void gl_bindbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferRange) mogl_glunsupported("glBindBufferRange");
	glBindBufferRange((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_bindbufferbase( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferBase) mogl_glunsupported("glBindBufferBase");
	glBindBufferBase((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_gettransformfeedbackvarying( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbackVarying) mogl_glunsupported("glGetTransformFeedbackVarying");
	glGetTransformFeedbackVarying((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_clampcolor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClampColor) mogl_glunsupported("glClampColor");
	glClampColor((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_beginconditionalrender( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginConditionalRender) mogl_glunsupported("glBeginConditionalRender");
	glBeginConditionalRender((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_endconditionalrender( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndConditionalRender) mogl_glunsupported("glEndConditionalRender");
	glEndConditionalRender();

}

void gl_vertexattribipointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribIPointer) mogl_glunsupported("glVertexAttribIPointer");
	glVertexAttribIPointer((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_getvertexattribiiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribIiv) mogl_glunsupported("glGetVertexAttribIiv");
	glGetVertexAttribIiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvertexattribiuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribIuiv) mogl_glunsupported("glGetVertexAttribIuiv");
	glGetVertexAttribIuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_vertexattribi1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1i) mogl_glunsupported("glVertexAttribI1i");
	glVertexAttribI1i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertexattribi2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2i) mogl_glunsupported("glVertexAttribI2i");
	glVertexAttribI2i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_vertexattribi3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3i) mogl_glunsupported("glVertexAttribI3i");
	glVertexAttribI3i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_vertexattribi4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4i) mogl_glunsupported("glVertexAttribI4i");
	glVertexAttribI4i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_vertexattribi1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1ui) mogl_glunsupported("glVertexAttribI1ui");
	glVertexAttribI1ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexattribi2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2ui) mogl_glunsupported("glVertexAttribI2ui");
	glVertexAttribI2ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexattribi3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3ui) mogl_glunsupported("glVertexAttribI3ui");
	glVertexAttribI3ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribi4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4ui) mogl_glunsupported("glVertexAttribI4ui");
	glVertexAttribI4ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexattribi1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1iv) mogl_glunsupported("glVertexAttribI1iv");
	glVertexAttribI1iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2iv) mogl_glunsupported("glVertexAttribI2iv");
	glVertexAttribI2iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3iv) mogl_glunsupported("glVertexAttribI3iv");
	glVertexAttribI3iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4iv) mogl_glunsupported("glVertexAttribI4iv");
	glVertexAttribI4iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1uiv) mogl_glunsupported("glVertexAttribI1uiv");
	glVertexAttribI1uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2uiv) mogl_glunsupported("glVertexAttribI2uiv");
	glVertexAttribI2uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3uiv) mogl_glunsupported("glVertexAttribI3uiv");
	glVertexAttribI3uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4uiv) mogl_glunsupported("glVertexAttribI4uiv");
	glVertexAttribI4uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4bv) mogl_glunsupported("glVertexAttribI4bv");
	glVertexAttribI4bv((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattribi4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4sv) mogl_glunsupported("glVertexAttribI4sv");
	glVertexAttribI4sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattribi4ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4ubv) mogl_glunsupported("glVertexAttribI4ubv");
	glVertexAttribI4ubv((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattribi4usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4usv) mogl_glunsupported("glVertexAttribI4usv");
	glVertexAttribI4usv((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_getuniformuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformuiv) mogl_glunsupported("glGetUniformuiv");
	glGetUniformuiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_bindfragdatalocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFragDataLocation) mogl_glunsupported("glBindFragDataLocation");
	glBindFragDataLocation((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getfragdatalocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragDataLocation) mogl_glunsupported("glGetFragDataLocation");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetFragDataLocation((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_uniform1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1ui) mogl_glunsupported("glUniform1ui");
	glUniform1ui((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_uniform2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2ui) mogl_glunsupported("glUniform2ui");
	glUniform2ui((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_uniform3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3ui) mogl_glunsupported("glUniform3ui");
	glUniform3ui((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_uniform4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4ui) mogl_glunsupported("glUniform4ui");
	glUniform4ui((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_uniform1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1uiv) mogl_glunsupported("glUniform1uiv");
	glUniform1uiv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2uiv) mogl_glunsupported("glUniform2uiv");
	glUniform2uiv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3uiv) mogl_glunsupported("glUniform3uiv");
	glUniform3uiv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4uiv) mogl_glunsupported("glUniform4uiv");
	glUniform4uiv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_texparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterIiv) mogl_glunsupported("glTexParameterIiv");
	glTexParameterIiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterIuiv) mogl_glunsupported("glTexParameterIuiv");
	glTexParameterIuiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_gettexparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameterIiv) mogl_glunsupported("glGetTexParameterIiv");
	glGetTexParameterIiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettexparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameterIuiv) mogl_glunsupported("glGetTexParameterIuiv");
	glGetTexParameterIuiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_clearbufferiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferiv) mogl_glunsupported("glClearBufferiv");
	glClearBufferiv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_clearbufferuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferuiv) mogl_glunsupported("glClearBufferuiv");
	glClearBufferuiv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_clearbufferfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferfv) mogl_glunsupported("glClearBufferfv");
	glClearBufferfv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_clearbufferfi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferfi) mogl_glunsupported("glClearBufferfi");
	glClearBufferfi((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_getstringi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetStringi) mogl_glunsupported("glGetStringi");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) glGetStringi((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1])));

}

void gl_isrenderbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsRenderbuffer) mogl_glunsupported("glIsRenderbuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsRenderbuffer((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindrenderbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindRenderbuffer) mogl_glunsupported("glBindRenderbuffer");
	glBindRenderbuffer((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleterenderbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteRenderbuffers) mogl_glunsupported("glDeleteRenderbuffers");
	glDeleteRenderbuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genrenderbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenRenderbuffers) mogl_glunsupported("glGenRenderbuffers");
	glGenRenderbuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_renderbufferstorage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderbufferStorage) mogl_glunsupported("glRenderbufferStorage");
	glRenderbufferStorage((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_getrenderbufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetRenderbufferParameteriv) mogl_glunsupported("glGetRenderbufferParameteriv");
	glGetRenderbufferParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_isframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsFramebuffer) mogl_glunsupported("glIsFramebuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsFramebuffer((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFramebuffer) mogl_glunsupported("glBindFramebuffer");
	glBindFramebuffer((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleteframebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteFramebuffers) mogl_glunsupported("glDeleteFramebuffers");
	glDeleteFramebuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genframebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenFramebuffers) mogl_glunsupported("glGenFramebuffers");
	glGenFramebuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_checkframebufferstatus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCheckFramebufferStatus) mogl_glunsupported("glCheckFramebufferStatus");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCheckFramebufferStatus((GLenum)mxGetScalar(prhs[0]));

}

void gl_framebuffertexture1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture1D) mogl_glunsupported("glFramebufferTexture1D");
	glFramebufferTexture1D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_framebuffertexture2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture2D) mogl_glunsupported("glFramebufferTexture2D");
	glFramebufferTexture2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_framebuffertexture3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture3D) mogl_glunsupported("glFramebufferTexture3D");
	glFramebufferTexture3D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_framebufferrenderbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferRenderbuffer) mogl_glunsupported("glFramebufferRenderbuffer");
	glFramebufferRenderbuffer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_getframebufferattachmentparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFramebufferAttachmentParameteriv) mogl_glunsupported("glGetFramebufferAttachmentParameteriv");
	glGetFramebufferAttachmentParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_generatemipmap( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenerateMipmap) mogl_glunsupported("glGenerateMipmap");
	glGenerateMipmap((GLenum)mxGetScalar(prhs[0]));

}

void gl_blitframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlitFramebuffer) mogl_glunsupported("glBlitFramebuffer");
	glBlitFramebuffer((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLbitfield)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]));

}

void gl_renderbufferstoragemultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderbufferStorageMultisample) mogl_glunsupported("glRenderbufferStorageMultisample");
	glRenderbufferStorageMultisample((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_framebuffertexturelayer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureLayer) mogl_glunsupported("glFramebufferTextureLayer");
	glFramebufferTextureLayer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_mapbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapBufferRange) mogl_glunsupported("glMapBufferRange");
	glMapBufferRange((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_flushmappedbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushMappedBufferRange) mogl_glunsupported("glFlushMappedBufferRange");
	glFlushMappedBufferRange((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]));

}

void gl_bindvertexarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVertexArray) mogl_glunsupported("glBindVertexArray");
	glBindVertexArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_deletevertexarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteVertexArrays) mogl_glunsupported("glDeleteVertexArrays");
	glDeleteVertexArrays((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genvertexarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenVertexArrays) mogl_glunsupported("glGenVertexArrays");
	glGenVertexArrays((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isvertexarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsVertexArray) mogl_glunsupported("glIsVertexArray");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsVertexArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_drawarraysinstanced( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysInstanced) mogl_glunsupported("glDrawArraysInstanced");
	glDrawArraysInstanced((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_drawelementsinstanced( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstanced) mogl_glunsupported("glDrawElementsInstanced");
	glDrawElementsInstanced((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_texbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBuffer) mogl_glunsupported("glTexBuffer");
	glTexBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_primitiverestartindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPrimitiveRestartIndex) mogl_glunsupported("glPrimitiveRestartIndex");
	glPrimitiveRestartIndex((GLuint)mxGetScalar(prhs[0]));

}

void gl_copybuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyBufferSubData) mogl_glunsupported("glCopyBufferSubData");
	glCopyBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_getactiveuniformsiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniformsiv) mogl_glunsupported("glGetActiveUniformsiv");
	glGetActiveUniformsiv((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_getactiveuniformname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniformName) mogl_glunsupported("glGetActiveUniformName");
	glGetActiveUniformName((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_getuniformblockindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformBlockIndex) mogl_glunsupported("glGetUniformBlockIndex");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformBlockIndex((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getactiveuniformblockiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniformBlockiv) mogl_glunsupported("glGetActiveUniformBlockiv");
	glGetActiveUniformBlockiv((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getactiveuniformblockname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniformBlockName) mogl_glunsupported("glGetActiveUniformBlockName");
	glGetActiveUniformBlockName((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_uniformblockbinding( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformBlockBinding) mogl_glunsupported("glUniformBlockBinding");
	glUniformBlockBinding((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_drawelementsbasevertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsBaseVertex) mogl_glunsupported("glDrawElementsBaseVertex");
	glDrawElementsBaseVertex((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_drawrangeelementsbasevertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawRangeElementsBaseVertex) mogl_glunsupported("glDrawRangeElementsBaseVertex");
	glDrawRangeElementsBaseVertex((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_drawelementsinstancedbasevertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstancedBaseVertex) mogl_glunsupported("glDrawElementsInstancedBaseVertex");
	glDrawElementsInstancedBaseVertex((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_provokingvertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProvokingVertex) mogl_glunsupported("glProvokingVertex");
	glProvokingVertex((GLenum)mxGetScalar(prhs[0]));

}

void gl_fencesync( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFenceSync) mogl_glunsupported("glFenceSync");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) glFenceSync((GLenum)mxGetScalar(prhs[0]),
		(GLbitfield)mxGetScalar(prhs[1])));

}

void gl_issync( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsSync) mogl_glunsupported("glIsSync");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsSync((GLsync) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void gl_deletesync( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteSync) mogl_glunsupported("glDeleteSync");
	glDeleteSync((GLsync) PsychDoubleToPtr(mxGetScalar(prhs[0])));

}

void gl_clientwaitsync( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClientWaitSync) mogl_glunsupported("glClientWaitSync");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glClientWaitSync((GLsync) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLbitfield)mxGetScalar(prhs[1]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[2])));

}

void gl_waitsync( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWaitSync) mogl_glunsupported("glWaitSync");
	glWaitSync((GLsync) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLbitfield)mxGetScalar(prhs[1]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[2])));

}

void gl_getinteger64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInteger64v) mogl_glunsupported("glGetInteger64v");
	glGetInteger64v((GLenum)mxGetScalar(prhs[0]),
		(GLint64*)mxGetData(prhs[1]));

}

void gl_getsynciv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSynciv) mogl_glunsupported("glGetSynciv");
	glGetSynciv((GLsync) PsychDoubleToPtr(mxGetScalar(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_getinteger64i_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInteger64i_v) mogl_glunsupported("glGetInteger64i_v");
	glGetInteger64i_v((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint64*)mxGetData(prhs[2]));

}

void gl_getbufferparameteri64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferParameteri64v) mogl_glunsupported("glGetBufferParameteri64v");
	glGetBufferParameteri64v((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64*)mxGetData(prhs[2]));

}

void gl_teximage2dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage2DMultisample) mogl_glunsupported("glTexImage2DMultisample");
	glTexImage2DMultisample((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]));

}

void gl_teximage3dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage3DMultisample) mogl_glunsupported("glTexImage3DMultisample");
	glTexImage3DMultisample((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_getmultisamplefv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultisamplefv) mogl_glunsupported("glGetMultisamplefv");
	glGetMultisamplefv((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_samplemaski( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMaski) mogl_glunsupported("glSampleMaski");
	glSampleMaski((GLuint)mxGetScalar(prhs[0]),
		(GLbitfield)mxGetScalar(prhs[1]));

}

void gl_bindfragdatalocationindexed( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFragDataLocationIndexed) mogl_glunsupported("glBindFragDataLocationIndexed");
	glBindFragDataLocationIndexed((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLchar*)mxGetData(prhs[3]));

}

void gl_getfragdataindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragDataIndex) mogl_glunsupported("glGetFragDataIndex");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetFragDataIndex((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_gensamplers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenSamplers) mogl_glunsupported("glGenSamplers");
	glGenSamplers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deletesamplers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteSamplers) mogl_glunsupported("glDeleteSamplers");
	glDeleteSamplers((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_issampler( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsSampler) mogl_glunsupported("glIsSampler");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsSampler((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindsampler( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindSampler) mogl_glunsupported("glBindSampler");
	glBindSampler((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_samplerparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameteri) mogl_glunsupported("glSamplerParameteri");
	glSamplerParameteri((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_samplerparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameteriv) mogl_glunsupported("glSamplerParameteriv");
	glSamplerParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_samplerparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameterf) mogl_glunsupported("glSamplerParameterf");
	glSamplerParameterf((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_samplerparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameterfv) mogl_glunsupported("glSamplerParameterfv");
	glSamplerParameterfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_samplerparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameterIiv) mogl_glunsupported("glSamplerParameterIiv");
	glSamplerParameterIiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_samplerparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplerParameterIuiv) mogl_glunsupported("glSamplerParameterIuiv");
	glSamplerParameterIuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_getsamplerparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSamplerParameteriv) mogl_glunsupported("glGetSamplerParameteriv");
	glGetSamplerParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getsamplerparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSamplerParameterIiv) mogl_glunsupported("glGetSamplerParameterIiv");
	glGetSamplerParameterIiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getsamplerparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSamplerParameterfv) mogl_glunsupported("glGetSamplerParameterfv");
	glGetSamplerParameterfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getsamplerparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSamplerParameterIuiv) mogl_glunsupported("glGetSamplerParameterIuiv");
	glGetSamplerParameterIuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_querycounter( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glQueryCounter) mogl_glunsupported("glQueryCounter");
	glQueryCounter((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_getqueryobjecti64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjecti64v) mogl_glunsupported("glGetQueryObjecti64v");
	glGetQueryObjecti64v((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64*)mxGetData(prhs[2]));

}

void gl_getqueryobjectui64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectui64v) mogl_glunsupported("glGetQueryObjectui64v");
	glGetQueryObjectui64v((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64*)mxGetData(prhs[2]));

}

void gl_vertexattribdivisor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribDivisor) mogl_glunsupported("glVertexAttribDivisor");
	glVertexAttribDivisor((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexattribp1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP1ui) mogl_glunsupported("glVertexAttribP1ui");
	glVertexAttribP1ui((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribp1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP1uiv) mogl_glunsupported("glVertexAttribP1uiv");
	glVertexAttribP1uiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_vertexattribp2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP2ui) mogl_glunsupported("glVertexAttribP2ui");
	glVertexAttribP2ui((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribp2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP2uiv) mogl_glunsupported("glVertexAttribP2uiv");
	glVertexAttribP2uiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_vertexattribp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP3ui) mogl_glunsupported("glVertexAttribP3ui");
	glVertexAttribP3ui((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP3uiv) mogl_glunsupported("glVertexAttribP3uiv");
	glVertexAttribP3uiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_vertexattribp4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP4ui) mogl_glunsupported("glVertexAttribP4ui");
	glVertexAttribP4ui((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribp4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribP4uiv) mogl_glunsupported("glVertexAttribP4uiv");
	glVertexAttribP4uiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_vertexp2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP2ui) mogl_glunsupported("glVertexP2ui");
	glVertexP2ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexp2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP2uiv) mogl_glunsupported("glVertexP2uiv");
	glVertexP2uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP3ui) mogl_glunsupported("glVertexP3ui");
	glVertexP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP3uiv) mogl_glunsupported("glVertexP3uiv");
	glVertexP3uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexp4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP4ui) mogl_glunsupported("glVertexP4ui");
	glVertexP4ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexp4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexP4uiv) mogl_glunsupported("glVertexP4uiv");
	glVertexP4uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_texcoordp1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP1ui) mogl_glunsupported("glTexCoordP1ui");
	glTexCoordP1ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_texcoordp1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP1uiv) mogl_glunsupported("glTexCoordP1uiv");
	glTexCoordP1uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_texcoordp2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP2ui) mogl_glunsupported("glTexCoordP2ui");
	glTexCoordP2ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_texcoordp2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP2uiv) mogl_glunsupported("glTexCoordP2uiv");
	glTexCoordP2uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_texcoordp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP3ui) mogl_glunsupported("glTexCoordP3ui");
	glTexCoordP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_texcoordp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP3uiv) mogl_glunsupported("glTexCoordP3uiv");
	glTexCoordP3uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_texcoordp4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP4ui) mogl_glunsupported("glTexCoordP4ui");
	glTexCoordP4ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_texcoordp4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordP4uiv) mogl_glunsupported("glTexCoordP4uiv");
	glTexCoordP4uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_multitexcoordp1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP1ui) mogl_glunsupported("glMultiTexCoordP1ui");
	glMultiTexCoordP1ui((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexcoordp1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP1uiv) mogl_glunsupported("glMultiTexCoordP1uiv");
	glMultiTexCoordP1uiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_multitexcoordp2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP2ui) mogl_glunsupported("glMultiTexCoordP2ui");
	glMultiTexCoordP2ui((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexcoordp2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP2uiv) mogl_glunsupported("glMultiTexCoordP2uiv");
	glMultiTexCoordP2uiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_multitexcoordp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP3ui) mogl_glunsupported("glMultiTexCoordP3ui");
	glMultiTexCoordP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexcoordp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP3uiv) mogl_glunsupported("glMultiTexCoordP3uiv");
	glMultiTexCoordP3uiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_multitexcoordp4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP4ui) mogl_glunsupported("glMultiTexCoordP4ui");
	glMultiTexCoordP4ui((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexcoordp4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordP4uiv) mogl_glunsupported("glMultiTexCoordP4uiv");
	glMultiTexCoordP4uiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_normalp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalP3ui) mogl_glunsupported("glNormalP3ui");
	glNormalP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_normalp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalP3uiv) mogl_glunsupported("glNormalP3uiv");
	glNormalP3uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_colorp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorP3ui) mogl_glunsupported("glColorP3ui");
	glColorP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_colorp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorP3uiv) mogl_glunsupported("glColorP3uiv");
	glColorP3uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_colorp4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorP4ui) mogl_glunsupported("glColorP4ui");
	glColorP4ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_colorp4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorP4uiv) mogl_glunsupported("glColorP4uiv");
	glColorP4uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_secondarycolorp3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColorP3ui) mogl_glunsupported("glSecondaryColorP3ui");
	glSecondaryColorP3ui((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_secondarycolorp3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColorP3uiv) mogl_glunsupported("glSecondaryColorP3uiv");
	glSecondaryColorP3uiv((GLenum)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_minsampleshading( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMinSampleShading) mogl_glunsupported("glMinSampleShading");
	glMinSampleShading((GLfloat)mxGetScalar(prhs[0]));

}

void gl_blendequationi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationi) mogl_glunsupported("glBlendEquationi");
	glBlendEquationi((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendequationseparatei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationSeparatei) mogl_glunsupported("glBlendEquationSeparatei");
	glBlendEquationSeparatei((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_blendfunci( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFunci) mogl_glunsupported("glBlendFunci");
	glBlendFunci((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_blendfuncseparatei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncSeparatei) mogl_glunsupported("glBlendFuncSeparatei");
	glBlendFuncSeparatei((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_drawarraysindirect( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysIndirect) mogl_glunsupported("glDrawArraysIndirect");
	glDrawArraysIndirect((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_drawelementsindirect( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsIndirect) mogl_glunsupported("glDrawElementsIndirect");
	glDrawElementsIndirect((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_uniform1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1d) mogl_glunsupported("glUniform1d");
	glUniform1d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_uniform2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2d) mogl_glunsupported("glUniform2d");
	glUniform2d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_uniform3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3d) mogl_glunsupported("glUniform3d");
	glUniform3d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_uniform4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4d) mogl_glunsupported("glUniform4d");
	glUniform4d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_uniform1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1dv) mogl_glunsupported("glUniform1dv");
	glUniform1dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_uniform2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2dv) mogl_glunsupported("glUniform2dv");
	glUniform2dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_uniform3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3dv) mogl_glunsupported("glUniform3dv");
	glUniform3dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_uniform4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4dv) mogl_glunsupported("glUniform4dv");
	glUniform4dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_uniformmatrix2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2dv) mogl_glunsupported("glUniformMatrix2dv");
	glUniformMatrix2dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3dv) mogl_glunsupported("glUniformMatrix3dv");
	glUniformMatrix3dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4dv) mogl_glunsupported("glUniformMatrix4dv");
	glUniformMatrix4dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix2x3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2x3dv) mogl_glunsupported("glUniformMatrix2x3dv");
	glUniformMatrix2x3dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix2x4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2x4dv) mogl_glunsupported("glUniformMatrix2x4dv");
	glUniformMatrix2x4dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3x2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3x2dv) mogl_glunsupported("glUniformMatrix3x2dv");
	glUniformMatrix3x2dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3x4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3x4dv) mogl_glunsupported("glUniformMatrix3x4dv");
	glUniformMatrix3x4dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4x2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4x2dv) mogl_glunsupported("glUniformMatrix4x2dv");
	glUniformMatrix4x2dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4x3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4x3dv) mogl_glunsupported("glUniformMatrix4x3dv");
	glUniformMatrix4x3dv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_getuniformdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformdv) mogl_glunsupported("glGetUniformdv");
	glGetUniformdv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getsubroutineuniformlocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSubroutineUniformLocation) mogl_glunsupported("glGetSubroutineUniformLocation");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetSubroutineUniformLocation((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getsubroutineindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSubroutineIndex) mogl_glunsupported("glGetSubroutineIndex");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetSubroutineIndex((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getactivesubroutineuniformiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveSubroutineUniformiv) mogl_glunsupported("glGetActiveSubroutineUniformiv");
	glGetActiveSubroutineUniformiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_getactivesubroutineuniformname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveSubroutineUniformName) mogl_glunsupported("glGetActiveSubroutineUniformName");
	glGetActiveSubroutineUniformName((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLchar*)mxGetData(prhs[5]));

}

void gl_getactivesubroutinename( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveSubroutineName) mogl_glunsupported("glGetActiveSubroutineName");
	glGetActiveSubroutineName((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLchar*)mxGetData(prhs[5]));

}

void gl_uniformsubroutinesuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformSubroutinesuiv) mogl_glunsupported("glUniformSubroutinesuiv");
	glUniformSubroutinesuiv((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_getuniformsubroutineuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformSubroutineuiv) mogl_glunsupported("glGetUniformSubroutineuiv");
	glGetUniformSubroutineuiv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_getprogramstageiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramStageiv) mogl_glunsupported("glGetProgramStageiv");
	glGetProgramStageiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_patchparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPatchParameteri) mogl_glunsupported("glPatchParameteri");
	glPatchParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_patchparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPatchParameterfv) mogl_glunsupported("glPatchParameterfv");
	glPatchParameterfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_bindtransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTransformFeedback) mogl_glunsupported("glBindTransformFeedback");
	glBindTransformFeedback((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletetransformfeedbacks( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteTransformFeedbacks) mogl_glunsupported("glDeleteTransformFeedbacks");
	glDeleteTransformFeedbacks((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_gentransformfeedbacks( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenTransformFeedbacks) mogl_glunsupported("glGenTransformFeedbacks");
	glGenTransformFeedbacks((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_istransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTransformFeedback) mogl_glunsupported("glIsTransformFeedback");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTransformFeedback((GLuint)mxGetScalar(prhs[0]));

}

void gl_pausetransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPauseTransformFeedback) mogl_glunsupported("glPauseTransformFeedback");
	glPauseTransformFeedback();

}

void gl_resumetransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResumeTransformFeedback) mogl_glunsupported("glResumeTransformFeedback");
	glResumeTransformFeedback();

}

void gl_drawtransformfeedback( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTransformFeedback) mogl_glunsupported("glDrawTransformFeedback");
	glDrawTransformFeedback((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_drawtransformfeedbackstream( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTransformFeedbackStream) mogl_glunsupported("glDrawTransformFeedbackStream");
	glDrawTransformFeedbackStream((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_beginqueryindexed( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginQueryIndexed) mogl_glunsupported("glBeginQueryIndexed");
	glBeginQueryIndexed((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_endqueryindexed( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndQueryIndexed) mogl_glunsupported("glEndQueryIndexed");
	glEndQueryIndexed((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_getqueryindexediv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryIndexediv) mogl_glunsupported("glGetQueryIndexediv");
	glGetQueryIndexediv((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_releaseshadercompiler( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReleaseShaderCompiler) mogl_glunsupported("glReleaseShaderCompiler");
	glReleaseShaderCompiler();

}

void gl_shaderbinary( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShaderBinary) mogl_glunsupported("glShaderBinary");
	glShaderBinary((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_getshaderprecisionformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetShaderPrecisionFormat) mogl_glunsupported("glGetShaderPrecisionFormat");
	glGetShaderPrecisionFormat((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_depthrangef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRangef) mogl_glunsupported("glDepthRangef");
	glDepthRangef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_cleardepthf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearDepthf) mogl_glunsupported("glClearDepthf");
	glClearDepthf((GLfloat)mxGetScalar(prhs[0]));

}

void gl_getprogrambinary( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramBinary) mogl_glunsupported("glGetProgramBinary");
	glGetProgramBinary((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLenum*)mxGetData(prhs[3]),
		(void*)mxGetData(prhs[4]));

}

void gl_programbinary( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramBinary) mogl_glunsupported("glProgramBinary");
	glProgramBinary((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_programparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameteri) mogl_glunsupported("glProgramParameteri");
	glProgramParameteri((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_useprogramstages( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUseProgramStages) mogl_glunsupported("glUseProgramStages");
	glUseProgramStages((GLuint)mxGetScalar(prhs[0]),
		(GLbitfield)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_activeshaderprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveShaderProgram) mogl_glunsupported("glActiveShaderProgram");
	glActiveShaderProgram((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_bindprogrampipeline( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindProgramPipeline) mogl_glunsupported("glBindProgramPipeline");
	glBindProgramPipeline((GLuint)mxGetScalar(prhs[0]));

}

void gl_deleteprogrampipelines( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteProgramPipelines) mogl_glunsupported("glDeleteProgramPipelines");
	glDeleteProgramPipelines((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genprogrampipelines( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenProgramPipelines) mogl_glunsupported("glGenProgramPipelines");
	glGenProgramPipelines((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isprogrampipeline( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsProgramPipeline) mogl_glunsupported("glIsProgramPipeline");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsProgramPipeline((GLuint)mxGetScalar(prhs[0]));

}

void gl_getprogrampipelineiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramPipelineiv) mogl_glunsupported("glGetProgramPipelineiv");
	glGetProgramPipelineiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_programuniform1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1i) mogl_glunsupported("glProgramUniform1i");
	glProgramUniform1i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_programuniform1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1iv) mogl_glunsupported("glProgramUniform1iv");
	glProgramUniform1iv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1f) mogl_glunsupported("glProgramUniform1f");
	glProgramUniform1f((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_programuniform1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1fv) mogl_glunsupported("glProgramUniform1fv");
	glProgramUniform1fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1d) mogl_glunsupported("glProgramUniform1d");
	glProgramUniform1d((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_programuniform1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1dv) mogl_glunsupported("glProgramUniform1dv");
	glProgramUniform1dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programuniform1ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1ui) mogl_glunsupported("glProgramUniform1ui");
	glProgramUniform1ui((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_programuniform1uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1uiv) mogl_glunsupported("glProgramUniform1uiv");
	glProgramUniform1uiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2i) mogl_glunsupported("glProgramUniform2i");
	glProgramUniform2i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_programuniform2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2iv) mogl_glunsupported("glProgramUniform2iv");
	glProgramUniform2iv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2f) mogl_glunsupported("glProgramUniform2f");
	glProgramUniform2f((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_programuniform2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2fv) mogl_glunsupported("glProgramUniform2fv");
	glProgramUniform2fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2d) mogl_glunsupported("glProgramUniform2d");
	glProgramUniform2d((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_programuniform2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2dv) mogl_glunsupported("glProgramUniform2dv");
	glProgramUniform2dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programuniform2ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2ui) mogl_glunsupported("glProgramUniform2ui");
	glProgramUniform2ui((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_programuniform2uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2uiv) mogl_glunsupported("glProgramUniform2uiv");
	glProgramUniform2uiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3i) mogl_glunsupported("glProgramUniform3i");
	glProgramUniform3i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_programuniform3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3iv) mogl_glunsupported("glProgramUniform3iv");
	glProgramUniform3iv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3f) mogl_glunsupported("glProgramUniform3f");
	glProgramUniform3f((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_programuniform3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3fv) mogl_glunsupported("glProgramUniform3fv");
	glProgramUniform3fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3d) mogl_glunsupported("glProgramUniform3d");
	glProgramUniform3d((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_programuniform3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3dv) mogl_glunsupported("glProgramUniform3dv");
	glProgramUniform3dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programuniform3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3ui) mogl_glunsupported("glProgramUniform3ui");
	glProgramUniform3ui((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_programuniform3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3uiv) mogl_glunsupported("glProgramUniform3uiv");
	glProgramUniform3uiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4i) mogl_glunsupported("glProgramUniform4i");
	glProgramUniform4i((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_programuniform4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4iv) mogl_glunsupported("glProgramUniform4iv");
	glProgramUniform4iv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4f) mogl_glunsupported("glProgramUniform4f");
	glProgramUniform4f((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_programuniform4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4fv) mogl_glunsupported("glProgramUniform4fv");
	glProgramUniform4fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4d) mogl_glunsupported("glProgramUniform4d");
	glProgramUniform4d((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_programuniform4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4dv) mogl_glunsupported("glProgramUniform4dv");
	glProgramUniform4dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programuniform4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4ui) mogl_glunsupported("glProgramUniform4ui");
	glProgramUniform4ui((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_programuniform4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4uiv) mogl_glunsupported("glProgramUniform4uiv");
	glProgramUniform4uiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniformmatrix2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2fv) mogl_glunsupported("glProgramUniformMatrix2fv");
	glProgramUniformMatrix2fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3fv) mogl_glunsupported("glProgramUniformMatrix3fv");
	glProgramUniformMatrix3fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4fv) mogl_glunsupported("glProgramUniformMatrix4fv");
	glProgramUniformMatrix4fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2dv) mogl_glunsupported("glProgramUniformMatrix2dv");
	glProgramUniformMatrix2dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3dv) mogl_glunsupported("glProgramUniformMatrix3dv");
	glProgramUniformMatrix3dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4dv) mogl_glunsupported("glProgramUniformMatrix4dv");
	glProgramUniformMatrix4dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x3fv) mogl_glunsupported("glProgramUniformMatrix2x3fv");
	glProgramUniformMatrix2x3fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x2fv) mogl_glunsupported("glProgramUniformMatrix3x2fv");
	glProgramUniformMatrix3x2fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x4fv) mogl_glunsupported("glProgramUniformMatrix2x4fv");
	glProgramUniformMatrix2x4fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x2fv) mogl_glunsupported("glProgramUniformMatrix4x2fv");
	glProgramUniformMatrix4x2fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x4fv) mogl_glunsupported("glProgramUniformMatrix3x4fv");
	glProgramUniformMatrix3x4fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x3fv) mogl_glunsupported("glProgramUniformMatrix4x3fv");
	glProgramUniformMatrix4x3fv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x3dv) mogl_glunsupported("glProgramUniformMatrix2x3dv");
	glProgramUniformMatrix2x3dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x2dv) mogl_glunsupported("glProgramUniformMatrix3x2dv");
	glProgramUniformMatrix3x2dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x4dv) mogl_glunsupported("glProgramUniformMatrix2x4dv");
	glProgramUniformMatrix2x4dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x2dv) mogl_glunsupported("glProgramUniformMatrix4x2dv");
	glProgramUniformMatrix4x2dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x4dv) mogl_glunsupported("glProgramUniformMatrix3x4dv");
	glProgramUniformMatrix3x4dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x3dv) mogl_glunsupported("glProgramUniformMatrix4x3dv");
	glProgramUniformMatrix4x3dv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLdouble*)mxGetData(prhs[4]));

}

void gl_validateprogrampipeline( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glValidateProgramPipeline) mogl_glunsupported("glValidateProgramPipeline");
	glValidateProgramPipeline((GLuint)mxGetScalar(prhs[0]));

}

void gl_getprogrampipelineinfolog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramPipelineInfoLog) mogl_glunsupported("glGetProgramPipelineInfoLog");
	glGetProgramPipelineInfoLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_vertexattribl1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1d) mogl_glunsupported("glVertexAttribL1d");
	glVertexAttribL1d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattribl2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2d) mogl_glunsupported("glVertexAttribL2d");
	glVertexAttribL2d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattribl3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3d) mogl_glunsupported("glVertexAttribL3d");
	glVertexAttribL3d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattribl4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4d) mogl_glunsupported("glVertexAttribL4d");
	glVertexAttribL4d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattribl1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1dv) mogl_glunsupported("glVertexAttribL1dv");
	glVertexAttribL1dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2dv) mogl_glunsupported("glVertexAttribL2dv");
	glVertexAttribL2dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3dv) mogl_glunsupported("glVertexAttribL3dv");
	glVertexAttribL3dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4dv) mogl_glunsupported("glVertexAttribL4dv");
	glVertexAttribL4dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattriblpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribLPointer) mogl_glunsupported("glVertexAttribLPointer");
	glVertexAttribLPointer((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_getvertexattribldv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribLdv) mogl_glunsupported("glGetVertexAttribLdv");
	glGetVertexAttribLdv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_viewportarrayv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glViewportArrayv) mogl_glunsupported("glViewportArrayv");
	glViewportArrayv((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_viewportindexedf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glViewportIndexedf) mogl_glunsupported("glViewportIndexedf");
	glViewportIndexedf((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_viewportindexedfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glViewportIndexedfv) mogl_glunsupported("glViewportIndexedfv");
	glViewportIndexedfv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_scissorarrayv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScissorArrayv) mogl_glunsupported("glScissorArrayv");
	glScissorArrayv((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_scissorindexed( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScissorIndexed) mogl_glunsupported("glScissorIndexed");
	glScissorIndexed((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_scissorindexedv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glScissorIndexedv) mogl_glunsupported("glScissorIndexedv");
	glScissorIndexedv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_depthrangearrayv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRangeArrayv) mogl_glunsupported("glDepthRangeArrayv");
	glDepthRangeArrayv((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_depthrangeindexed( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRangeIndexed) mogl_glunsupported("glDepthRangeIndexed");
	glDepthRangeIndexed((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_getfloati_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFloati_v) mogl_glunsupported("glGetFloati_v");
	glGetFloati_v((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getdoublei_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDoublei_v) mogl_glunsupported("glGetDoublei_v");
	glGetDoublei_v((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_drawarraysinstancedbaseinstance( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysInstancedBaseInstance) mogl_glunsupported("glDrawArraysInstancedBaseInstance");
	glDrawArraysInstancedBaseInstance((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_drawelementsinstancedbaseinstance( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstancedBaseInstance) mogl_glunsupported("glDrawElementsInstancedBaseInstance");
	glDrawElementsInstancedBaseInstance((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_drawelementsinstancedbasevertexbaseinstance( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstancedBaseVertexBaseInstance) mogl_glunsupported("glDrawElementsInstancedBaseVertexBaseInstance");
	glDrawElementsInstancedBaseVertexBaseInstance((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]));

}

void gl_getinternalformativ( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInternalformativ) mogl_glunsupported("glGetInternalformativ");
	glGetInternalformativ((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_getactiveatomiccounterbufferiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveAtomicCounterBufferiv) mogl_glunsupported("glGetActiveAtomicCounterBufferiv");
	glGetActiveAtomicCounterBufferiv((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_bindimagetexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindImageTexture) mogl_glunsupported("glBindImageTexture");
	glBindImageTexture((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]));

}

void gl_memorybarrier( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMemoryBarrier) mogl_glunsupported("glMemoryBarrier");
	glMemoryBarrier((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_texstorage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorage1D) mogl_glunsupported("glTexStorage1D");
	glTexStorage1D((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_texstorage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorage2D) mogl_glunsupported("glTexStorage2D");
	glTexStorage2D((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_texstorage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorage3D) mogl_glunsupported("glTexStorage3D");
	glTexStorage3D((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_drawtransformfeedbackinstanced( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTransformFeedbackInstanced) mogl_glunsupported("glDrawTransformFeedbackInstanced");
	glDrawTransformFeedbackInstanced((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_drawtransformfeedbackstreaminstanced( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTransformFeedbackStreamInstanced) mogl_glunsupported("glDrawTransformFeedbackStreamInstanced");
	glDrawTransformFeedbackStreamInstanced((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_clearbufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferData) mogl_glunsupported("glClearBufferData");
	glClearBufferData((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_clearbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearBufferSubData) mogl_glunsupported("glClearBufferSubData");
	glClearBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLsizeiptr)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_dispatchcompute( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDispatchCompute) mogl_glunsupported("glDispatchCompute");
	glDispatchCompute((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_dispatchcomputeindirect( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDispatchComputeIndirect) mogl_glunsupported("glDispatchComputeIndirect");
	glDispatchComputeIndirect((GLintptr)mxGetScalar(prhs[0]));

}

void gl_copyimagesubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyImageSubData) mogl_glunsupported("glCopyImageSubData");
	glCopyImageSubData((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(GLint)mxGetScalar(prhs[10]),
		(GLint)mxGetScalar(prhs[11]),
		(GLsizei)mxGetScalar(prhs[12]),
		(GLsizei)mxGetScalar(prhs[13]),
		(GLsizei)mxGetScalar(prhs[14]));

}

void gl_framebufferparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferParameteri) mogl_glunsupported("glFramebufferParameteri");
	glFramebufferParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_getframebufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFramebufferParameteriv) mogl_glunsupported("glGetFramebufferParameteriv");
	glGetFramebufferParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getinternalformati64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInternalformati64v) mogl_glunsupported("glGetInternalformati64v");
	glGetInternalformati64v((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint64*)mxGetData(prhs[4]));

}

void gl_invalidatetexsubimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateTexSubImage) mogl_glunsupported("glInvalidateTexSubImage");
	glInvalidateTexSubImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]));

}

void gl_invalidateteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateTexImage) mogl_glunsupported("glInvalidateTexImage");
	glInvalidateTexImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_invalidatebuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateBufferSubData) mogl_glunsupported("glInvalidateBufferSubData");
	glInvalidateBufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]));

}

void gl_invalidatebufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateBufferData) mogl_glunsupported("glInvalidateBufferData");
	glInvalidateBufferData((GLuint)mxGetScalar(prhs[0]));

}

void gl_invalidateframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateFramebuffer) mogl_glunsupported("glInvalidateFramebuffer");
	glInvalidateFramebuffer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]));

}

void gl_invalidatesubframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateSubFramebuffer) mogl_glunsupported("glInvalidateSubFramebuffer");
	glInvalidateSubFramebuffer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]));

}

void gl_multidrawarraysindirect( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysIndirect) mogl_glunsupported("glMultiDrawArraysIndirect");
	glMultiDrawArraysIndirect((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_multidrawelementsindirect( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsIndirect) mogl_glunsupported("glMultiDrawElementsIndirect");
	glMultiDrawElementsIndirect((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_getprograminterfaceiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramInterfaceiv) mogl_glunsupported("glGetProgramInterfaceiv");
	glGetProgramInterfaceiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getprogramresourceindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramResourceIndex) mogl_glunsupported("glGetProgramResourceIndex");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetProgramResourceIndex((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getprogramresourcename( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramResourceName) mogl_glunsupported("glGetProgramResourceName");
	glGetProgramResourceName((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLchar*)mxGetData(prhs[5]));

}

void gl_getprogramresourceiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramResourceiv) mogl_glunsupported("glGetProgramResourceiv");
	glGetProgramResourceiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLenum*)mxGetData(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei*)mxGetData(prhs[6]),
		(GLint*)mxGetData(prhs[7]));

}

void gl_getprogramresourcelocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramResourceLocation) mogl_glunsupported("glGetProgramResourceLocation");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetProgramResourceLocation((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getprogramresourcelocationindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramResourceLocationIndex) mogl_glunsupported("glGetProgramResourceLocationIndex");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetProgramResourceLocationIndex((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_shaderstorageblockbinding( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShaderStorageBlockBinding) mogl_glunsupported("glShaderStorageBlockBinding");
	glShaderStorageBlockBinding((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_texbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBufferRange) mogl_glunsupported("glTexBufferRange");
	glTexBufferRange((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_texstorage2dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorage2DMultisample) mogl_glunsupported("glTexStorage2DMultisample");
	glTexStorage2DMultisample((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]));

}

void gl_texstorage3dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorage3DMultisample) mogl_glunsupported("glTexStorage3DMultisample");
	glTexStorage3DMultisample((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_textureview( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureView) mogl_glunsupported("glTextureView");
	glTextureView((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]));

}

void gl_bindvertexbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVertexBuffer) mogl_glunsupported("glBindVertexBuffer");
	glBindVertexBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_vertexattribformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribFormat) mogl_glunsupported("glVertexAttribFormat");
	glVertexAttribFormat((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexattribiformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribIFormat) mogl_glunsupported("glVertexAttribIFormat");
	glVertexAttribIFormat((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattriblformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribLFormat) mogl_glunsupported("glVertexAttribLFormat");
	glVertexAttribLFormat((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribbinding( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribBinding) mogl_glunsupported("glVertexAttribBinding");
	glVertexAttribBinding((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexbindingdivisor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexBindingDivisor) mogl_glunsupported("glVertexBindingDivisor");
	glVertexBindingDivisor((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_debugmessagecontrol( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageControl) mogl_glunsupported("glDebugMessageControl");
	glDebugMessageControl((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLuint*)mxGetData(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]));

}

void gl_debugmessageinsert( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageInsert) mogl_glunsupported("glDebugMessageInsert");
	glDebugMessageInsert((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(const GLchar*)mxGetData(prhs[5]));

}

void gl_getdebugmessagelog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDebugMessageLog) mogl_glunsupported("glGetDebugMessageLog");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetDebugMessageLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum*)mxGetData(prhs[2]),
		(GLenum*)mxGetData(prhs[3]),
		(GLuint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLsizei*)mxGetData(prhs[6]),
		(GLchar*)mxGetData(prhs[7]));

}

void gl_pushdebuggroup( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushDebugGroup) mogl_glunsupported("glPushDebugGroup");
	glPushDebugGroup((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLchar*)mxGetData(prhs[3]));

}

void gl_popdebuggroup( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopDebugGroup) mogl_glunsupported("glPopDebugGroup");
	glPopDebugGroup();

}

void gl_objectlabel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glObjectLabel) mogl_glunsupported("glObjectLabel");
	glObjectLabel((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLchar*)mxGetData(prhs[3]));

}

void gl_getobjectlabel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectLabel) mogl_glunsupported("glGetObjectLabel");
	glGetObjectLabel((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_objectptrlabel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glObjectPtrLabel) mogl_glunsupported("glObjectPtrLabel");
	glObjectPtrLabel((const void*)mxGetData(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getobjectptrlabel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectPtrLabel) mogl_glunsupported("glGetObjectPtrLabel");
	glGetObjectPtrLabel((const void*)mxGetData(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_bufferstorage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferStorage) mogl_glunsupported("glBufferStorage");
	glBufferStorage((GLenum)mxGetScalar(prhs[0]),
		(GLsizeiptr)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_clearteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearTexImage) mogl_glunsupported("glClearTexImage");
	glClearTexImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_cleartexsubimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearTexSubImage) mogl_glunsupported("glClearTexSubImage");
	glClearTexSubImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_bindbuffersbase( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBuffersBase) mogl_glunsupported("glBindBuffersBase");
	glBindBuffersBase((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_bindbuffersrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBuffersRange) mogl_glunsupported("glBindBuffersRange");
	glBindBuffersRange((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]),
		(const GLintptr*)mxGetData(prhs[4]),
		(const GLsizeiptr*)mxGetData(prhs[5]));

}

void gl_bindtextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTextures) mogl_glunsupported("glBindTextures");
	glBindTextures((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_bindsamplers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindSamplers) mogl_glunsupported("glBindSamplers");
	glBindSamplers((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_bindimagetextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindImageTextures) mogl_glunsupported("glBindImageTextures");
	glBindImageTextures((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_bindvertexbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVertexBuffers) mogl_glunsupported("glBindVertexBuffers");
	glBindVertexBuffers((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]),
		(const GLintptr*)mxGetData(prhs[3]),
		(const GLsizei*)mxGetData(prhs[4]));

}

void gl_clipcontrol( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClipControl) mogl_glunsupported("glClipControl");
	glClipControl((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_createtransformfeedbacks( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateTransformFeedbacks) mogl_glunsupported("glCreateTransformFeedbacks");
	glCreateTransformFeedbacks((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_transformfeedbackbufferbase( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTransformFeedbackBufferBase) mogl_glunsupported("glTransformFeedbackBufferBase");
	glTransformFeedbackBufferBase((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_transformfeedbackbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTransformFeedbackBufferRange) mogl_glunsupported("glTransformFeedbackBufferRange");
	glTransformFeedbackBufferRange((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_gettransformfeedbackiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbackiv) mogl_glunsupported("glGetTransformFeedbackiv");
	glGetTransformFeedbackiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettransformfeedbacki_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbacki_v) mogl_glunsupported("glGetTransformFeedbacki_v");
	glGetTransformFeedbacki_v((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettransformfeedbacki64_v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbacki64_v) mogl_glunsupported("glGetTransformFeedbacki64_v");
	glGetTransformFeedbacki64_v((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint64*)mxGetData(prhs[3]));

}

void gl_createbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateBuffers) mogl_glunsupported("glCreateBuffers");
	glCreateBuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_namedbufferstorage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferStorage) mogl_glunsupported("glNamedBufferStorage");
	glNamedBufferStorage((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_namedbufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferData) mogl_glunsupported("glNamedBufferData");
	glNamedBufferData((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_namedbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferSubData) mogl_glunsupported("glNamedBufferSubData");
	glNamedBufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_copynamedbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyNamedBufferSubData) mogl_glunsupported("glCopyNamedBufferSubData");
	glCopyNamedBufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_clearnamedbufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedBufferData) mogl_glunsupported("glClearNamedBufferData");
	glClearNamedBufferData((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_clearnamedbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedBufferSubData) mogl_glunsupported("glClearNamedBufferSubData");
	glClearNamedBufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_mapnamedbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapNamedBuffer) mogl_glunsupported("glMapNamedBuffer");
	glMapNamedBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_mapnamedbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapNamedBufferRange) mogl_glunsupported("glMapNamedBufferRange");
	glMapNamedBufferRange((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_unmapnamedbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapNamedBuffer) mogl_glunsupported("glUnmapNamedBuffer");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glUnmapNamedBuffer((GLuint)mxGetScalar(prhs[0]));

}

void gl_flushmappednamedbufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushMappedNamedBufferRange) mogl_glunsupported("glFlushMappedNamedBufferRange");
	glFlushMappedNamedBufferRange((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_getnamedbufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferParameteriv) mogl_glunsupported("glGetNamedBufferParameteriv");
	glGetNamedBufferParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getnamedbufferparameteri64v( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferParameteri64v) mogl_glunsupported("glGetNamedBufferParameteri64v");
	glGetNamedBufferParameteri64v((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64*)mxGetData(prhs[2]));

}

void gl_getnamedbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferSubData) mogl_glunsupported("glGetNamedBufferSubData");
	glGetNamedBufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_createframebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateFramebuffers) mogl_glunsupported("glCreateFramebuffers");
	glCreateFramebuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_namedframebufferrenderbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferRenderbuffer) mogl_glunsupported("glNamedFramebufferRenderbuffer");
	glNamedFramebufferRenderbuffer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_namedframebufferparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferParameteri) mogl_glunsupported("glNamedFramebufferParameteri");
	glNamedFramebufferParameteri((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_namedframebuffertexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTexture) mogl_glunsupported("glNamedFramebufferTexture");
	glNamedFramebufferTexture((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_namedframebuffertexturelayer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTextureLayer) mogl_glunsupported("glNamedFramebufferTextureLayer");
	glNamedFramebufferTextureLayer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_namedframebufferdrawbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferDrawBuffer) mogl_glunsupported("glNamedFramebufferDrawBuffer");
	glNamedFramebufferDrawBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_namedframebufferdrawbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferDrawBuffers) mogl_glunsupported("glNamedFramebufferDrawBuffers");
	glNamedFramebufferDrawBuffers((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]));

}

void gl_namedframebufferreadbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferReadBuffer) mogl_glunsupported("glNamedFramebufferReadBuffer");
	glNamedFramebufferReadBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_invalidatenamedframebufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateNamedFramebufferData) mogl_glunsupported("glInvalidateNamedFramebufferData");
	glInvalidateNamedFramebufferData((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]));

}

void gl_invalidatenamedframebuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInvalidateNamedFramebufferSubData) mogl_glunsupported("glInvalidateNamedFramebufferSubData");
	glInvalidateNamedFramebufferSubData((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]));

}

void gl_clearnamedframebufferiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedFramebufferiv) mogl_glunsupported("glClearNamedFramebufferiv");
	glClearNamedFramebufferiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_clearnamedframebufferuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedFramebufferuiv) mogl_glunsupported("glClearNamedFramebufferuiv");
	glClearNamedFramebufferuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_clearnamedframebufferfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedFramebufferfv) mogl_glunsupported("glClearNamedFramebufferfv");
	glClearNamedFramebufferfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_clearnamedframebufferfi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedFramebufferfi) mogl_glunsupported("glClearNamedFramebufferfi");
	glClearNamedFramebufferfi((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_blitnamedframebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlitNamedFramebuffer) mogl_glunsupported("glBlitNamedFramebuffer");
	glBlitNamedFramebuffer((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(GLbitfield)mxGetScalar(prhs[10]),
		(GLenum)mxGetScalar(prhs[11]));

}

void gl_checknamedframebufferstatus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCheckNamedFramebufferStatus) mogl_glunsupported("glCheckNamedFramebufferStatus");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCheckNamedFramebufferStatus((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_getnamedframebufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedFramebufferParameteriv) mogl_glunsupported("glGetNamedFramebufferParameteriv");
	glGetNamedFramebufferParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getnamedframebufferattachmentparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedFramebufferAttachmentParameteriv) mogl_glunsupported("glGetNamedFramebufferAttachmentParameteriv");
	glGetNamedFramebufferAttachmentParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_createrenderbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateRenderbuffers) mogl_glunsupported("glCreateRenderbuffers");
	glCreateRenderbuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_namedrenderbufferstorage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedRenderbufferStorage) mogl_glunsupported("glNamedRenderbufferStorage");
	glNamedRenderbufferStorage((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_namedrenderbufferstoragemultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedRenderbufferStorageMultisample) mogl_glunsupported("glNamedRenderbufferStorageMultisample");
	glNamedRenderbufferStorageMultisample((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_getnamedrenderbufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedRenderbufferParameteriv) mogl_glunsupported("glGetNamedRenderbufferParameteriv");
	glGetNamedRenderbufferParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_createtextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateTextures) mogl_glunsupported("glCreateTextures");
	glCreateTextures((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_texturebuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBuffer) mogl_glunsupported("glTextureBuffer");
	glTextureBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_texturebufferrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBufferRange) mogl_glunsupported("glTextureBufferRange");
	glTextureBufferRange((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_texturestorage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage1D) mogl_glunsupported("glTextureStorage1D");
	glTextureStorage1D((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_texturestorage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage2D) mogl_glunsupported("glTextureStorage2D");
	glTextureStorage2D((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_texturestorage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage3D) mogl_glunsupported("glTextureStorage3D");
	glTextureStorage3D((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_texturestorage2dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage2DMultisample) mogl_glunsupported("glTextureStorage2DMultisample");
	glTextureStorage2DMultisample((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]));

}

void gl_texturestorage3dmultisample( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage3DMultisample) mogl_glunsupported("glTextureStorage3DMultisample");
	glTextureStorage3DMultisample((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_texturesubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage1D) mogl_glunsupported("glTextureSubImage1D");
	glTextureSubImage1D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_texturesubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage2D) mogl_glunsupported("glTextureSubImage2D");
	glTextureSubImage2D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_texturesubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage3D) mogl_glunsupported("glTextureSubImage3D");
	glTextureSubImage3D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_compressedtexturesubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage1D) mogl_glunsupported("glCompressedTextureSubImage1D");
	glCompressedTextureSubImage1D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_compressedtexturesubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage2D) mogl_glunsupported("glCompressedTextureSubImage2D");
	glCompressedTextureSubImage2D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_compressedtexturesubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage3D) mogl_glunsupported("glCompressedTextureSubImage3D");
	glCompressedTextureSubImage3D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_copytexturesubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage1D) mogl_glunsupported("glCopyTextureSubImage1D");
	glCopyTextureSubImage1D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copytexturesubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage2D) mogl_glunsupported("glCopyTextureSubImage2D");
	glCopyTextureSubImage2D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]));

}

void gl_copytexturesubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage3D) mogl_glunsupported("glCopyTextureSubImage3D");
	glCopyTextureSubImage3D((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]));

}

void gl_textureparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterf) mogl_glunsupported("glTextureParameterf");
	glTextureParameterf((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_textureparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterfv) mogl_glunsupported("glTextureParameterfv");
	glTextureParameterfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_textureparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameteri) mogl_glunsupported("glTextureParameteri");
	glTextureParameteri((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_textureparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterIiv) mogl_glunsupported("glTextureParameterIiv");
	glTextureParameterIiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_textureparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterIuiv) mogl_glunsupported("glTextureParameterIuiv");
	glTextureParameterIuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_textureparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameteriv) mogl_glunsupported("glTextureParameteriv");
	glTextureParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_generatetexturemipmap( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenerateTextureMipmap) mogl_glunsupported("glGenerateTextureMipmap");
	glGenerateTextureMipmap((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindtextureunit( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTextureUnit) mogl_glunsupported("glBindTextureUnit");
	glBindTextureUnit((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_gettextureimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureImage) mogl_glunsupported("glGetTextureImage");
	glGetTextureImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(void*)mxGetData(prhs[5]));

}

void gl_getcompressedtextureimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedTextureImage) mogl_glunsupported("glGetCompressedTextureImage");
	glGetCompressedTextureImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_gettexturelevelparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureLevelParameterfv) mogl_glunsupported("glGetTextureLevelParameterfv");
	glGetTextureLevelParameterfv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_gettexturelevelparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureLevelParameteriv) mogl_glunsupported("glGetTextureLevelParameteriv");
	glGetTextureLevelParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettextureparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterfv) mogl_glunsupported("glGetTextureParameterfv");
	glGetTextureParameterfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettextureparameteriiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterIiv) mogl_glunsupported("glGetTextureParameterIiv");
	glGetTextureParameterIiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettextureparameteriuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterIuiv) mogl_glunsupported("glGetTextureParameterIuiv");
	glGetTextureParameterIuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_gettextureparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameteriv) mogl_glunsupported("glGetTextureParameteriv");
	glGetTextureParameteriv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_createvertexarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateVertexArrays) mogl_glunsupported("glCreateVertexArrays");
	glCreateVertexArrays((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_disablevertexarrayattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexArrayAttrib) mogl_glunsupported("glDisableVertexArrayAttrib");
	glDisableVertexArrayAttrib((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_enablevertexarrayattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexArrayAttrib) mogl_glunsupported("glEnableVertexArrayAttrib");
	glEnableVertexArrayAttrib((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexarrayelementbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayElementBuffer) mogl_glunsupported("glVertexArrayElementBuffer");
	glVertexArrayElementBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexarrayvertexbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexBuffer) mogl_glunsupported("glVertexArrayVertexBuffer");
	glVertexArrayVertexBuffer((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_vertexarrayvertexbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexBuffers) mogl_glunsupported("glVertexArrayVertexBuffers");
	glVertexArrayVertexBuffers((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]),
		(const GLintptr*)mxGetData(prhs[4]),
		(const GLsizei*)mxGetData(prhs[5]));

}

void gl_vertexarrayattribbinding( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayAttribBinding) mogl_glunsupported("glVertexArrayAttribBinding");
	glVertexArrayAttribBinding((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexarrayattribformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayAttribFormat) mogl_glunsupported("glVertexArrayAttribFormat");
	glVertexArrayAttribFormat((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLboolean)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_vertexarrayattribiformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayAttribIFormat) mogl_glunsupported("glVertexArrayAttribIFormat");
	glVertexArrayAttribIFormat((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexarrayattriblformat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayAttribLFormat) mogl_glunsupported("glVertexArrayAttribLFormat");
	glVertexArrayAttribLFormat((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexarraybindingdivisor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayBindingDivisor) mogl_glunsupported("glVertexArrayBindingDivisor");
	glVertexArrayBindingDivisor((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_getvertexarrayiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexArrayiv) mogl_glunsupported("glGetVertexArrayiv");
	glGetVertexArrayiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvertexarrayindexediv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexArrayIndexediv) mogl_glunsupported("glGetVertexArrayIndexediv");
	glGetVertexArrayIndexediv((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getvertexarrayindexed64iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexArrayIndexed64iv) mogl_glunsupported("glGetVertexArrayIndexed64iv");
	glGetVertexArrayIndexed64iv((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint64*)mxGetData(prhs[3]));

}

void gl_createsamplers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateSamplers) mogl_glunsupported("glCreateSamplers");
	glCreateSamplers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_createprogrampipelines( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateProgramPipelines) mogl_glunsupported("glCreateProgramPipelines");
	glCreateProgramPipelines((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_createqueries( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateQueries) mogl_glunsupported("glCreateQueries");
	glCreateQueries((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_memorybarrierbyregion( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMemoryBarrierByRegion) mogl_glunsupported("glMemoryBarrierByRegion");
	glMemoryBarrierByRegion((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_gettexturesubimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureSubImage) mogl_glunsupported("glGetTextureSubImage");
	glGetTextureSubImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLsizei)mxGetScalar(prhs[10]),
		(void*)mxGetData(prhs[11]));

}

void gl_getcompressedtexturesubimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedTextureSubImage) mogl_glunsupported("glGetCompressedTextureSubImage");
	glGetCompressedTextureSubImage((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(void*)mxGetData(prhs[9]));

}

void gl_getnuniformfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformfv) mogl_glunsupported("glGetnUniformfv");
	glGetnUniformfv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getnuniformiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformiv) mogl_glunsupported("glGetnUniformiv");
	glGetnUniformiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getnuniformuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformuiv) mogl_glunsupported("glGetnUniformuiv");
	glGetnUniformuiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_readnpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReadnPixels) mogl_glunsupported("glReadnPixels");
	glReadnPixels((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(void*)mxGetData(prhs[7]));

}

void gl_texturebarrier( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBarrier) mogl_glunsupported("glTextureBarrier");
	glTextureBarrier();

}

void gl_gettexturehandlearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureHandleARB) mogl_glunsupported("glGetTextureHandleARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetTextureHandleARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_gettexturesamplerhandlearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureSamplerHandleARB) mogl_glunsupported("glGetTextureSamplerHandleARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetTextureSamplerHandleARB((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_maketexturehandleresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeTextureHandleResidentARB) mogl_glunsupported("glMakeTextureHandleResidentARB");
	glMakeTextureHandleResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_maketexturehandlenonresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeTextureHandleNonResidentARB) mogl_glunsupported("glMakeTextureHandleNonResidentARB");
	glMakeTextureHandleNonResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_getimagehandlearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetImageHandleARB) mogl_glunsupported("glGetImageHandleARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetImageHandleARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_makeimagehandleresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeImageHandleResidentARB) mogl_glunsupported("glMakeImageHandleResidentARB");
	glMakeImageHandleResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_makeimagehandlenonresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeImageHandleNonResidentARB) mogl_glunsupported("glMakeImageHandleNonResidentARB");
	glMakeImageHandleNonResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_uniformhandleui64arb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformHandleui64ARB) mogl_glunsupported("glUniformHandleui64ARB");
	glUniformHandleui64ARB((GLint)mxGetScalar(prhs[0]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[1])));

}

void gl_uniformhandleui64varb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformHandleui64vARB) mogl_glunsupported("glUniformHandleui64vARB");
	glUniformHandleui64vARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64*)mxGetData(prhs[2]));

}

void gl_programuniformhandleui64arb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformHandleui64ARB) mogl_glunsupported("glProgramUniformHandleui64ARB");
	glProgramUniformHandleui64ARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[2])));

}

void gl_programuniformhandleui64varb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformHandleui64vARB) mogl_glunsupported("glProgramUniformHandleui64vARB");
	glProgramUniformHandleui64vARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64*)mxGetData(prhs[3]));

}

void gl_istexturehandleresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTextureHandleResidentARB) mogl_glunsupported("glIsTextureHandleResidentARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTextureHandleResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_isimagehandleresidentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsImageHandleResidentARB) mogl_glunsupported("glIsImageHandleResidentARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsImageHandleResidentARB((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_vertexattribl1ui64arb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1ui64ARB) mogl_glunsupported("glVertexAttribL1ui64ARB");
	glVertexAttribL1ui64ARB((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]));

}

void gl_vertexattribl1ui64varb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1ui64vARB) mogl_glunsupported("glVertexAttribL1ui64vARB");
	glVertexAttribL1ui64vARB((GLuint)mxGetScalar(prhs[0]),
		(const GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_getvertexattriblui64varb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribLui64vARB) mogl_glunsupported("glGetVertexAttribLui64vARB");
	glGetVertexAttribLui64vARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_clampcolorarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClampColorARB) mogl_glunsupported("glClampColorARB");
	glClampColorARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_dispatchcomputegroupsizearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDispatchComputeGroupSizeARB) mogl_glunsupported("glDispatchComputeGroupSizeARB");
	glDispatchComputeGroupSizeARB((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_debugmessagecontrolarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageControlARB) mogl_glunsupported("glDebugMessageControlARB");
	glDebugMessageControlARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLuint*)mxGetData(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]));

}

void gl_debugmessageinsertarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageInsertARB) mogl_glunsupported("glDebugMessageInsertARB");
	glDebugMessageInsertARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(const GLchar*)mxGetData(prhs[5]));

}

void gl_getdebugmessagelogarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDebugMessageLogARB) mogl_glunsupported("glGetDebugMessageLogARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetDebugMessageLogARB((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum*)mxGetData(prhs[2]),
		(GLenum*)mxGetData(prhs[3]),
		(GLuint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLsizei*)mxGetData(prhs[6]),
		(GLchar*)mxGetData(prhs[7]));

}

void gl_drawbuffersarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawBuffersARB) mogl_glunsupported("glDrawBuffersARB");
	glDrawBuffersARB((GLsizei)mxGetScalar(prhs[0]),
		(const GLenum*)mxGetData(prhs[1]));

}

void gl_blendequationiarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationiARB) mogl_glunsupported("glBlendEquationiARB");
	glBlendEquationiARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendequationseparateiarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationSeparateiARB) mogl_glunsupported("glBlendEquationSeparateiARB");
	glBlendEquationSeparateiARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_blendfunciarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFunciARB) mogl_glunsupported("glBlendFunciARB");
	glBlendFunciARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_blendfuncseparateiarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncSeparateiARB) mogl_glunsupported("glBlendFuncSeparateiARB");
	glBlendFuncSeparateiARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_drawarraysinstancedarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysInstancedARB) mogl_glunsupported("glDrawArraysInstancedARB");
	glDrawArraysInstancedARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_drawelementsinstancedarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstancedARB) mogl_glunsupported("glDrawElementsInstancedARB");
	glDrawElementsInstancedARB((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_programstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramStringARB) mogl_glunsupported("glProgramStringARB");
	glProgramStringARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_bindprogramarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindProgramARB) mogl_glunsupported("glBindProgramARB");
	glBindProgramARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleteprogramsarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteProgramsARB) mogl_glunsupported("glDeleteProgramsARB");
	glDeleteProgramsARB((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genprogramsarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenProgramsARB) mogl_glunsupported("glGenProgramsARB");
	glGenProgramsARB((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_programenvparameter4darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameter4dARB) mogl_glunsupported("glProgramEnvParameter4dARB");
	glProgramEnvParameter4dARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_programenvparameter4dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameter4dvARB) mogl_glunsupported("glProgramEnvParameter4dvARB");
	glProgramEnvParameter4dvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_programenvparameter4farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameter4fARB) mogl_glunsupported("glProgramEnvParameter4fARB");
	glProgramEnvParameter4fARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_programenvparameter4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameter4fvARB) mogl_glunsupported("glProgramEnvParameter4fvARB");
	glProgramEnvParameter4fvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_programlocalparameter4darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameter4dARB) mogl_glunsupported("glProgramLocalParameter4dARB");
	glProgramLocalParameter4dARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_programlocalparameter4dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameter4dvARB) mogl_glunsupported("glProgramLocalParameter4dvARB");
	glProgramLocalParameter4dvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_programlocalparameter4farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameter4fARB) mogl_glunsupported("glProgramLocalParameter4fARB");
	glProgramLocalParameter4fARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_programlocalparameter4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameter4fvARB) mogl_glunsupported("glProgramLocalParameter4fvARB");
	glProgramLocalParameter4fvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getprogramenvparameterdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramEnvParameterdvARB) mogl_glunsupported("glGetProgramEnvParameterdvARB");
	glGetProgramEnvParameterdvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getprogramenvparameterfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramEnvParameterfvARB) mogl_glunsupported("glGetProgramEnvParameterfvARB");
	glGetProgramEnvParameterfvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getprogramlocalparameterdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramLocalParameterdvARB) mogl_glunsupported("glGetProgramLocalParameterdvARB");
	glGetProgramLocalParameterdvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getprogramlocalparameterfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramLocalParameterfvARB) mogl_glunsupported("glGetProgramLocalParameterfvARB");
	glGetProgramLocalParameterfvARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getprogramivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramivARB) mogl_glunsupported("glGetProgramivARB");
	glGetProgramivARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getprogramstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramStringARB) mogl_glunsupported("glGetProgramStringARB");
	glGetProgramStringARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(void*)mxGetData(prhs[2]));

}

void gl_isprogramarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsProgramARB) mogl_glunsupported("glIsProgramARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsProgramARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_programparameteriarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameteriARB) mogl_glunsupported("glProgramParameteriARB");
	glProgramParameteriARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_framebuffertexturearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureARB) mogl_glunsupported("glFramebufferTextureARB");
	glFramebufferTextureARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_framebuffertexturelayerarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureLayerARB) mogl_glunsupported("glFramebufferTextureLayerARB");
	glFramebufferTextureLayerARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_framebuffertexturefacearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureFaceARB) mogl_glunsupported("glFramebufferTextureFaceARB");
	glFramebufferTextureFaceARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_multidrawarraysindirectcountarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysIndirectCountARB) mogl_glunsupported("glMultiDrawArraysIndirectCountARB");
	glMultiDrawArraysIndirectCountARB((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_multidrawelementsindirectcountarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsIndirectCountARB) mogl_glunsupported("glMultiDrawElementsIndirectCountARB");
	glMultiDrawElementsIndirectCountARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_vertexattribdivisorarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribDivisorARB) mogl_glunsupported("glVertexAttribDivisorARB");
	glVertexAttribDivisorARB((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_currentpalettematrixarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCurrentPaletteMatrixARB) mogl_glunsupported("glCurrentPaletteMatrixARB");
	glCurrentPaletteMatrixARB((GLint)mxGetScalar(prhs[0]));

}

void gl_matrixindexubvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixIndexubvARB) mogl_glunsupported("glMatrixIndexubvARB");
	glMatrixIndexubvARB((GLint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_matrixindexusvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixIndexusvARB) mogl_glunsupported("glMatrixIndexusvARB");
	glMatrixIndexusvARB((GLint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_matrixindexuivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixIndexuivARB) mogl_glunsupported("glMatrixIndexuivARB");
	glMatrixIndexuivARB((GLint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_matrixindexpointerarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixIndexPointerARB) mogl_glunsupported("glMatrixIndexPointerARB");
	glMatrixIndexPointerARB((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_samplecoveragearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleCoverageARB) mogl_glunsupported("glSampleCoverageARB");
	glSampleCoverageARB((GLfloat)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_activetexturearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveTextureARB) mogl_glunsupported("glActiveTextureARB");
	glActiveTextureARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_clientactivetexturearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClientActiveTextureARB) mogl_glunsupported("glClientActiveTextureARB");
	glClientActiveTextureARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_multitexcoord1darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1dARB) mogl_glunsupported("glMultiTexCoord1dARB");
	glMultiTexCoord1dARB((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1dvARB) mogl_glunsupported("glMultiTexCoord1dvARB");
	glMultiTexCoord1dvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord1farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1fARB) mogl_glunsupported("glMultiTexCoord1fARB");
	glMultiTexCoord1fARB((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1fvARB) mogl_glunsupported("glMultiTexCoord1fvARB");
	glMultiTexCoord1fvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord1iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1iARB) mogl_glunsupported("glMultiTexCoord1iARB");
	glMultiTexCoord1iARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1ivARB) mogl_glunsupported("glMultiTexCoord1ivARB");
	glMultiTexCoord1ivARB((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord1sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1sARB) mogl_glunsupported("glMultiTexCoord1sARB");
	glMultiTexCoord1sARB((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord1svARB) mogl_glunsupported("glMultiTexCoord1svARB");
	glMultiTexCoord1svARB((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord2darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2dARB) mogl_glunsupported("glMultiTexCoord2dARB");
	glMultiTexCoord2dARB((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2dvARB) mogl_glunsupported("glMultiTexCoord2dvARB");
	glMultiTexCoord2dvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord2farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2fARB) mogl_glunsupported("glMultiTexCoord2fARB");
	glMultiTexCoord2fARB((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2fvARB) mogl_glunsupported("glMultiTexCoord2fvARB");
	glMultiTexCoord2fvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord2iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2iARB) mogl_glunsupported("glMultiTexCoord2iARB");
	glMultiTexCoord2iARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2ivARB) mogl_glunsupported("glMultiTexCoord2ivARB");
	glMultiTexCoord2ivARB((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord2sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2sARB) mogl_glunsupported("glMultiTexCoord2sARB");
	glMultiTexCoord2sARB((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord2svARB) mogl_glunsupported("glMultiTexCoord2svARB");
	glMultiTexCoord2svARB((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord3darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3dARB) mogl_glunsupported("glMultiTexCoord3dARB");
	glMultiTexCoord3dARB((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3dvARB) mogl_glunsupported("glMultiTexCoord3dvARB");
	glMultiTexCoord3dvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord3farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3fARB) mogl_glunsupported("glMultiTexCoord3fARB");
	glMultiTexCoord3fARB((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3fvARB) mogl_glunsupported("glMultiTexCoord3fvARB");
	glMultiTexCoord3fvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord3iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3iARB) mogl_glunsupported("glMultiTexCoord3iARB");
	glMultiTexCoord3iARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3ivARB) mogl_glunsupported("glMultiTexCoord3ivARB");
	glMultiTexCoord3ivARB((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord3sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3sARB) mogl_glunsupported("glMultiTexCoord3sARB");
	glMultiTexCoord3sARB((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord3svARB) mogl_glunsupported("glMultiTexCoord3svARB");
	glMultiTexCoord3svARB((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord4darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4dARB) mogl_glunsupported("glMultiTexCoord4dARB");
	glMultiTexCoord4dARB((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4dvARB) mogl_glunsupported("glMultiTexCoord4dvARB");
	glMultiTexCoord4dvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord4farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4fARB) mogl_glunsupported("glMultiTexCoord4fARB");
	glMultiTexCoord4fARB((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4fvARB) mogl_glunsupported("glMultiTexCoord4fvARB");
	glMultiTexCoord4fvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord4iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4iARB) mogl_glunsupported("glMultiTexCoord4iARB");
	glMultiTexCoord4iARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4ivARB) mogl_glunsupported("glMultiTexCoord4ivARB");
	glMultiTexCoord4ivARB((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord4sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4sARB) mogl_glunsupported("glMultiTexCoord4sARB");
	glMultiTexCoord4sARB((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoord4svARB) mogl_glunsupported("glMultiTexCoord4svARB");
	glMultiTexCoord4svARB((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_genqueriesarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenQueriesARB) mogl_glunsupported("glGenQueriesARB");
	glGenQueriesARB((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deletequeriesarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteQueriesARB) mogl_glunsupported("glDeleteQueriesARB");
	glDeleteQueriesARB((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_isqueryarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsQueryARB) mogl_glunsupported("glIsQueryARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsQueryARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_beginqueryarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginQueryARB) mogl_glunsupported("glBeginQueryARB");
	glBeginQueryARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_endqueryarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndQueryARB) mogl_glunsupported("glEndQueryARB");
	glEndQueryARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_getqueryivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryivARB) mogl_glunsupported("glGetQueryivARB");
	glGetQueryivARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectivARB) mogl_glunsupported("glGetQueryObjectivARB");
	glGetQueryObjectivARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectuivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectuivARB) mogl_glunsupported("glGetQueryObjectuivARB");
	glGetQueryObjectuivARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_pointparameterfarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterfARB) mogl_glunsupported("glPointParameterfARB");
	glPointParameterfARB((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointparameterfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterfvARB) mogl_glunsupported("glPointParameterfvARB");
	glPointParameterfvARB((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_getgraphicsresetstatusarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetGraphicsResetStatusARB) mogl_glunsupported("glGetGraphicsResetStatusARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetGraphicsResetStatusARB();

}

void gl_getnteximagearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnTexImageARB) mogl_glunsupported("glGetnTexImageARB");
	glGetnTexImageARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(void*)mxGetData(prhs[5]));

}

void gl_readnpixelsarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReadnPixelsARB) mogl_glunsupported("glReadnPixelsARB");
	glReadnPixelsARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(void*)mxGetData(prhs[7]));

}

void gl_getncompressedteximagearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnCompressedTexImageARB) mogl_glunsupported("glGetnCompressedTexImageARB");
	glGetnCompressedTexImageARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_getnuniformfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformfvARB) mogl_glunsupported("glGetnUniformfvARB");
	glGetnUniformfvARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getnuniformivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformivARB) mogl_glunsupported("glGetnUniformivARB");
	glGetnUniformivARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getnuniformuivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformuivARB) mogl_glunsupported("glGetnUniformuivARB");
	glGetnUniformuivARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_getnuniformdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnUniformdvARB) mogl_glunsupported("glGetnUniformdvARB");
	glGetnUniformdvARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_getnmapdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnMapdvARB) mogl_glunsupported("glGetnMapdvARB");
	glGetnMapdvARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_getnmapfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnMapfvARB) mogl_glunsupported("glGetnMapfvARB");
	glGetnMapfvARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getnmapivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnMapivARB) mogl_glunsupported("glGetnMapivARB");
	glGetnMapivARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getnpolygonstipplearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetnPolygonStippleARB) mogl_glunsupported("glGetnPolygonStippleARB");
	glGetnPolygonStippleARB((GLsizei)mxGetScalar(prhs[0]),
		(GLubyte*)mxGetData(prhs[1]));

}

void gl_minsampleshadingarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMinSampleShadingARB) mogl_glunsupported("glMinSampleShadingARB");
	glMinSampleShadingARB((GLfloat)mxGetScalar(prhs[0]));

}

void gl_deleteobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteObjectARB) mogl_glunsupported("glDeleteObjectARB");
	glDeleteObjectARB((GLhandleARB)mxGetScalar(prhs[0]));

}

void gl_gethandlearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHandleARB) mogl_glunsupported("glGetHandleARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetHandleARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_detachobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDetachObjectARB) mogl_glunsupported("glDetachObjectARB");
	glDetachObjectARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLhandleARB)mxGetScalar(prhs[1]));

}

void gl_createshaderobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateShaderObjectARB) mogl_glunsupported("glCreateShaderObjectARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateShaderObjectARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_compileshaderarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompileShaderARB) mogl_glunsupported("glCompileShaderARB");
	glCompileShaderARB((GLhandleARB)mxGetScalar(prhs[0]));

}

void gl_createprogramobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateProgramObjectARB) mogl_glunsupported("glCreateProgramObjectARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateProgramObjectARB();

}

void gl_attachobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAttachObjectARB) mogl_glunsupported("glAttachObjectARB");
	glAttachObjectARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLhandleARB)mxGetScalar(prhs[1]));

}

void gl_linkprogramarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLinkProgramARB) mogl_glunsupported("glLinkProgramARB");
	glLinkProgramARB((GLhandleARB)mxGetScalar(prhs[0]));

}

void gl_useprogramobjectarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUseProgramObjectARB) mogl_glunsupported("glUseProgramObjectARB");
	glUseProgramObjectARB((GLhandleARB)mxGetScalar(prhs[0]));

}

void gl_validateprogramarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glValidateProgramARB) mogl_glunsupported("glValidateProgramARB");
	glValidateProgramARB((GLhandleARB)mxGetScalar(prhs[0]));

}

void gl_uniform1farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1fARB) mogl_glunsupported("glUniform1fARB");
	glUniform1fARB((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_uniform2farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2fARB) mogl_glunsupported("glUniform2fARB");
	glUniform2fARB((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_uniform3farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3fARB) mogl_glunsupported("glUniform3fARB");
	glUniform3fARB((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_uniform4farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4fARB) mogl_glunsupported("glUniform4fARB");
	glUniform4fARB((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_uniform1iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1iARB) mogl_glunsupported("glUniform1iARB");
	glUniform1iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_uniform2iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2iARB) mogl_glunsupported("glUniform2iARB");
	glUniform2iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_uniform3iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3iARB) mogl_glunsupported("glUniform3iARB");
	glUniform3iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_uniform4iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4iARB) mogl_glunsupported("glUniform4iARB");
	glUniform4iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_uniform1fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1fvARB) mogl_glunsupported("glUniform1fvARB");
	glUniform1fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform2fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2fvARB) mogl_glunsupported("glUniform2fvARB");
	glUniform2fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform3fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3fvARB) mogl_glunsupported("glUniform3fvARB");
	glUniform3fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4fvARB) mogl_glunsupported("glUniform4fvARB");
	glUniform4fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform1ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1ivARB) mogl_glunsupported("glUniform1ivARB");
	glUniform1ivARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform2ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2ivARB) mogl_glunsupported("glUniform2ivARB");
	glUniform2ivARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform3ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3ivARB) mogl_glunsupported("glUniform3ivARB");
	glUniform3ivARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform4ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4ivARB) mogl_glunsupported("glUniform4ivARB");
	glUniform4ivARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniformmatrix2fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix2fvARB) mogl_glunsupported("glUniformMatrix2fvARB");
	glUniformMatrix2fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix3fvARB) mogl_glunsupported("glUniformMatrix3fvARB");
	glUniformMatrix3fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformMatrix4fvARB) mogl_glunsupported("glUniformMatrix4fvARB");
	glUniformMatrix4fvARB((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_getobjectparameterfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectParameterfvARB) mogl_glunsupported("glGetObjectParameterfvARB");
	glGetObjectParameterfvARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getobjectparameterivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectParameterivARB) mogl_glunsupported("glGetObjectParameterivARB");
	glGetObjectParameterivARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getinfologarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInfoLogARB) mogl_glunsupported("glGetInfoLogARB");
	glGetInfoLogARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLcharARB*)mxGetData(prhs[3]));

}

void gl_getattachedobjectsarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetAttachedObjectsARB) mogl_glunsupported("glGetAttachedObjectsARB");
	glGetAttachedObjectsARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLhandleARB*)mxGetData(prhs[3]));

}

void gl_getuniformlocationarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformLocationARB) mogl_glunsupported("glGetUniformLocationARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformLocationARB((GLhandleARB)mxGetScalar(prhs[0]),
		(const GLcharARB*)mxGetData(prhs[1]));

}

void gl_getactiveuniformarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveUniformARB) mogl_glunsupported("glGetActiveUniformARB");
	glGetActiveUniformARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLcharARB*)mxGetData(prhs[6]));

}

void gl_getuniformfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformfvARB) mogl_glunsupported("glGetUniformfvARB");
	glGetUniformfvARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getuniformivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformivARB) mogl_glunsupported("glGetUniformivARB");
	glGetUniformivARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getshadersourcearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetShaderSourceARB) mogl_glunsupported("glGetShaderSourceARB");
	glGetShaderSourceARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLcharARB*)mxGetData(prhs[3]));

}

void gl_namedstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedStringARB) mogl_glunsupported("glNamedStringARB");
	glNamedStringARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(const GLchar*)mxGetData(prhs[4]));

}

void gl_deletenamedstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteNamedStringARB) mogl_glunsupported("glDeleteNamedStringARB");
	glDeleteNamedStringARB((GLint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_compileshaderincludearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompileShaderIncludeARB) mogl_glunsupported("glCompileShaderIncludeARB");
	glCompileShaderIncludeARB((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_isnamedstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsNamedStringARB) mogl_glunsupported("glIsNamedStringARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsNamedStringARB((GLint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getnamedstringarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedStringARB) mogl_glunsupported("glGetNamedStringARB");
	glGetNamedStringARB((GLint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_getnamedstringivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedStringivARB) mogl_glunsupported("glGetNamedStringivARB");
	glGetNamedStringivARB((GLint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_bufferpagecommitmentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferPageCommitmentARB) mogl_glunsupported("glBufferPageCommitmentARB");
	glBufferPageCommitmentARB((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_texpagecommitmentarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexPageCommitmentARB) mogl_glunsupported("glTexPageCommitmentARB");
	glTexPageCommitmentARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLboolean)mxGetScalar(prhs[8]));

}

void gl_texbufferarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBufferARB) mogl_glunsupported("glTexBufferARB");
	glTexBufferARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_compressedteximage3darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage3DARB) mogl_glunsupported("glCompressedTexImage3DARB");
	glCompressedTexImage3DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_compressedteximage2darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage2DARB) mogl_glunsupported("glCompressedTexImage2DARB");
	glCompressedTexImage2DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_compressedteximage1darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexImage1DARB) mogl_glunsupported("glCompressedTexImage1DARB");
	glCompressedTexImage1DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_compressedtexsubimage3darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage3DARB) mogl_glunsupported("glCompressedTexSubImage3DARB");
	glCompressedTexSubImage3DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_compressedtexsubimage2darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage2DARB) mogl_glunsupported("glCompressedTexSubImage2DARB");
	glCompressedTexSubImage2DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_compressedtexsubimage1darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTexSubImage1DARB) mogl_glunsupported("glCompressedTexSubImage1DARB");
	glCompressedTexSubImage1DARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_getcompressedteximagearb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedTexImageARB) mogl_glunsupported("glGetCompressedTexImageARB");
	glGetCompressedTexImageARB((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(void*)mxGetData(prhs[2]));

}

void gl_loadtransposematrixfarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadTransposeMatrixfARB) mogl_glunsupported("glLoadTransposeMatrixfARB");
	glLoadTransposeMatrixfARB((const GLfloat*)mxGetData(prhs[0]));

}

void gl_loadtransposematrixdarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadTransposeMatrixdARB) mogl_glunsupported("glLoadTransposeMatrixdARB");
	glLoadTransposeMatrixdARB((const GLdouble*)mxGetData(prhs[0]));

}

void gl_multtransposematrixfarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultTransposeMatrixfARB) mogl_glunsupported("glMultTransposeMatrixfARB");
	glMultTransposeMatrixfARB((const GLfloat*)mxGetData(prhs[0]));

}

void gl_multtransposematrixdarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultTransposeMatrixdARB) mogl_glunsupported("glMultTransposeMatrixdARB");
	glMultTransposeMatrixdARB((const GLdouble*)mxGetData(prhs[0]));

}

void gl_weightbvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightbvARB) mogl_glunsupported("glWeightbvARB");
	glWeightbvARB((GLint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_weightsvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightsvARB) mogl_glunsupported("glWeightsvARB");
	glWeightsvARB((GLint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_weightivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightivARB) mogl_glunsupported("glWeightivARB");
	glWeightivARB((GLint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_weightfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightfvARB) mogl_glunsupported("glWeightfvARB");
	glWeightfvARB((GLint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_weightdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightdvARB) mogl_glunsupported("glWeightdvARB");
	glWeightdvARB((GLint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_weightubvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightubvARB) mogl_glunsupported("glWeightubvARB");
	glWeightubvARB((GLint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_weightusvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightusvARB) mogl_glunsupported("glWeightusvARB");
	glWeightusvARB((GLint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_weightuivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightuivARB) mogl_glunsupported("glWeightuivARB");
	glWeightuivARB((GLint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_weightpointerarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightPointerARB) mogl_glunsupported("glWeightPointerARB");
	glWeightPointerARB((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_vertexblendarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexBlendARB) mogl_glunsupported("glVertexBlendARB");
	glVertexBlendARB((GLint)mxGetScalar(prhs[0]));

}

void gl_bindbufferarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferARB) mogl_glunsupported("glBindBufferARB");
	glBindBufferARB((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletebuffersarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteBuffersARB) mogl_glunsupported("glDeleteBuffersARB");
	glDeleteBuffersARB((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genbuffersarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenBuffersARB) mogl_glunsupported("glGenBuffersARB");
	glGenBuffersARB((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isbufferarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsBufferARB) mogl_glunsupported("glIsBufferARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsBufferARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_bufferdataarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferDataARB) mogl_glunsupported("glBufferDataARB");
	glBufferDataARB((GLenum)mxGetScalar(prhs[0]),
		(GLsizeiptr)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_buffersubdataarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferSubDataARB) mogl_glunsupported("glBufferSubDataARB");
	glBufferSubDataARB((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_getbuffersubdataarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferSubDataARB) mogl_glunsupported("glGetBufferSubDataARB");
	glGetBufferSubDataARB((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_mapbufferarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapBufferARB) mogl_glunsupported("glMapBufferARB");
	glMapBufferARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_unmapbufferarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapBufferARB) mogl_glunsupported("glUnmapBufferARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glUnmapBufferARB((GLenum)mxGetScalar(prhs[0]));

}

void gl_getbufferparameterivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferParameterivARB) mogl_glunsupported("glGetBufferParameterivARB");
	glGetBufferParameterivARB((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_vertexattrib1darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dARB) mogl_glunsupported("glVertexAttrib1dARB");
	glVertexAttrib1dARB((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dvARB) mogl_glunsupported("glVertexAttrib1dvARB");
	glVertexAttrib1dvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib1farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fARB) mogl_glunsupported("glVertexAttrib1fARB");
	glVertexAttrib1fARB((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fvARB) mogl_glunsupported("glVertexAttrib1fvARB");
	glVertexAttrib1fvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib1sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1sARB) mogl_glunsupported("glVertexAttrib1sARB");
	glVertexAttrib1sARB((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1svARB) mogl_glunsupported("glVertexAttrib1svARB");
	glVertexAttrib1svARB((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib2darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dARB) mogl_glunsupported("glVertexAttrib2dARB");
	glVertexAttrib2dARB((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dvARB) mogl_glunsupported("glVertexAttrib2dvARB");
	glVertexAttrib2dvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib2farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fARB) mogl_glunsupported("glVertexAttrib2fARB");
	glVertexAttrib2fARB((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fvARB) mogl_glunsupported("glVertexAttrib2fvARB");
	glVertexAttrib2fvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib2sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2sARB) mogl_glunsupported("glVertexAttrib2sARB");
	glVertexAttrib2sARB((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2svARB) mogl_glunsupported("glVertexAttrib2svARB");
	glVertexAttrib2svARB((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib3darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dARB) mogl_glunsupported("glVertexAttrib3dARB");
	glVertexAttrib3dARB((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dvARB) mogl_glunsupported("glVertexAttrib3dvARB");
	glVertexAttrib3dvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib3farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fARB) mogl_glunsupported("glVertexAttrib3fARB");
	glVertexAttrib3fARB((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fvARB) mogl_glunsupported("glVertexAttrib3fvARB");
	glVertexAttrib3fvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib3sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3sARB) mogl_glunsupported("glVertexAttrib3sARB");
	glVertexAttrib3sARB((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3svARB) mogl_glunsupported("glVertexAttrib3svARB");
	glVertexAttrib3svARB((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nbvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NbvARB) mogl_glunsupported("glVertexAttrib4NbvARB");
	glVertexAttrib4NbvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NivARB) mogl_glunsupported("glVertexAttrib4NivARB");
	glVertexAttrib4NivARB((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nsvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NsvARB) mogl_glunsupported("glVertexAttrib4NsvARB");
	glVertexAttrib4NsvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nubarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NubARB) mogl_glunsupported("glVertexAttrib4NubARB");
	glVertexAttrib4NubARB((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4nubvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NubvARB) mogl_glunsupported("glVertexAttrib4NubvARB");
	glVertexAttrib4NubvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nuivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NuivARB) mogl_glunsupported("glVertexAttrib4NuivARB");
	glVertexAttrib4NuivARB((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nusvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4NusvARB) mogl_glunsupported("glVertexAttrib4NusvARB");
	glVertexAttrib4NusvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4bvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4bvARB) mogl_glunsupported("glVertexAttrib4bvARB");
	glVertexAttrib4bvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dARB) mogl_glunsupported("glVertexAttrib4dARB");
	glVertexAttrib4dARB((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dvARB) mogl_glunsupported("glVertexAttrib4dvARB");
	glVertexAttrib4dvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fARB) mogl_glunsupported("glVertexAttrib4fARB");
	glVertexAttrib4fARB((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fvARB) mogl_glunsupported("glVertexAttrib4fvARB");
	glVertexAttrib4fvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ivARB) mogl_glunsupported("glVertexAttrib4ivARB");
	glVertexAttrib4ivARB((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4sARB) mogl_glunsupported("glVertexAttrib4sARB");
	glVertexAttrib4sARB((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4svARB) mogl_glunsupported("glVertexAttrib4svARB");
	glVertexAttrib4svARB((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ubvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubvARB) mogl_glunsupported("glVertexAttrib4ubvARB");
	glVertexAttrib4ubvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4uivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4uivARB) mogl_glunsupported("glVertexAttrib4uivARB");
	glVertexAttrib4uivARB((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4usvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4usvARB) mogl_glunsupported("glVertexAttrib4usvARB");
	glVertexAttrib4usvARB((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattribpointerarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribPointerARB) mogl_glunsupported("glVertexAttribPointerARB");
	glVertexAttribPointerARB((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_enablevertexattribarrayarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexAttribArrayARB) mogl_glunsupported("glEnableVertexAttribArrayARB");
	glEnableVertexAttribArrayARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_disablevertexattribarrayarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexAttribArrayARB) mogl_glunsupported("glDisableVertexAttribArrayARB");
	glDisableVertexAttribArrayARB((GLuint)mxGetScalar(prhs[0]));

}

void gl_getvertexattribdvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribdvARB) mogl_glunsupported("glGetVertexAttribdvARB");
	glGetVertexAttribdvARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getvertexattribfvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribfvARB) mogl_glunsupported("glGetVertexAttribfvARB");
	glGetVertexAttribfvARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvertexattribivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribivARB) mogl_glunsupported("glGetVertexAttribivARB");
	glGetVertexAttribivARB((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_bindattriblocationarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindAttribLocationARB) mogl_glunsupported("glBindAttribLocationARB");
	glBindAttribLocationARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLcharARB*)mxGetData(prhs[2]));

}

void gl_getactiveattribarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveAttribARB) mogl_glunsupported("glGetActiveAttribARB");
	glGetActiveAttribARB((GLhandleARB)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLcharARB*)mxGetData(prhs[6]));

}

void gl_getattriblocationarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetAttribLocationARB) mogl_glunsupported("glGetAttribLocationARB");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetAttribLocationARB((GLhandleARB)mxGetScalar(prhs[0]),
		(const GLcharARB*)mxGetData(prhs[1]));

}

void gl_windowpos2darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2dARB) mogl_glunsupported("glWindowPos2dARB");
	glWindowPos2dARB((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_windowpos2dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2dvARB) mogl_glunsupported("glWindowPos2dvARB");
	glWindowPos2dvARB((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos2farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2fARB) mogl_glunsupported("glWindowPos2fARB");
	glWindowPos2fARB((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_windowpos2fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2fvARB) mogl_glunsupported("glWindowPos2fvARB");
	glWindowPos2fvARB((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos2iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2iARB) mogl_glunsupported("glWindowPos2iARB");
	glWindowPos2iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_windowpos2ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2ivARB) mogl_glunsupported("glWindowPos2ivARB");
	glWindowPos2ivARB((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos2sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2sARB) mogl_glunsupported("glWindowPos2sARB");
	glWindowPos2sARB((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_windowpos2svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2svARB) mogl_glunsupported("glWindowPos2svARB");
	glWindowPos2svARB((const GLshort*)mxGetData(prhs[0]));

}

void gl_windowpos3darb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3dARB) mogl_glunsupported("glWindowPos3dARB");
	glWindowPos3dARB((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_windowpos3dvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3dvARB) mogl_glunsupported("glWindowPos3dvARB");
	glWindowPos3dvARB((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos3farb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3fARB) mogl_glunsupported("glWindowPos3fARB");
	glWindowPos3fARB((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_windowpos3fvarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3fvARB) mogl_glunsupported("glWindowPos3fvARB");
	glWindowPos3fvARB((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos3iarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3iARB) mogl_glunsupported("glWindowPos3iARB");
	glWindowPos3iARB((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_windowpos3ivarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3ivARB) mogl_glunsupported("glWindowPos3ivARB");
	glWindowPos3ivARB((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos3sarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3sARB) mogl_glunsupported("glWindowPos3sARB");
	glWindowPos3sARB((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_windowpos3svarb( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3svARB) mogl_glunsupported("glWindowPos3svARB");
	glWindowPos3svARB((const GLshort*)mxGetData(prhs[0]));

}

void gl_blendbarrierkhr( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendBarrierKHR) mogl_glunsupported("glBlendBarrierKHR");
	glBlendBarrierKHR();

}

void gl_cleardepthfoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearDepthfOES) mogl_glunsupported("glClearDepthfOES");
	glClearDepthfOES((GLclampf)mxGetScalar(prhs[0]));

}

void gl_clipplanefoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClipPlanefOES) mogl_glunsupported("glClipPlanefOES");
	glClipPlanefOES((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_depthrangefoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRangefOES) mogl_glunsupported("glDepthRangefOES");
	glDepthRangefOES((GLclampf)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]));

}

void gl_frustumfoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFrustumfOES) mogl_glunsupported("glFrustumfOES");
	glFrustumfOES((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_getclipplanefoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetClipPlanefOES) mogl_glunsupported("glGetClipPlanefOES");
	glGetClipPlanefOES((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_orthofoes( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glOrthofOES) mogl_glunsupported("glOrthofOES");
	glOrthofOES((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_tbuffermask3dfx( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTbufferMask3DFX) mogl_glunsupported("glTbufferMask3DFX");
	glTbufferMask3DFX((GLuint)mxGetScalar(prhs[0]));

}

void gl_debugmessageenableamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageEnableAMD) mogl_glunsupported("glDebugMessageEnableAMD");
	glDebugMessageEnableAMD((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]),
		(GLboolean)mxGetScalar(prhs[4]));

}

void gl_debugmessageinsertamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDebugMessageInsertAMD) mogl_glunsupported("glDebugMessageInsertAMD");
	glDebugMessageInsertAMD((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLchar*)mxGetData(prhs[4]));

}

void gl_getdebugmessagelogamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDebugMessageLogAMD) mogl_glunsupported("glGetDebugMessageLogAMD");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetDebugMessageLogAMD((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum*)mxGetData(prhs[2]),
		(GLuint*)mxGetData(prhs[3]),
		(GLuint*)mxGetData(prhs[4]),
		(GLsizei*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_blendfuncindexedamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncIndexedAMD) mogl_glunsupported("glBlendFuncIndexedAMD");
	glBlendFuncIndexedAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_blendfuncseparateindexedamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncSeparateIndexedAMD) mogl_glunsupported("glBlendFuncSeparateIndexedAMD");
	glBlendFuncSeparateIndexedAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_blendequationindexedamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationIndexedAMD) mogl_glunsupported("glBlendEquationIndexedAMD");
	glBlendEquationIndexedAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendequationseparateindexedamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationSeparateIndexedAMD) mogl_glunsupported("glBlendEquationSeparateIndexedAMD");
	glBlendEquationSeparateIndexedAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_uniform1i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1i64NV) mogl_glunsupported("glUniform1i64NV");
	glUniform1i64NV((GLint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]));

}

void gl_uniform2i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2i64NV) mogl_glunsupported("glUniform2i64NV");
	glUniform2i64NV((GLint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]));

}

void gl_uniform3i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3i64NV) mogl_glunsupported("glUniform3i64NV");
	glUniform3i64NV((GLint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]));

}

void gl_uniform4i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4i64NV) mogl_glunsupported("glUniform4i64NV");
	glUniform4i64NV((GLint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]),
		(GLint64EXT)mxGetScalar(prhs[4]));

}

void gl_uniform1i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1i64vNV) mogl_glunsupported("glUniform1i64vNV");
	glUniform1i64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform2i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2i64vNV) mogl_glunsupported("glUniform2i64vNV");
	glUniform2i64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform3i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3i64vNV) mogl_glunsupported("glUniform3i64vNV");
	glUniform3i64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform4i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4i64vNV) mogl_glunsupported("glUniform4i64vNV");
	glUniform4i64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform1ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1ui64NV) mogl_glunsupported("glUniform1ui64NV");
	glUniform1ui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]));

}

void gl_uniform2ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2ui64NV) mogl_glunsupported("glUniform2ui64NV");
	glUniform2ui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]));

}

void gl_uniform3ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3ui64NV) mogl_glunsupported("glUniform3ui64NV");
	glUniform3ui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]));

}

void gl_uniform4ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4ui64NV) mogl_glunsupported("glUniform4ui64NV");
	glUniform4ui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]),
		(GLuint64EXT)mxGetScalar(prhs[4]));

}

void gl_uniform1ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1ui64vNV) mogl_glunsupported("glUniform1ui64vNV");
	glUniform1ui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform2ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2ui64vNV) mogl_glunsupported("glUniform2ui64vNV");
	glUniform2ui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform3ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3ui64vNV) mogl_glunsupported("glUniform3ui64vNV");
	glUniform3ui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_uniform4ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4ui64vNV) mogl_glunsupported("glUniform4ui64vNV");
	glUniform4ui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_getuniformi64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformi64vNV) mogl_glunsupported("glGetUniformi64vNV");
	glGetUniformi64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint64EXT*)mxGetData(prhs[2]));

}

void gl_getuniformui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformui64vNV) mogl_glunsupported("glGetUniformui64vNV");
	glGetUniformui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_programuniform1i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1i64NV) mogl_glunsupported("glProgramUniform1i64NV");
	glProgramUniform1i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]));

}

void gl_programuniform2i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2i64NV) mogl_glunsupported("glProgramUniform2i64NV");
	glProgramUniform2i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]));

}

void gl_programuniform3i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3i64NV) mogl_glunsupported("glProgramUniform3i64NV");
	glProgramUniform3i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]),
		(GLint64EXT)mxGetScalar(prhs[4]));

}

void gl_programuniform4i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4i64NV) mogl_glunsupported("glProgramUniform4i64NV");
	glProgramUniform4i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]),
		(GLint64EXT)mxGetScalar(prhs[4]),
		(GLint64EXT)mxGetScalar(prhs[5]));

}

void gl_programuniform1i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1i64vNV) mogl_glunsupported("glProgramUniform1i64vNV");
	glProgramUniform1i64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform2i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2i64vNV) mogl_glunsupported("glProgramUniform2i64vNV");
	glProgramUniform2i64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform3i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3i64vNV) mogl_glunsupported("glProgramUniform3i64vNV");
	glProgramUniform3i64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform4i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4i64vNV) mogl_glunsupported("glProgramUniform4i64vNV");
	glProgramUniform4i64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform1ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1ui64NV) mogl_glunsupported("glProgramUniform1ui64NV");
	glProgramUniform1ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]));

}

void gl_programuniform2ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2ui64NV) mogl_glunsupported("glProgramUniform2ui64NV");
	glProgramUniform2ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]));

}

void gl_programuniform3ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3ui64NV) mogl_glunsupported("glProgramUniform3ui64NV");
	glProgramUniform3ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]),
		(GLuint64EXT)mxGetScalar(prhs[4]));

}

void gl_programuniform4ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4ui64NV) mogl_glunsupported("glProgramUniform4ui64NV");
	glProgramUniform4ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]),
		(GLuint64EXT)mxGetScalar(prhs[4]),
		(GLuint64EXT)mxGetScalar(prhs[5]));

}

void gl_programuniform1ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1ui64vNV) mogl_glunsupported("glProgramUniform1ui64vNV");
	glProgramUniform1ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform2ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2ui64vNV) mogl_glunsupported("glProgramUniform2ui64vNV");
	glProgramUniform2ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform3ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3ui64vNV) mogl_glunsupported("glProgramUniform3ui64vNV");
	glProgramUniform3ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64EXT*)mxGetData(prhs[3]));

}

void gl_programuniform4ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4ui64vNV) mogl_glunsupported("glProgramUniform4ui64vNV");
	glProgramUniform4ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64EXT*)mxGetData(prhs[3]));

}

void gl_vertexattribparameteriamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribParameteriAMD) mogl_glunsupported("glVertexAttribParameteriAMD");
	glVertexAttribParameteriAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_multidrawarraysindirectamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysIndirectAMD) mogl_glunsupported("glMultiDrawArraysIndirectAMD");
	glMultiDrawArraysIndirectAMD((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_multidrawelementsindirectamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsIndirectAMD) mogl_glunsupported("glMultiDrawElementsIndirectAMD");
	glMultiDrawElementsIndirectAMD((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_gennamesamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenNamesAMD) mogl_glunsupported("glGenNamesAMD");
	glGenNamesAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_deletenamesamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteNamesAMD) mogl_glunsupported("glDeleteNamesAMD");
	glDeleteNamesAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_isnameamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsNameAMD) mogl_glunsupported("glIsNameAMD");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsNameAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_queryobjectparameteruiamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glQueryObjectParameteruiAMD) mogl_glunsupported("glQueryObjectParameteruiAMD");
	glQueryObjectParameteruiAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_getperfmonitorgroupsamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorGroupsAMD) mogl_glunsupported("glGetPerfMonitorGroupsAMD");
	glGetPerfMonitorGroupsAMD((GLint*)mxGetData(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_getperfmonitorcountersamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorCountersAMD) mogl_glunsupported("glGetPerfMonitorCountersAMD");
	glGetPerfMonitorCountersAMD((GLuint)mxGetScalar(prhs[0]),
		(GLint*)mxGetData(prhs[1]),
		(GLint*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLuint*)mxGetData(prhs[4]));

}

void gl_getperfmonitorgroupstringamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorGroupStringAMD) mogl_glunsupported("glGetPerfMonitorGroupStringAMD");
	glGetPerfMonitorGroupStringAMD((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_getperfmonitorcounterstringamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorCounterStringAMD) mogl_glunsupported("glGetPerfMonitorCounterStringAMD");
	glGetPerfMonitorCounterStringAMD((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_getperfmonitorcounterinfoamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorCounterInfoAMD) mogl_glunsupported("glGetPerfMonitorCounterInfoAMD");
	glGetPerfMonitorCounterInfoAMD((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_genperfmonitorsamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenPerfMonitorsAMD) mogl_glunsupported("glGenPerfMonitorsAMD");
	glGenPerfMonitorsAMD((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deleteperfmonitorsamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeletePerfMonitorsAMD) mogl_glunsupported("glDeletePerfMonitorsAMD");
	glDeletePerfMonitorsAMD((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_selectperfmonitorcountersamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSelectPerfMonitorCountersAMD) mogl_glunsupported("glSelectPerfMonitorCountersAMD");
	glSelectPerfMonitorCountersAMD((GLuint)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLuint*)mxGetData(prhs[4]));

}

void gl_beginperfmonitoramd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginPerfMonitorAMD) mogl_glunsupported("glBeginPerfMonitorAMD");
	glBeginPerfMonitorAMD((GLuint)mxGetScalar(prhs[0]));

}

void gl_endperfmonitoramd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndPerfMonitorAMD) mogl_glunsupported("glEndPerfMonitorAMD");
	glEndPerfMonitorAMD((GLuint)mxGetScalar(prhs[0]));

}

void gl_getperfmonitorcounterdataamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfMonitorCounterDataAMD) mogl_glunsupported("glGetPerfMonitorCounterDataAMD");
	glGetPerfMonitorCounterDataAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_setmultisamplefvamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetMultisamplefvAMD) mogl_glunsupported("glSetMultisamplefvAMD");
	glSetMultisamplefvAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texstoragesparseamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexStorageSparseAMD) mogl_glunsupported("glTexStorageSparseAMD");
	glTexStorageSparseAMD((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLbitfield)mxGetScalar(prhs[6]));

}

void gl_texturestoragesparseamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorageSparseAMD) mogl_glunsupported("glTextureStorageSparseAMD");
	glTextureStorageSparseAMD((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLbitfield)mxGetScalar(prhs[7]));

}

void gl_stencilopvalueamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilOpValueAMD) mogl_glunsupported("glStencilOpValueAMD");
	glStencilOpValueAMD((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_tessellationfactoramd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTessellationFactorAMD) mogl_glunsupported("glTessellationFactorAMD");
	glTessellationFactorAMD((GLfloat)mxGetScalar(prhs[0]));

}

void gl_tessellationmodeamd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTessellationModeAMD) mogl_glunsupported("glTessellationModeAMD");
	glTessellationModeAMD((GLenum)mxGetScalar(prhs[0]));

}

void gl_elementpointerapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glElementPointerAPPLE) mogl_glunsupported("glElementPointerAPPLE");
	glElementPointerAPPLE((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_drawelementarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementArrayAPPLE) mogl_glunsupported("glDrawElementArrayAPPLE");
	glDrawElementArrayAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_drawrangeelementarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawRangeElementArrayAPPLE) mogl_glunsupported("glDrawRangeElementArrayAPPLE");
	glDrawRangeElementArrayAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_multidrawelementarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementArrayAPPLE) mogl_glunsupported("glMultiDrawElementArrayAPPLE");
	glMultiDrawElementArrayAPPLE((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(const GLsizei*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_multidrawrangeelementarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawRangeElementArrayAPPLE) mogl_glunsupported("glMultiDrawRangeElementArrayAPPLE");
	glMultiDrawRangeElementArrayAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]),
		(const GLsizei*)mxGetData(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_genfencesapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenFencesAPPLE) mogl_glunsupported("glGenFencesAPPLE");
	glGenFencesAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deletefencesapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteFencesAPPLE) mogl_glunsupported("glDeleteFencesAPPLE");
	glDeleteFencesAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_setfenceapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetFenceAPPLE) mogl_glunsupported("glSetFenceAPPLE");
	glSetFenceAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_isfenceapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsFenceAPPLE) mogl_glunsupported("glIsFenceAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsFenceAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_testfenceapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTestFenceAPPLE) mogl_glunsupported("glTestFenceAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glTestFenceAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_finishfenceapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinishFenceAPPLE) mogl_glunsupported("glFinishFenceAPPLE");
	glFinishFenceAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_testobjectapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTestObjectAPPLE) mogl_glunsupported("glTestObjectAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glTestObjectAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_finishobjectapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinishObjectAPPLE) mogl_glunsupported("glFinishObjectAPPLE");
	glFinishObjectAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_bufferparameteriapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferParameteriAPPLE) mogl_glunsupported("glBufferParameteriAPPLE");
	glBufferParameteriAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_flushmappedbufferrangeapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushMappedBufferRangeAPPLE) mogl_glunsupported("glFlushMappedBufferRangeAPPLE");
	glFlushMappedBufferRangeAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]));

}

void gl_objectpurgeableapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glObjectPurgeableAPPLE) mogl_glunsupported("glObjectPurgeableAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glObjectPurgeableAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_objectunpurgeableapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glObjectUnpurgeableAPPLE) mogl_glunsupported("glObjectUnpurgeableAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glObjectUnpurgeableAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_getobjectparameterivapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectParameterivAPPLE) mogl_glunsupported("glGetObjectParameterivAPPLE");
	glGetObjectParameterivAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_texturerangeapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureRangeAPPLE) mogl_glunsupported("glTextureRangeAPPLE");
	glTextureRangeAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_bindvertexarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVertexArrayAPPLE) mogl_glunsupported("glBindVertexArrayAPPLE");
	glBindVertexArrayAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_deletevertexarraysapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteVertexArraysAPPLE) mogl_glunsupported("glDeleteVertexArraysAPPLE");
	glDeleteVertexArraysAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genvertexarraysapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenVertexArraysAPPLE) mogl_glunsupported("glGenVertexArraysAPPLE");
	glGenVertexArraysAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isvertexarrayapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsVertexArrayAPPLE) mogl_glunsupported("glIsVertexArrayAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsVertexArrayAPPLE((GLuint)mxGetScalar(prhs[0]));

}

void gl_vertexarrayrangeapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayRangeAPPLE) mogl_glunsupported("glVertexArrayRangeAPPLE");
	glVertexArrayRangeAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(void*)mxGetData(prhs[1]));

}

void gl_flushvertexarrayrangeapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushVertexArrayRangeAPPLE) mogl_glunsupported("glFlushVertexArrayRangeAPPLE");
	glFlushVertexArrayRangeAPPLE((GLsizei)mxGetScalar(prhs[0]),
		(void*)mxGetData(prhs[1]));

}

void gl_vertexarrayparameteriapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayParameteriAPPLE) mogl_glunsupported("glVertexArrayParameteriAPPLE");
	glVertexArrayParameteriAPPLE((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_enablevertexattribapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexAttribAPPLE) mogl_glunsupported("glEnableVertexAttribAPPLE");
	glEnableVertexAttribAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_disablevertexattribapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexAttribAPPLE) mogl_glunsupported("glDisableVertexAttribAPPLE");
	glDisableVertexAttribAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_isvertexattribenabledapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsVertexAttribEnabledAPPLE) mogl_glunsupported("glIsVertexAttribEnabledAPPLE");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsVertexAttribEnabledAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_mapvertexattrib1dapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapVertexAttrib1dAPPLE) mogl_glunsupported("glMapVertexAttrib1dAPPLE");
	glMapVertexAttrib1dAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(const GLdouble*)mxGetData(prhs[6]));

}

void gl_mapvertexattrib1fapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapVertexAttrib1fAPPLE) mogl_glunsupported("glMapVertexAttrib1fAPPLE");
	glMapVertexAttrib1fAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(const GLfloat*)mxGetData(prhs[6]));

}

void gl_mapvertexattrib2dapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapVertexAttrib2dAPPLE) mogl_glunsupported("glMapVertexAttrib2dAPPLE");
	glMapVertexAttrib2dAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]),
		(GLdouble)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(const GLdouble*)mxGetData(prhs[10]));

}

void gl_mapvertexattrib2fapple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapVertexAttrib2fAPPLE) mogl_glunsupported("glMapVertexAttrib2fAPPLE");
	glMapVertexAttrib2fAPPLE((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(const GLfloat*)mxGetData(prhs[10]));

}

void gl_drawbuffersati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawBuffersATI) mogl_glunsupported("glDrawBuffersATI");
	glDrawBuffersATI((GLsizei)mxGetScalar(prhs[0]),
		(const GLenum*)mxGetData(prhs[1]));

}

void gl_elementpointerati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glElementPointerATI) mogl_glunsupported("glElementPointerATI");
	glElementPointerATI((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_drawelementarrayati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementArrayATI) mogl_glunsupported("glDrawElementArrayATI");
	glDrawElementArrayATI((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_drawrangeelementarrayati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawRangeElementArrayATI) mogl_glunsupported("glDrawRangeElementArrayATI");
	glDrawRangeElementArrayATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_texbumpparameterivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBumpParameterivATI) mogl_glunsupported("glTexBumpParameterivATI");
	glTexBumpParameterivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_texbumpparameterfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBumpParameterfvATI) mogl_glunsupported("glTexBumpParameterfvATI");
	glTexBumpParameterfvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_gettexbumpparameterivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexBumpParameterivATI) mogl_glunsupported("glGetTexBumpParameterivATI");
	glGetTexBumpParameterivATI((GLenum)mxGetScalar(prhs[0]),
		(GLint*)mxGetData(prhs[1]));

}

void gl_gettexbumpparameterfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexBumpParameterfvATI) mogl_glunsupported("glGetTexBumpParameterfvATI");
	glGetTexBumpParameterfvATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_genfragmentshadersati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenFragmentShadersATI) mogl_glunsupported("glGenFragmentShadersATI");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenFragmentShadersATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindfragmentshaderati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFragmentShaderATI) mogl_glunsupported("glBindFragmentShaderATI");
	glBindFragmentShaderATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_deletefragmentshaderati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteFragmentShaderATI) mogl_glunsupported("glDeleteFragmentShaderATI");
	glDeleteFragmentShaderATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_beginfragmentshaderati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginFragmentShaderATI) mogl_glunsupported("glBeginFragmentShaderATI");
	glBeginFragmentShaderATI();

}

void gl_endfragmentshaderati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndFragmentShaderATI) mogl_glunsupported("glEndFragmentShaderATI");
	glEndFragmentShaderATI();

}

void gl_passtexcoordati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPassTexCoordATI) mogl_glunsupported("glPassTexCoordATI");
	glPassTexCoordATI((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_samplemapati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMapATI) mogl_glunsupported("glSampleMapATI");
	glSampleMapATI((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_colorfragmentop1ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorFragmentOp1ATI) mogl_glunsupported("glColorFragmentOp1ATI");
	glColorFragmentOp1ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]));

}

void gl_colorfragmentop2ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorFragmentOp2ATI) mogl_glunsupported("glColorFragmentOp2ATI");
	glColorFragmentOp2ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]),
		(GLuint)mxGetScalar(prhs[9]));

}

void gl_colorfragmentop3ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorFragmentOp3ATI) mogl_glunsupported("glColorFragmentOp3ATI");
	glColorFragmentOp3ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]),
		(GLuint)mxGetScalar(prhs[9]),
		(GLuint)mxGetScalar(prhs[10]),
		(GLuint)mxGetScalar(prhs[11]),
		(GLuint)mxGetScalar(prhs[12]));

}

void gl_alphafragmentop1ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAlphaFragmentOp1ATI) mogl_glunsupported("glAlphaFragmentOp1ATI");
	glAlphaFragmentOp1ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_alphafragmentop2ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAlphaFragmentOp2ATI) mogl_glunsupported("glAlphaFragmentOp2ATI");
	glAlphaFragmentOp2ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]));

}

void gl_alphafragmentop3ati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAlphaFragmentOp3ATI) mogl_glunsupported("glAlphaFragmentOp3ATI");
	glAlphaFragmentOp3ATI((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]),
		(GLuint)mxGetScalar(prhs[9]),
		(GLuint)mxGetScalar(prhs[10]),
		(GLuint)mxGetScalar(prhs[11]));

}

void gl_setfragmentshaderconstantati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetFragmentShaderConstantATI) mogl_glunsupported("glSetFragmentShaderConstantATI");
	glSetFragmentShaderConstantATI((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_mapobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapObjectBufferATI) mogl_glunsupported("glMapObjectBufferATI");
	glMapObjectBufferATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_unmapobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapObjectBufferATI) mogl_glunsupported("glUnmapObjectBufferATI");
	glUnmapObjectBufferATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_pntrianglesiati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPNTrianglesiATI) mogl_glunsupported("glPNTrianglesiATI");
	glPNTrianglesiATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pntrianglesfati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPNTrianglesfATI) mogl_glunsupported("glPNTrianglesfATI");
	glPNTrianglesfATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_stencilopseparateati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilOpSeparateATI) mogl_glunsupported("glStencilOpSeparateATI");
	glStencilOpSeparateATI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_stencilfuncseparateati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilFuncSeparateATI) mogl_glunsupported("glStencilFuncSeparateATI");
	glStencilFuncSeparateATI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_newobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNewObjectBufferATI) mogl_glunsupported("glNewObjectBufferATI");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glNewObjectBufferATI((GLsizei)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_isobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsObjectBufferATI) mogl_glunsupported("glIsObjectBufferATI");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsObjectBufferATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_updateobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUpdateObjectBufferATI) mogl_glunsupported("glUpdateObjectBufferATI");
	glUpdateObjectBufferATI((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_getobjectbufferfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectBufferfvATI) mogl_glunsupported("glGetObjectBufferfvATI");
	glGetObjectBufferfvATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getobjectbufferivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectBufferivATI) mogl_glunsupported("glGetObjectBufferivATI");
	glGetObjectBufferivATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_freeobjectbufferati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFreeObjectBufferATI) mogl_glunsupported("glFreeObjectBufferATI");
	glFreeObjectBufferATI((GLuint)mxGetScalar(prhs[0]));

}

void gl_arrayobjectati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glArrayObjectATI) mogl_glunsupported("glArrayObjectATI");
	glArrayObjectATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_getarrayobjectfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetArrayObjectfvATI) mogl_glunsupported("glGetArrayObjectfvATI");
	glGetArrayObjectfvATI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getarrayobjectivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetArrayObjectivATI) mogl_glunsupported("glGetArrayObjectivATI");
	glGetArrayObjectivATI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_variantarrayobjectati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantArrayObjectATI) mogl_glunsupported("glVariantArrayObjectATI");
	glVariantArrayObjectATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_getvariantarrayobjectfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVariantArrayObjectfvATI) mogl_glunsupported("glGetVariantArrayObjectfvATI");
	glGetVariantArrayObjectfvATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvariantarrayobjectivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVariantArrayObjectivATI) mogl_glunsupported("glGetVariantArrayObjectivATI");
	glGetVariantArrayObjectivATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_vertexattribarrayobjectati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribArrayObjectATI) mogl_glunsupported("glVertexAttribArrayObjectATI");
	glVertexAttribArrayObjectATI((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]));

}

void gl_getvertexattribarrayobjectfvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribArrayObjectfvATI) mogl_glunsupported("glGetVertexAttribArrayObjectfvATI");
	glGetVertexAttribArrayObjectfvATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvertexattribarrayobjectivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribArrayObjectivATI) mogl_glunsupported("glGetVertexAttribArrayObjectivATI");
	glGetVertexAttribArrayObjectivATI((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_vertexstream1sati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1sATI) mogl_glunsupported("glVertexStream1sATI");
	glVertexStream1sATI((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexstream1svati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1svATI) mogl_glunsupported("glVertexStream1svATI");
	glVertexStream1svATI((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexstream1iati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1iATI) mogl_glunsupported("glVertexStream1iATI");
	glVertexStream1iATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertexstream1ivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1ivATI) mogl_glunsupported("glVertexStream1ivATI");
	glVertexStream1ivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexstream1fati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1fATI) mogl_glunsupported("glVertexStream1fATI");
	glVertexStream1fATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexstream1fvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1fvATI) mogl_glunsupported("glVertexStream1fvATI");
	glVertexStream1fvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexstream1dati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1dATI) mogl_glunsupported("glVertexStream1dATI");
	glVertexStream1dATI((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexstream1dvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream1dvATI) mogl_glunsupported("glVertexStream1dvATI");
	glVertexStream1dvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexstream2sati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2sATI) mogl_glunsupported("glVertexStream2sATI");
	glVertexStream2sATI((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexstream2svati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2svATI) mogl_glunsupported("glVertexStream2svATI");
	glVertexStream2svATI((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexstream2iati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2iATI) mogl_glunsupported("glVertexStream2iATI");
	glVertexStream2iATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_vertexstream2ivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2ivATI) mogl_glunsupported("glVertexStream2ivATI");
	glVertexStream2ivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexstream2fati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2fATI) mogl_glunsupported("glVertexStream2fATI");
	glVertexStream2fATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexstream2fvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2fvATI) mogl_glunsupported("glVertexStream2fvATI");
	glVertexStream2fvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexstream2dati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2dATI) mogl_glunsupported("glVertexStream2dATI");
	glVertexStream2dATI((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexstream2dvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream2dvATI) mogl_glunsupported("glVertexStream2dvATI");
	glVertexStream2dvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexstream3sati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3sATI) mogl_glunsupported("glVertexStream3sATI");
	glVertexStream3sATI((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexstream3svati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3svATI) mogl_glunsupported("glVertexStream3svATI");
	glVertexStream3svATI((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexstream3iati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3iATI) mogl_glunsupported("glVertexStream3iATI");
	glVertexStream3iATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_vertexstream3ivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3ivATI) mogl_glunsupported("glVertexStream3ivATI");
	glVertexStream3ivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexstream3fati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3fATI) mogl_glunsupported("glVertexStream3fATI");
	glVertexStream3fATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexstream3fvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3fvATI) mogl_glunsupported("glVertexStream3fvATI");
	glVertexStream3fvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexstream3dati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3dATI) mogl_glunsupported("glVertexStream3dATI");
	glVertexStream3dATI((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexstream3dvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream3dvATI) mogl_glunsupported("glVertexStream3dvATI");
	glVertexStream3dvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexstream4sati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4sATI) mogl_glunsupported("glVertexStream4sATI");
	glVertexStream4sATI((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexstream4svati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4svATI) mogl_glunsupported("glVertexStream4svATI");
	glVertexStream4svATI((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexstream4iati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4iATI) mogl_glunsupported("glVertexStream4iATI");
	glVertexStream4iATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_vertexstream4ivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4ivATI) mogl_glunsupported("glVertexStream4ivATI");
	glVertexStream4ivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexstream4fati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4fATI) mogl_glunsupported("glVertexStream4fATI");
	glVertexStream4fATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexstream4fvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4fvATI) mogl_glunsupported("glVertexStream4fvATI");
	glVertexStream4fvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexstream4dati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4dATI) mogl_glunsupported("glVertexStream4dATI");
	glVertexStream4dATI((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexstream4dvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexStream4dvATI) mogl_glunsupported("glVertexStream4dvATI");
	glVertexStream4dvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_normalstream3bati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3bATI) mogl_glunsupported("glNormalStream3bATI");
	glNormalStream3bATI((GLenum)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]),
		(GLbyte)mxGetScalar(prhs[3]));

}

void gl_normalstream3bvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3bvATI) mogl_glunsupported("glNormalStream3bvATI");
	glNormalStream3bvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_normalstream3sati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3sATI) mogl_glunsupported("glNormalStream3sATI");
	glNormalStream3sATI((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_normalstream3svati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3svATI) mogl_glunsupported("glNormalStream3svATI");
	glNormalStream3svATI((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_normalstream3iati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3iATI) mogl_glunsupported("glNormalStream3iATI");
	glNormalStream3iATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_normalstream3ivati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3ivATI) mogl_glunsupported("glNormalStream3ivATI");
	glNormalStream3ivATI((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_normalstream3fati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3fATI) mogl_glunsupported("glNormalStream3fATI");
	glNormalStream3fATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_normalstream3fvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3fvATI) mogl_glunsupported("glNormalStream3fvATI");
	glNormalStream3fvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_normalstream3dati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3dATI) mogl_glunsupported("glNormalStream3dATI");
	glNormalStream3dATI((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_normalstream3dvati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalStream3dvATI) mogl_glunsupported("glNormalStream3dvATI");
	glNormalStream3dvATI((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_clientactivevertexstreamati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClientActiveVertexStreamATI) mogl_glunsupported("glClientActiveVertexStreamATI");
	glClientActiveVertexStreamATI((GLenum)mxGetScalar(prhs[0]));

}

void gl_vertexblendenviati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexBlendEnviATI) mogl_glunsupported("glVertexBlendEnviATI");
	glVertexBlendEnviATI((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertexblendenvfati( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexBlendEnvfATI) mogl_glunsupported("glVertexBlendEnvfATI");
	glVertexBlendEnvfATI((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_uniformbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformBufferEXT) mogl_glunsupported("glUniformBufferEXT");
	glUniformBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_getuniformbuffersizeext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformBufferSizeEXT) mogl_glunsupported("glGetUniformBufferSizeEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformBufferSizeEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_getuniformoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformOffsetEXT) mogl_glunsupported("glGetUniformOffsetEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_blendcolorext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendColorEXT) mogl_glunsupported("glBlendColorEXT");
	glBlendColorEXT((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_blendequationseparateext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationSeparateEXT) mogl_glunsupported("glBlendEquationSeparateEXT");
	glBlendEquationSeparateEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendfuncseparateext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendFuncSeparateEXT) mogl_glunsupported("glBlendFuncSeparateEXT");
	glBlendFuncSeparateEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_blendequationext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendEquationEXT) mogl_glunsupported("glBlendEquationEXT");
	glBlendEquationEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_colorsubtableext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorSubTableEXT) mogl_glunsupported("glColorSubTableEXT");
	glColorSubTableEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_copycolorsubtableext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyColorSubTableEXT) mogl_glunsupported("glCopyColorSubTableEXT");
	glCopyColorSubTableEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_lockarraysext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLockArraysEXT) mogl_glunsupported("glLockArraysEXT");
	glLockArraysEXT((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_unlockarraysext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnlockArraysEXT) mogl_glunsupported("glUnlockArraysEXT");
	glUnlockArraysEXT();

}

void gl_convolutionfilter1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionFilter1DEXT) mogl_glunsupported("glConvolutionFilter1DEXT");
	glConvolutionFilter1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_convolutionfilter2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionFilter2DEXT) mogl_glunsupported("glConvolutionFilter2DEXT");
	glConvolutionFilter2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_convolutionparameterfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameterfEXT) mogl_glunsupported("glConvolutionParameterfEXT");
	glConvolutionParameterfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_convolutionparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameterfvEXT) mogl_glunsupported("glConvolutionParameterfvEXT");
	glConvolutionParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_convolutionparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameteriEXT) mogl_glunsupported("glConvolutionParameteriEXT");
	glConvolutionParameteriEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_convolutionparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glConvolutionParameterivEXT) mogl_glunsupported("glConvolutionParameterivEXT");
	glConvolutionParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_copyconvolutionfilter1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyConvolutionFilter1DEXT) mogl_glunsupported("glCopyConvolutionFilter1DEXT");
	glCopyConvolutionFilter1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copyconvolutionfilter2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyConvolutionFilter2DEXT) mogl_glunsupported("glCopyConvolutionFilter2DEXT");
	glCopyConvolutionFilter2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_getconvolutionfilterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionFilterEXT) mogl_glunsupported("glGetConvolutionFilterEXT");
	glGetConvolutionFilterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_getconvolutionparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionParameterfvEXT) mogl_glunsupported("glGetConvolutionParameterfvEXT");
	glGetConvolutionParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getconvolutionparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionParameterivEXT) mogl_glunsupported("glGetConvolutionParameterivEXT");
	glGetConvolutionParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getseparablefilterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSeparableFilterEXT) mogl_glunsupported("glGetSeparableFilterEXT");
	glGetSeparableFilterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]),
		(void*)mxGetData(prhs[4]),
		(void*)mxGetData(prhs[5]));

}

void gl_separablefilter2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSeparableFilter2DEXT) mogl_glunsupported("glSeparableFilter2DEXT");
	glSeparableFilter2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_tangentpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTangentPointerEXT) mogl_glunsupported("glTangentPointerEXT");
	glTangentPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_binormalpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBinormalPointerEXT) mogl_glunsupported("glBinormalPointerEXT");
	glBinormalPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_copyteximage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexImage1DEXT) mogl_glunsupported("glCopyTexImage1DEXT");
	glCopyTexImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_copyteximage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexImage2DEXT) mogl_glunsupported("glCopyTexImage2DEXT");
	glCopyTexImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]));

}

void gl_copytexsubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage1DEXT) mogl_glunsupported("glCopyTexSubImage1DEXT");
	glCopyTexSubImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copytexsubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage2DEXT) mogl_glunsupported("glCopyTexSubImage2DEXT");
	glCopyTexSubImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]));

}

void gl_copytexsubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTexSubImage3DEXT) mogl_glunsupported("glCopyTexSubImage3DEXT");
	glCopyTexSubImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]));

}

void gl_cullparameterdvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCullParameterdvEXT) mogl_glunsupported("glCullParameterdvEXT");
	glCullParameterdvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_cullparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCullParameterfvEXT) mogl_glunsupported("glCullParameterfvEXT");
	glCullParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_labelobjectext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLabelObjectEXT) mogl_glunsupported("glLabelObjectEXT");
	glLabelObjectEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLchar*)mxGetData(prhs[3]));

}

void gl_getobjectlabelext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetObjectLabelEXT) mogl_glunsupported("glGetObjectLabelEXT");
	glGetObjectLabelEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLchar*)mxGetData(prhs[4]));

}

void gl_inserteventmarkerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInsertEventMarkerEXT) mogl_glunsupported("glInsertEventMarkerEXT");
	glInsertEventMarkerEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_pushgroupmarkerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushGroupMarkerEXT) mogl_glunsupported("glPushGroupMarkerEXT");
	glPushGroupMarkerEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_popgroupmarkerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPopGroupMarkerEXT) mogl_glunsupported("glPopGroupMarkerEXT");
	glPopGroupMarkerEXT();

}

void gl_depthboundsext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthBoundsEXT) mogl_glunsupported("glDepthBoundsEXT");
	glDepthBoundsEXT((GLclampd)mxGetScalar(prhs[0]),
		(GLclampd)mxGetScalar(prhs[1]));

}

void gl_matrixloadfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixLoadfEXT) mogl_glunsupported("glMatrixLoadfEXT");
	glMatrixLoadfEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_matrixloaddext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixLoaddEXT) mogl_glunsupported("glMatrixLoaddEXT");
	glMatrixLoaddEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_matrixmultfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixMultfEXT) mogl_glunsupported("glMatrixMultfEXT");
	glMatrixMultfEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_matrixmultdext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixMultdEXT) mogl_glunsupported("glMatrixMultdEXT");
	glMatrixMultdEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_matrixloadidentityext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixLoadIdentityEXT) mogl_glunsupported("glMatrixLoadIdentityEXT");
	glMatrixLoadIdentityEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_matrixrotatefext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixRotatefEXT) mogl_glunsupported("glMatrixRotatefEXT");
	glMatrixRotatefEXT((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_matrixrotatedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixRotatedEXT) mogl_glunsupported("glMatrixRotatedEXT");
	glMatrixRotatedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_matrixscalefext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixScalefEXT) mogl_glunsupported("glMatrixScalefEXT");
	glMatrixScalefEXT((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_matrixscaledext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixScaledEXT) mogl_glunsupported("glMatrixScaledEXT");
	glMatrixScaledEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_matrixtranslatefext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixTranslatefEXT) mogl_glunsupported("glMatrixTranslatefEXT");
	glMatrixTranslatefEXT((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_matrixtranslatedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixTranslatedEXT) mogl_glunsupported("glMatrixTranslatedEXT");
	glMatrixTranslatedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_matrixfrustumext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixFrustumEXT) mogl_glunsupported("glMatrixFrustumEXT");
	glMatrixFrustumEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void gl_matrixorthoext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixOrthoEXT) mogl_glunsupported("glMatrixOrthoEXT");
	glMatrixOrthoEXT((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void gl_matrixpopext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixPopEXT) mogl_glunsupported("glMatrixPopEXT");
	glMatrixPopEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_matrixpushext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixPushEXT) mogl_glunsupported("glMatrixPushEXT");
	glMatrixPushEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_clientattribdefaultext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClientAttribDefaultEXT) mogl_glunsupported("glClientAttribDefaultEXT");
	glClientAttribDefaultEXT((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_pushclientattribdefaultext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPushClientAttribDefaultEXT) mogl_glunsupported("glPushClientAttribDefaultEXT");
	glPushClientAttribDefaultEXT((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_textureparameterfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterfEXT) mogl_glunsupported("glTextureParameterfEXT");
	glTextureParameterfEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_textureparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterfvEXT) mogl_glunsupported("glTextureParameterfvEXT");
	glTextureParameterfvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_textureparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameteriEXT) mogl_glunsupported("glTextureParameteriEXT");
	glTextureParameteriEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_textureparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterivEXT) mogl_glunsupported("glTextureParameterivEXT");
	glTextureParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_textureimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage1DEXT) mogl_glunsupported("glTextureImage1DEXT");
	glTextureImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_textureimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage2DEXT) mogl_glunsupported("glTextureImage2DEXT");
	glTextureImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_texturesubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage1DEXT) mogl_glunsupported("glTextureSubImage1DEXT");
	glTextureSubImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_texturesubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage2DEXT) mogl_glunsupported("glTextureSubImage2DEXT");
	glTextureSubImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_copytextureimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureImage1DEXT) mogl_glunsupported("glCopyTextureImage1DEXT");
	glCopyTextureImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]));

}

void gl_copytextureimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureImage2DEXT) mogl_glunsupported("glCopyTextureImage2DEXT");
	glCopyTextureImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]));

}

void gl_copytexturesubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage1DEXT) mogl_glunsupported("glCopyTextureSubImage1DEXT");
	glCopyTextureSubImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]));

}

void gl_copytexturesubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage2DEXT) mogl_glunsupported("glCopyTextureSubImage2DEXT");
	glCopyTextureSubImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]));

}

void gl_gettextureimageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureImageEXT) mogl_glunsupported("glGetTextureImageEXT");
	glGetTextureImageEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(void*)mxGetData(prhs[5]));

}

void gl_gettextureparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterfvEXT) mogl_glunsupported("glGetTextureParameterfvEXT");
	glGetTextureParameterfvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_gettextureparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterivEXT) mogl_glunsupported("glGetTextureParameterivEXT");
	glGetTextureParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettexturelevelparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureLevelParameterfvEXT) mogl_glunsupported("glGetTextureLevelParameterfvEXT");
	glGetTextureLevelParameterfvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]));

}

void gl_gettexturelevelparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureLevelParameterivEXT) mogl_glunsupported("glGetTextureLevelParameterivEXT");
	glGetTextureLevelParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_textureimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage3DEXT) mogl_glunsupported("glTextureImage3DEXT");
	glTextureImage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_texturesubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureSubImage3DEXT) mogl_glunsupported("glTextureSubImage3DEXT");
	glTextureSubImage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLenum)mxGetScalar(prhs[10]),
		(const void*)mxGetData(prhs[11]));

}

void gl_copytexturesubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyTextureSubImage3DEXT) mogl_glunsupported("glCopyTextureSubImage3DEXT");
	glCopyTextureSubImage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]));

}

void gl_bindmultitextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindMultiTextureEXT) mogl_glunsupported("glBindMultiTextureEXT");
	glBindMultiTextureEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexcoordpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexCoordPointerEXT) mogl_glunsupported("glMultiTexCoordPointerEXT");
	glMultiTexCoordPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_multitexenvfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexEnvfEXT) mogl_glunsupported("glMultiTexEnvfEXT");
	glMultiTexEnvfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexenvfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexEnvfvEXT) mogl_glunsupported("glMultiTexEnvfvEXT");
	glMultiTexEnvfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_multitexenviext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexEnviEXT) mogl_glunsupported("glMultiTexEnviEXT");
	glMultiTexEnviEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexenvivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexEnvivEXT) mogl_glunsupported("glMultiTexEnvivEXT");
	glMultiTexEnvivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_multitexgendext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGendEXT) mogl_glunsupported("glMultiTexGendEXT");
	glMultiTexGendEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_multitexgendvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGendvEXT) mogl_glunsupported("glMultiTexGendvEXT");
	glMultiTexGendvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_multitexgenfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGenfEXT) mogl_glunsupported("glMultiTexGenfEXT");
	glMultiTexGenfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexgenfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGenfvEXT) mogl_glunsupported("glMultiTexGenfvEXT");
	glMultiTexGenfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_multitexgeniext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGeniEXT) mogl_glunsupported("glMultiTexGeniEXT");
	glMultiTexGeniEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexgenivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexGenivEXT) mogl_glunsupported("glMultiTexGenivEXT");
	glMultiTexGenivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_getmultitexenvfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexEnvfvEXT) mogl_glunsupported("glGetMultiTexEnvfvEXT");
	glGetMultiTexEnvfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getmultitexenvivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexEnvivEXT) mogl_glunsupported("glGetMultiTexEnvivEXT");
	glGetMultiTexEnvivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getmultitexgendvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexGendvEXT) mogl_glunsupported("glGetMultiTexGendvEXT");
	glGetMultiTexGendvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_getmultitexgenfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexGenfvEXT) mogl_glunsupported("glGetMultiTexGenfvEXT");
	glGetMultiTexGenfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getmultitexgenivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexGenivEXT) mogl_glunsupported("glGetMultiTexGenivEXT");
	glGetMultiTexGenivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_multitexparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameteriEXT) mogl_glunsupported("glMultiTexParameteriEXT");
	glMultiTexParameteriEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameterivEXT) mogl_glunsupported("glMultiTexParameterivEXT");
	glMultiTexParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_multitexparameterfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameterfEXT) mogl_glunsupported("glMultiTexParameterfEXT");
	glMultiTexParameterfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameterfvEXT) mogl_glunsupported("glMultiTexParameterfvEXT");
	glMultiTexParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_multiteximage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexImage1DEXT) mogl_glunsupported("glMultiTexImage1DEXT");
	glMultiTexImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_multiteximage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexImage2DEXT) mogl_glunsupported("glMultiTexImage2DEXT");
	glMultiTexImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_multitexsubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexSubImage1DEXT) mogl_glunsupported("glMultiTexSubImage1DEXT");
	glMultiTexSubImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_multitexsubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexSubImage2DEXT) mogl_glunsupported("glMultiTexSubImage2DEXT");
	glMultiTexSubImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_copymultiteximage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyMultiTexImage1DEXT) mogl_glunsupported("glCopyMultiTexImage1DEXT");
	glCopyMultiTexImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]));

}

void gl_copymultiteximage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyMultiTexImage2DEXT) mogl_glunsupported("glCopyMultiTexImage2DEXT");
	glCopyMultiTexImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]));

}

void gl_copymultitexsubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyMultiTexSubImage1DEXT) mogl_glunsupported("glCopyMultiTexSubImage1DEXT");
	glCopyMultiTexSubImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]));

}

void gl_copymultitexsubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyMultiTexSubImage2DEXT) mogl_glunsupported("glCopyMultiTexSubImage2DEXT");
	glCopyMultiTexSubImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]));

}

void gl_getmultiteximageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexImageEXT) mogl_glunsupported("glGetMultiTexImageEXT");
	glGetMultiTexImageEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(void*)mxGetData(prhs[5]));

}

void gl_getmultitexparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexParameterfvEXT) mogl_glunsupported("glGetMultiTexParameterfvEXT");
	glGetMultiTexParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getmultitexparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexParameterivEXT) mogl_glunsupported("glGetMultiTexParameterivEXT");
	glGetMultiTexParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getmultitexlevelparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexLevelParameterfvEXT) mogl_glunsupported("glGetMultiTexLevelParameterfvEXT");
	glGetMultiTexLevelParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]));

}

void gl_getmultitexlevelparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexLevelParameterivEXT) mogl_glunsupported("glGetMultiTexLevelParameterivEXT");
	glGetMultiTexLevelParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_multiteximage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexImage3DEXT) mogl_glunsupported("glMultiTexImage3DEXT");
	glMultiTexImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_multitexsubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexSubImage3DEXT) mogl_glunsupported("glMultiTexSubImage3DEXT");
	glMultiTexSubImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLenum)mxGetScalar(prhs[10]),
		(const void*)mxGetData(prhs[11]));

}

void gl_copymultitexsubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyMultiTexSubImage3DEXT) mogl_glunsupported("glCopyMultiTexSubImage3DEXT");
	glCopyMultiTexSubImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]));

}

void gl_enableclientstateindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableClientStateIndexedEXT) mogl_glunsupported("glEnableClientStateIndexedEXT");
	glEnableClientStateIndexedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_disableclientstateindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableClientStateIndexedEXT) mogl_glunsupported("glDisableClientStateIndexedEXT");
	glDisableClientStateIndexedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_getfloatindexedvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFloatIndexedvEXT) mogl_glunsupported("glGetFloatIndexedvEXT");
	glGetFloatIndexedvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getdoubleindexedvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDoubleIndexedvEXT) mogl_glunsupported("glGetDoubleIndexedvEXT");
	glGetDoubleIndexedvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_enableindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableIndexedEXT) mogl_glunsupported("glEnableIndexedEXT");
	glEnableIndexedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_disableindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableIndexedEXT) mogl_glunsupported("glDisableIndexedEXT");
	glDisableIndexedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_isenabledindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsEnabledIndexedEXT) mogl_glunsupported("glIsEnabledIndexedEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsEnabledIndexedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_getintegerindexedvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetIntegerIndexedvEXT) mogl_glunsupported("glGetIntegerIndexedvEXT");
	glGetIntegerIndexedvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getbooleanindexedvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBooleanIndexedvEXT) mogl_glunsupported("glGetBooleanIndexedvEXT");
	glGetBooleanIndexedvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_compressedtextureimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureImage3DEXT) mogl_glunsupported("glCompressedTextureImage3DEXT");
	glCompressedTextureImage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_compressedtextureimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureImage2DEXT) mogl_glunsupported("glCompressedTextureImage2DEXT");
	glCompressedTextureImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_compressedtextureimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureImage1DEXT) mogl_glunsupported("glCompressedTextureImage1DEXT");
	glCompressedTextureImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_compressedtexturesubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage3DEXT) mogl_glunsupported("glCompressedTextureSubImage3DEXT");
	glCompressedTextureSubImage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLsizei)mxGetScalar(prhs[10]),
		(const void*)mxGetData(prhs[11]));

}

void gl_compressedtexturesubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage2DEXT) mogl_glunsupported("glCompressedTextureSubImage2DEXT");
	glCompressedTextureSubImage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_compressedtexturesubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedTextureSubImage1DEXT) mogl_glunsupported("glCompressedTextureSubImage1DEXT");
	glCompressedTextureSubImage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_getcompressedtextureimageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedTextureImageEXT) mogl_glunsupported("glGetCompressedTextureImageEXT");
	glGetCompressedTextureImageEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_compressedmultiteximage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexImage3DEXT) mogl_glunsupported("glCompressedMultiTexImage3DEXT");
	glCompressedMultiTexImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_compressedmultiteximage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexImage2DEXT) mogl_glunsupported("glCompressedMultiTexImage2DEXT");
	glCompressedMultiTexImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_compressedmultiteximage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexImage1DEXT) mogl_glunsupported("glCompressedMultiTexImage1DEXT");
	glCompressedMultiTexImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_compressedmultitexsubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexSubImage3DEXT) mogl_glunsupported("glCompressedMultiTexSubImage3DEXT");
	glCompressedMultiTexSubImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLsizei)mxGetScalar(prhs[10]),
		(const void*)mxGetData(prhs[11]));

}

void gl_compressedmultitexsubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexSubImage2DEXT) mogl_glunsupported("glCompressedMultiTexSubImage2DEXT");
	glCompressedMultiTexSubImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_compressedmultitexsubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCompressedMultiTexSubImage1DEXT) mogl_glunsupported("glCompressedMultiTexSubImage1DEXT");
	glCompressedMultiTexSubImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_getcompressedmultiteximageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCompressedMultiTexImageEXT) mogl_glunsupported("glGetCompressedMultiTexImageEXT");
	glGetCompressedMultiTexImageEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_matrixloadtransposefext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixLoadTransposefEXT) mogl_glunsupported("glMatrixLoadTransposefEXT");
	glMatrixLoadTransposefEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_matrixloadtransposedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixLoadTransposedEXT) mogl_glunsupported("glMatrixLoadTransposedEXT");
	glMatrixLoadTransposedEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_matrixmulttransposefext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixMultTransposefEXT) mogl_glunsupported("glMatrixMultTransposefEXT");
	glMatrixMultTransposefEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_matrixmulttransposedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMatrixMultTransposedEXT) mogl_glunsupported("glMatrixMultTransposedEXT");
	glMatrixMultTransposedEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_namedbufferdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferDataEXT) mogl_glunsupported("glNamedBufferDataEXT");
	glNamedBufferDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLsizeiptr)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_namedbuffersubdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferSubDataEXT) mogl_glunsupported("glNamedBufferSubDataEXT");
	glNamedBufferSubDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_mapnamedbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapNamedBufferEXT) mogl_glunsupported("glMapNamedBufferEXT");
	glMapNamedBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_unmapnamedbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapNamedBufferEXT) mogl_glunsupported("glUnmapNamedBufferEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glUnmapNamedBufferEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_getnamedbufferparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferParameterivEXT) mogl_glunsupported("glGetNamedBufferParameterivEXT");
	glGetNamedBufferParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getnamedbuffersubdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferSubDataEXT) mogl_glunsupported("glGetNamedBufferSubDataEXT");
	glGetNamedBufferSubDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_programuniform1fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1fEXT) mogl_glunsupported("glProgramUniform1fEXT");
	glProgramUniform1fEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_programuniform2fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2fEXT) mogl_glunsupported("glProgramUniform2fEXT");
	glProgramUniform2fEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_programuniform3fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3fEXT) mogl_glunsupported("glProgramUniform3fEXT");
	glProgramUniform3fEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_programuniform4fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4fEXT) mogl_glunsupported("glProgramUniform4fEXT");
	glProgramUniform4fEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_programuniform1iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1iEXT) mogl_glunsupported("glProgramUniform1iEXT");
	glProgramUniform1iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_programuniform2iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2iEXT) mogl_glunsupported("glProgramUniform2iEXT");
	glProgramUniform2iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_programuniform3iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3iEXT) mogl_glunsupported("glProgramUniform3iEXT");
	glProgramUniform3iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_programuniform4iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4iEXT) mogl_glunsupported("glProgramUniform4iEXT");
	glProgramUniform4iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_programuniform1fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1fvEXT) mogl_glunsupported("glProgramUniform1fvEXT");
	glProgramUniform1fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform2fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2fvEXT) mogl_glunsupported("glProgramUniform2fvEXT");
	glProgramUniform2fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform3fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3fvEXT) mogl_glunsupported("glProgramUniform3fvEXT");
	glProgramUniform3fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4fvEXT) mogl_glunsupported("glProgramUniform4fvEXT");
	glProgramUniform4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programuniform1ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1ivEXT) mogl_glunsupported("glProgramUniform1ivEXT");
	glProgramUniform1ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform2ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2ivEXT) mogl_glunsupported("glProgramUniform2ivEXT");
	glProgramUniform2ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform3ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3ivEXT) mogl_glunsupported("glProgramUniform3ivEXT");
	glProgramUniform3ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniform4ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4ivEXT) mogl_glunsupported("glProgramUniform4ivEXT");
	glProgramUniform4ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programuniformmatrix2fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2fvEXT) mogl_glunsupported("glProgramUniformMatrix2fvEXT");
	glProgramUniformMatrix2fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3fvEXT) mogl_glunsupported("glProgramUniformMatrix3fvEXT");
	glProgramUniformMatrix3fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4fvEXT) mogl_glunsupported("glProgramUniformMatrix4fvEXT");
	glProgramUniformMatrix4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x3fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x3fvEXT) mogl_glunsupported("glProgramUniformMatrix2x3fvEXT");
	glProgramUniformMatrix2x3fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x2fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x2fvEXT) mogl_glunsupported("glProgramUniformMatrix3x2fvEXT");
	glProgramUniformMatrix3x2fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix2x4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix2x4fvEXT) mogl_glunsupported("glProgramUniformMatrix2x4fvEXT");
	glProgramUniformMatrix2x4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x2fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x2fvEXT) mogl_glunsupported("glProgramUniformMatrix4x2fvEXT");
	glProgramUniformMatrix4x2fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix3x4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix3x4fvEXT) mogl_glunsupported("glProgramUniformMatrix3x4fvEXT");
	glProgramUniformMatrix3x4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programuniformmatrix4x3fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformMatrix4x3fvEXT) mogl_glunsupported("glProgramUniformMatrix4x3fvEXT");
	glProgramUniformMatrix4x3fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_texturebufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBufferEXT) mogl_glunsupported("glTextureBufferEXT");
	glTextureBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_multitexbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexBufferEXT) mogl_glunsupported("glMultiTexBufferEXT");
	glMultiTexBufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_textureparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterIivEXT) mogl_glunsupported("glTextureParameterIivEXT");
	glTextureParameterIivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_textureparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureParameterIuivEXT) mogl_glunsupported("glTextureParameterIuivEXT");
	glTextureParameterIuivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_gettextureparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterIivEXT) mogl_glunsupported("glGetTextureParameterIivEXT");
	glGetTextureParameterIivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettextureparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureParameterIuivEXT) mogl_glunsupported("glGetTextureParameterIuivEXT");
	glGetTextureParameterIuivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_multitexparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameterIivEXT) mogl_glunsupported("glMultiTexParameterIivEXT");
	glMultiTexParameterIivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_multitexparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexParameterIuivEXT) mogl_glunsupported("glMultiTexParameterIuivEXT");
	glMultiTexParameterIuivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_getmultitexparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexParameterIivEXT) mogl_glunsupported("glGetMultiTexParameterIivEXT");
	glGetMultiTexParameterIivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getmultitexparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultiTexParameterIuivEXT) mogl_glunsupported("glGetMultiTexParameterIuivEXT");
	glGetMultiTexParameterIuivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform1uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1uiEXT) mogl_glunsupported("glProgramUniform1uiEXT");
	glProgramUniform1uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_programuniform2uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2uiEXT) mogl_glunsupported("glProgramUniform2uiEXT");
	glProgramUniform2uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_programuniform3uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3uiEXT) mogl_glunsupported("glProgramUniform3uiEXT");
	glProgramUniform3uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_programuniform4uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4uiEXT) mogl_glunsupported("glProgramUniform4uiEXT");
	glProgramUniform4uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_programuniform1uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform1uivEXT) mogl_glunsupported("glProgramUniform1uivEXT");
	glProgramUniform1uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform2uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform2uivEXT) mogl_glunsupported("glProgramUniform2uivEXT");
	glProgramUniform2uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform3uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform3uivEXT) mogl_glunsupported("glProgramUniform3uivEXT");
	glProgramUniform3uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programuniform4uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniform4uivEXT) mogl_glunsupported("glProgramUniform4uivEXT");
	glProgramUniform4uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_namedprogramlocalparameters4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameters4fvEXT) mogl_glunsupported("glNamedProgramLocalParameters4fvEXT");
	glNamedProgramLocalParameters4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_namedprogramlocalparameteri4iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameterI4iEXT) mogl_glunsupported("glNamedProgramLocalParameterI4iEXT");
	glNamedProgramLocalParameterI4iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_namedprogramlocalparameteri4ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameterI4ivEXT) mogl_glunsupported("glNamedProgramLocalParameterI4ivEXT");
	glNamedProgramLocalParameterI4ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_namedprogramlocalparametersi4ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParametersI4ivEXT) mogl_glunsupported("glNamedProgramLocalParametersI4ivEXT");
	glNamedProgramLocalParametersI4ivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLint*)mxGetData(prhs[4]));

}

void gl_namedprogramlocalparameteri4uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameterI4uiEXT) mogl_glunsupported("glNamedProgramLocalParameterI4uiEXT");
	glNamedProgramLocalParameterI4uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]));

}

void gl_namedprogramlocalparameteri4uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameterI4uivEXT) mogl_glunsupported("glNamedProgramLocalParameterI4uivEXT");
	glNamedProgramLocalParameterI4uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_namedprogramlocalparametersi4uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParametersI4uivEXT) mogl_glunsupported("glNamedProgramLocalParametersI4uivEXT");
	glNamedProgramLocalParametersI4uivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLuint*)mxGetData(prhs[4]));

}

void gl_getnamedprogramlocalparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramLocalParameterIivEXT) mogl_glunsupported("glGetNamedProgramLocalParameterIivEXT");
	glGetNamedProgramLocalParameterIivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getnamedprogramlocalparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramLocalParameterIuivEXT) mogl_glunsupported("glGetNamedProgramLocalParameterIuivEXT");
	glGetNamedProgramLocalParameterIuivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_enableclientstateiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableClientStateiEXT) mogl_glunsupported("glEnableClientStateiEXT");
	glEnableClientStateiEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_disableclientstateiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableClientStateiEXT) mogl_glunsupported("glDisableClientStateiEXT");
	glDisableClientStateiEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_getfloati_vext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFloati_vEXT) mogl_glunsupported("glGetFloati_vEXT");
	glGetFloati_vEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getdoublei_vext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDoublei_vEXT) mogl_glunsupported("glGetDoublei_vEXT");
	glGetDoublei_vEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_namedprogramstringext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramStringEXT) mogl_glunsupported("glNamedProgramStringEXT");
	glNamedProgramStringEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_namedprogramlocalparameter4dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameter4dEXT) mogl_glunsupported("glNamedProgramLocalParameter4dEXT");
	glNamedProgramLocalParameter4dEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void gl_namedprogramlocalparameter4dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameter4dvEXT) mogl_glunsupported("glNamedProgramLocalParameter4dvEXT");
	glNamedProgramLocalParameter4dvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_namedprogramlocalparameter4fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameter4fEXT) mogl_glunsupported("glNamedProgramLocalParameter4fEXT");
	glNamedProgramLocalParameter4fEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]));

}

void gl_namedprogramlocalparameter4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedProgramLocalParameter4fvEXT) mogl_glunsupported("glNamedProgramLocalParameter4fvEXT");
	glNamedProgramLocalParameter4fvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_getnamedprogramlocalparameterdvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramLocalParameterdvEXT) mogl_glunsupported("glGetNamedProgramLocalParameterdvEXT");
	glGetNamedProgramLocalParameterdvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_getnamedprogramlocalparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramLocalParameterfvEXT) mogl_glunsupported("glGetNamedProgramLocalParameterfvEXT");
	glGetNamedProgramLocalParameterfvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getnamedprogramivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramivEXT) mogl_glunsupported("glGetNamedProgramivEXT");
	glGetNamedProgramivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getnamedprogramstringext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedProgramStringEXT) mogl_glunsupported("glGetNamedProgramStringEXT");
	glGetNamedProgramStringEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_namedrenderbufferstorageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedRenderbufferStorageEXT) mogl_glunsupported("glNamedRenderbufferStorageEXT");
	glNamedRenderbufferStorageEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_getnamedrenderbufferparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedRenderbufferParameterivEXT) mogl_glunsupported("glGetNamedRenderbufferParameterivEXT");
	glGetNamedRenderbufferParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_namedrenderbufferstoragemultisampleext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedRenderbufferStorageMultisampleEXT) mogl_glunsupported("glNamedRenderbufferStorageMultisampleEXT");
	glNamedRenderbufferStorageMultisampleEXT((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_namedrenderbufferstoragemultisamplecoverageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedRenderbufferStorageMultisampleCoverageEXT) mogl_glunsupported("glNamedRenderbufferStorageMultisampleCoverageEXT");
	glNamedRenderbufferStorageMultisampleCoverageEXT((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_checknamedframebufferstatusext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCheckNamedFramebufferStatusEXT) mogl_glunsupported("glCheckNamedFramebufferStatusEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCheckNamedFramebufferStatusEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_namedframebuffertexture1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTexture1DEXT) mogl_glunsupported("glNamedFramebufferTexture1DEXT");
	glNamedFramebufferTexture1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_namedframebuffertexture2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTexture2DEXT) mogl_glunsupported("glNamedFramebufferTexture2DEXT");
	glNamedFramebufferTexture2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_namedframebuffertexture3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTexture3DEXT) mogl_glunsupported("glNamedFramebufferTexture3DEXT");
	glNamedFramebufferTexture3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_namedframebufferrenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferRenderbufferEXT) mogl_glunsupported("glNamedFramebufferRenderbufferEXT");
	glNamedFramebufferRenderbufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_getnamedframebufferattachmentparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedFramebufferAttachmentParameterivEXT) mogl_glunsupported("glGetNamedFramebufferAttachmentParameterivEXT");
	glGetNamedFramebufferAttachmentParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_generatetexturemipmapext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenerateTextureMipmapEXT) mogl_glunsupported("glGenerateTextureMipmapEXT");
	glGenerateTextureMipmapEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_generatemultitexmipmapext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenerateMultiTexMipmapEXT) mogl_glunsupported("glGenerateMultiTexMipmapEXT");
	glGenerateMultiTexMipmapEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_framebufferdrawbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferDrawBufferEXT) mogl_glunsupported("glFramebufferDrawBufferEXT");
	glFramebufferDrawBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_framebufferdrawbuffersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferDrawBuffersEXT) mogl_glunsupported("glFramebufferDrawBuffersEXT");
	glFramebufferDrawBuffersEXT((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLenum*)mxGetData(prhs[2]));

}

void gl_framebufferreadbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferReadBufferEXT) mogl_glunsupported("glFramebufferReadBufferEXT");
	glFramebufferReadBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_getframebufferparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFramebufferParameterivEXT) mogl_glunsupported("glGetFramebufferParameterivEXT");
	glGetFramebufferParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_namedcopybuffersubdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedCopyBufferSubDataEXT) mogl_glunsupported("glNamedCopyBufferSubDataEXT");
	glNamedCopyBufferSubDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLintptr)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_namedframebuffertextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTextureEXT) mogl_glunsupported("glNamedFramebufferTextureEXT");
	glNamedFramebufferTextureEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_namedframebuffertexturelayerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTextureLayerEXT) mogl_glunsupported("glNamedFramebufferTextureLayerEXT");
	glNamedFramebufferTextureLayerEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_namedframebuffertexturefaceext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferTextureFaceEXT) mogl_glunsupported("glNamedFramebufferTextureFaceEXT");
	glNamedFramebufferTextureFaceEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_texturerenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureRenderbufferEXT) mogl_glunsupported("glTextureRenderbufferEXT");
	glTextureRenderbufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_multitexrenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiTexRenderbufferEXT) mogl_glunsupported("glMultiTexRenderbufferEXT");
	glMultiTexRenderbufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexarrayvertexoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexOffsetEXT) mogl_glunsupported("glVertexArrayVertexOffsetEXT");
	glVertexArrayVertexOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLintptr)mxGetScalar(prhs[5]));

}

void gl_vertexarraycoloroffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayColorOffsetEXT) mogl_glunsupported("glVertexArrayColorOffsetEXT");
	glVertexArrayColorOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLintptr)mxGetScalar(prhs[5]));

}

void gl_vertexarrayedgeflagoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayEdgeFlagOffsetEXT) mogl_glunsupported("glVertexArrayEdgeFlagOffsetEXT");
	glVertexArrayEdgeFlagOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]));

}

void gl_vertexarrayindexoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayIndexOffsetEXT) mogl_glunsupported("glVertexArrayIndexOffsetEXT");
	glVertexArrayIndexOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLintptr)mxGetScalar(prhs[4]));

}

void gl_vertexarraynormaloffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayNormalOffsetEXT) mogl_glunsupported("glVertexArrayNormalOffsetEXT");
	glVertexArrayNormalOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLintptr)mxGetScalar(prhs[4]));

}

void gl_vertexarraytexcoordoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayTexCoordOffsetEXT) mogl_glunsupported("glVertexArrayTexCoordOffsetEXT");
	glVertexArrayTexCoordOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLintptr)mxGetScalar(prhs[5]));

}

void gl_vertexarraymultitexcoordoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayMultiTexCoordOffsetEXT) mogl_glunsupported("glVertexArrayMultiTexCoordOffsetEXT");
	glVertexArrayMultiTexCoordOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLintptr)mxGetScalar(prhs[6]));

}

void gl_vertexarrayfogcoordoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayFogCoordOffsetEXT) mogl_glunsupported("glVertexArrayFogCoordOffsetEXT");
	glVertexArrayFogCoordOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLintptr)mxGetScalar(prhs[4]));

}

void gl_vertexarraysecondarycoloroffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArraySecondaryColorOffsetEXT) mogl_glunsupported("glVertexArraySecondaryColorOffsetEXT");
	glVertexArraySecondaryColorOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLintptr)mxGetScalar(prhs[5]));

}

void gl_vertexarrayvertexattriboffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribOffsetEXT) mogl_glunsupported("glVertexArrayVertexAttribOffsetEXT");
	glVertexArrayVertexAttribOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLintptr)mxGetScalar(prhs[7]));

}

void gl_vertexarrayvertexattribioffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribIOffsetEXT) mogl_glunsupported("glVertexArrayVertexAttribIOffsetEXT");
	glVertexArrayVertexAttribIOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLintptr)mxGetScalar(prhs[6]));

}

void gl_enablevertexarrayext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexArrayEXT) mogl_glunsupported("glEnableVertexArrayEXT");
	glEnableVertexArrayEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_disablevertexarrayext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexArrayEXT) mogl_glunsupported("glDisableVertexArrayEXT");
	glDisableVertexArrayEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_enablevertexarrayattribext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVertexArrayAttribEXT) mogl_glunsupported("glEnableVertexArrayAttribEXT");
	glEnableVertexArrayAttribEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_disablevertexarrayattribext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVertexArrayAttribEXT) mogl_glunsupported("glDisableVertexArrayAttribEXT");
	glDisableVertexArrayAttribEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_getvertexarrayintegervext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexArrayIntegervEXT) mogl_glunsupported("glGetVertexArrayIntegervEXT");
	glGetVertexArrayIntegervEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvertexarrayintegeri_vext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexArrayIntegeri_vEXT) mogl_glunsupported("glGetVertexArrayIntegeri_vEXT");
	glGetVertexArrayIntegeri_vEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_mapnamedbufferrangeext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapNamedBufferRangeEXT) mogl_glunsupported("glMapNamedBufferRangeEXT");
	glMapNamedBufferRangeEXT((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_flushmappednamedbufferrangeext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushMappedNamedBufferRangeEXT) mogl_glunsupported("glFlushMappedNamedBufferRangeEXT");
	glFlushMappedNamedBufferRangeEXT((GLuint)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]));

}

void gl_namedbufferstorageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedBufferStorageEXT) mogl_glunsupported("glNamedBufferStorageEXT");
	glNamedBufferStorageEXT((GLuint)mxGetScalar(prhs[0]),
		(GLsizeiptr)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]));

}

void gl_clearnamedbufferdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedBufferDataEXT) mogl_glunsupported("glClearNamedBufferDataEXT");
	glClearNamedBufferDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_clearnamedbuffersubdataext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearNamedBufferSubDataEXT) mogl_glunsupported("glClearNamedBufferSubDataEXT");
	glClearNamedBufferSubDataEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizeiptr)mxGetScalar(prhs[2]),
		(GLsizeiptr)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_namedframebufferparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNamedFramebufferParameteriEXT) mogl_glunsupported("glNamedFramebufferParameteriEXT");
	glNamedFramebufferParameteriEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_getnamedframebufferparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedFramebufferParameterivEXT) mogl_glunsupported("glGetNamedFramebufferParameterivEXT");
	glGetNamedFramebufferParameterivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_texturebufferrangeext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBufferRangeEXT) mogl_glunsupported("glTextureBufferRangeEXT");
	glTextureBufferRangeEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLintptr)mxGetScalar(prhs[4]),
		(GLsizeiptr)mxGetScalar(prhs[5]));

}

void gl_texturestorage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage1DEXT) mogl_glunsupported("glTextureStorage1DEXT");
	glTextureStorage1DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_texturestorage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage2DEXT) mogl_glunsupported("glTextureStorage2DEXT");
	glTextureStorage2DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_texturestorage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage3DEXT) mogl_glunsupported("glTextureStorage3DEXT");
	glTextureStorage3DEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]));

}

void gl_texturestorage2dmultisampleext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage2DMultisampleEXT) mogl_glunsupported("glTextureStorage2DMultisampleEXT");
	glTextureStorage2DMultisampleEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_texturestorage3dmultisampleext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureStorage3DMultisampleEXT) mogl_glunsupported("glTextureStorage3DMultisampleEXT");
	glTextureStorage3DMultisampleEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]));

}

void gl_vertexarraybindvertexbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayBindVertexBufferEXT) mogl_glunsupported("glVertexArrayBindVertexBufferEXT");
	glVertexArrayBindVertexBufferEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_vertexarrayvertexattribformatext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribFormatEXT) mogl_glunsupported("glVertexArrayVertexAttribFormatEXT");
	glVertexArrayVertexAttribFormatEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLboolean)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_vertexarrayvertexattribiformatext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribIFormatEXT) mogl_glunsupported("glVertexArrayVertexAttribIFormatEXT");
	glVertexArrayVertexAttribIFormatEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexarrayvertexattriblformatext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribLFormatEXT) mogl_glunsupported("glVertexArrayVertexAttribLFormatEXT");
	glVertexArrayVertexAttribLFormatEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexarrayvertexattribbindingext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribBindingEXT) mogl_glunsupported("glVertexArrayVertexAttribBindingEXT");
	glVertexArrayVertexAttribBindingEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexarrayvertexbindingdivisorext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexBindingDivisorEXT) mogl_glunsupported("glVertexArrayVertexBindingDivisorEXT");
	glVertexArrayVertexBindingDivisorEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexarrayvertexattribloffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribLOffsetEXT) mogl_glunsupported("glVertexArrayVertexAttribLOffsetEXT");
	glVertexArrayVertexAttribLOffsetEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLintptr)mxGetScalar(prhs[6]));

}

void gl_texturepagecommitmentext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexturePageCommitmentEXT) mogl_glunsupported("glTexturePageCommitmentEXT");
	glTexturePageCommitmentEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLboolean)mxGetScalar(prhs[8]));

}

void gl_vertexarrayvertexattribdivisorext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayVertexAttribDivisorEXT) mogl_glunsupported("glVertexArrayVertexAttribDivisorEXT");
	glVertexArrayVertexAttribDivisorEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_colormaskindexedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorMaskIndexedEXT) mogl_glunsupported("glColorMaskIndexedEXT");
	glColorMaskIndexedEXT((GLuint)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLboolean)mxGetScalar(prhs[4]));

}

void gl_drawarraysinstancedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysInstancedEXT) mogl_glunsupported("glDrawArraysInstancedEXT");
	glDrawArraysInstancedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_drawelementsinstancedext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElementsInstancedEXT) mogl_glunsupported("glDrawElementsInstancedEXT");
	glDrawElementsInstancedEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_drawrangeelementsext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawRangeElementsEXT) mogl_glunsupported("glDrawRangeElementsEXT");
	glDrawRangeElementsEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_fogcoordfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordfEXT) mogl_glunsupported("glFogCoordfEXT");
	glFogCoordfEXT((GLfloat)mxGetScalar(prhs[0]));

}

void gl_fogcoordfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordfvEXT) mogl_glunsupported("glFogCoordfvEXT");
	glFogCoordfvEXT((const GLfloat*)mxGetData(prhs[0]));

}

void gl_fogcoorddext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoorddEXT) mogl_glunsupported("glFogCoorddEXT");
	glFogCoorddEXT((GLdouble)mxGetScalar(prhs[0]));

}

void gl_fogcoorddvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoorddvEXT) mogl_glunsupported("glFogCoorddvEXT");
	glFogCoorddvEXT((const GLdouble*)mxGetData(prhs[0]));

}

void gl_fogcoordpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordPointerEXT) mogl_glunsupported("glFogCoordPointerEXT");
	glFogCoordPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_blitframebufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlitFramebufferEXT) mogl_glunsupported("glBlitFramebufferEXT");
	glBlitFramebufferEXT((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLbitfield)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]));

}

void gl_renderbufferstoragemultisampleext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderbufferStorageMultisampleEXT) mogl_glunsupported("glRenderbufferStorageMultisampleEXT");
	glRenderbufferStorageMultisampleEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_isrenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsRenderbufferEXT) mogl_glunsupported("glIsRenderbufferEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsRenderbufferEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindrenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindRenderbufferEXT) mogl_glunsupported("glBindRenderbufferEXT");
	glBindRenderbufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleterenderbuffersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteRenderbuffersEXT) mogl_glunsupported("glDeleteRenderbuffersEXT");
	glDeleteRenderbuffersEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genrenderbuffersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenRenderbuffersEXT) mogl_glunsupported("glGenRenderbuffersEXT");
	glGenRenderbuffersEXT((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_renderbufferstorageext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderbufferStorageEXT) mogl_glunsupported("glRenderbufferStorageEXT");
	glRenderbufferStorageEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_getrenderbufferparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetRenderbufferParameterivEXT) mogl_glunsupported("glGetRenderbufferParameterivEXT");
	glGetRenderbufferParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_isframebufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsFramebufferEXT) mogl_glunsupported("glIsFramebufferEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsFramebufferEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindframebufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFramebufferEXT) mogl_glunsupported("glBindFramebufferEXT");
	glBindFramebufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleteframebuffersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteFramebuffersEXT) mogl_glunsupported("glDeleteFramebuffersEXT");
	glDeleteFramebuffersEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genframebuffersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenFramebuffersEXT) mogl_glunsupported("glGenFramebuffersEXT");
	glGenFramebuffersEXT((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_checkframebufferstatusext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCheckFramebufferStatusEXT) mogl_glunsupported("glCheckFramebufferStatusEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCheckFramebufferStatusEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_framebuffertexture1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture1DEXT) mogl_glunsupported("glFramebufferTexture1DEXT");
	glFramebufferTexture1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_framebuffertexture2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture2DEXT) mogl_glunsupported("glFramebufferTexture2DEXT");
	glFramebufferTexture2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_framebuffertexture3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTexture3DEXT) mogl_glunsupported("glFramebufferTexture3DEXT");
	glFramebufferTexture3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_framebufferrenderbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferRenderbufferEXT) mogl_glunsupported("glFramebufferRenderbufferEXT");
	glFramebufferRenderbufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_getframebufferattachmentparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFramebufferAttachmentParameterivEXT) mogl_glunsupported("glGetFramebufferAttachmentParameterivEXT");
	glGetFramebufferAttachmentParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_generatemipmapext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenerateMipmapEXT) mogl_glunsupported("glGenerateMipmapEXT");
	glGenerateMipmapEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_programparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameteriEXT) mogl_glunsupported("glProgramParameteriEXT");
	glProgramParameteriEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_programenvparameters4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameters4fvEXT) mogl_glunsupported("glProgramEnvParameters4fvEXT");
	glProgramEnvParameters4fvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programlocalparameters4fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameters4fvEXT) mogl_glunsupported("glProgramLocalParameters4fvEXT");
	glProgramLocalParameters4fvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_getuniformuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetUniformuivEXT) mogl_glunsupported("glGetUniformuivEXT");
	glGetUniformuivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_bindfragdatalocationext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindFragDataLocationEXT) mogl_glunsupported("glBindFragDataLocationEXT");
	glBindFragDataLocationEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getfragdatalocationext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragDataLocationEXT) mogl_glunsupported("glGetFragDataLocationEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetFragDataLocationEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_uniform1uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1uiEXT) mogl_glunsupported("glUniform1uiEXT");
	glUniform1uiEXT((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_uniform2uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2uiEXT) mogl_glunsupported("glUniform2uiEXT");
	glUniform2uiEXT((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_uniform3uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3uiEXT) mogl_glunsupported("glUniform3uiEXT");
	glUniform3uiEXT((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_uniform4uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4uiEXT) mogl_glunsupported("glUniform4uiEXT");
	glUniform4uiEXT((GLint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_uniform1uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform1uivEXT) mogl_glunsupported("glUniform1uivEXT");
	glUniform1uivEXT((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform2uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform2uivEXT) mogl_glunsupported("glUniform2uivEXT");
	glUniform2uivEXT((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform3uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform3uivEXT) mogl_glunsupported("glUniform3uivEXT");
	glUniform3uivEXT((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_uniform4uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniform4uivEXT) mogl_glunsupported("glUniform4uivEXT");
	glUniform4uivEXT((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_gethistogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogramEXT) mogl_glunsupported("glGetHistogramEXT");
	glGetHistogramEXT((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(void*)mxGetData(prhs[4]));

}

void gl_gethistogramparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogramParameterfvEXT) mogl_glunsupported("glGetHistogramParameterfvEXT");
	glGetHistogramParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gethistogramparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogramParameterivEXT) mogl_glunsupported("glGetHistogramParameterivEXT");
	glGetHistogramParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getminmaxext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmaxEXT) mogl_glunsupported("glGetMinmaxEXT");
	glGetMinmaxEXT((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(void*)mxGetData(prhs[4]));

}

void gl_getminmaxparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmaxParameterfvEXT) mogl_glunsupported("glGetMinmaxParameterfvEXT");
	glGetMinmaxParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getminmaxparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmaxParameterivEXT) mogl_glunsupported("glGetMinmaxParameterivEXT");
	glGetMinmaxParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_histogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glHistogramEXT) mogl_glunsupported("glHistogramEXT");
	glHistogramEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_minmaxext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMinmaxEXT) mogl_glunsupported("glMinmaxEXT");
	glMinmaxEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]));

}

void gl_resethistogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResetHistogramEXT) mogl_glunsupported("glResetHistogramEXT");
	glResetHistogramEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_resetminmaxext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResetMinmaxEXT) mogl_glunsupported("glResetMinmaxEXT");
	glResetMinmaxEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_indexfuncext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexFuncEXT) mogl_glunsupported("glIndexFuncEXT");
	glIndexFuncEXT((GLenum)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]));

}

void gl_indexmaterialext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexMaterialEXT) mogl_glunsupported("glIndexMaterialEXT");
	glIndexMaterialEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_applytextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glApplyTextureEXT) mogl_glunsupported("glApplyTextureEXT");
	glApplyTextureEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_texturelightext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureLightEXT) mogl_glunsupported("glTextureLightEXT");
	glTextureLightEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_texturematerialext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureMaterialEXT) mogl_glunsupported("glTextureMaterialEXT");
	glTextureMaterialEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_multidrawarraysext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysEXT) mogl_glunsupported("glMultiDrawArraysEXT");
	glMultiDrawArraysEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(const GLsizei*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_multidrawelementsext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsEXT) mogl_glunsupported("glMultiDrawElementsEXT");
	glMultiDrawElementsEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLsizei*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_samplemaskext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMaskEXT) mogl_glunsupported("glSampleMaskEXT");
	glSampleMaskEXT((GLclampf)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_samplepatternext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplePatternEXT) mogl_glunsupported("glSamplePatternEXT");
	glSamplePatternEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_colortableext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableEXT) mogl_glunsupported("glColorTableEXT");
	glColorTableEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_getcolortableext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableEXT) mogl_glunsupported("glGetColorTableEXT");
	glGetColorTableEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_getcolortableparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameterivEXT) mogl_glunsupported("glGetColorTableParameterivEXT");
	glGetColorTableParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getcolortableparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameterfvEXT) mogl_glunsupported("glGetColorTableParameterfvEXT");
	glGetColorTableParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_pixeltransformparameteriext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransformParameteriEXT) mogl_glunsupported("glPixelTransformParameteriEXT");
	glPixelTransformParameteriEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_pixeltransformparameterfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransformParameterfEXT) mogl_glunsupported("glPixelTransformParameterfEXT");
	glPixelTransformParameterfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_pixeltransformparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransformParameterivEXT) mogl_glunsupported("glPixelTransformParameterivEXT");
	glPixelTransformParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_pixeltransformparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTransformParameterfvEXT) mogl_glunsupported("glPixelTransformParameterfvEXT");
	glPixelTransformParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getpixeltransformparameterivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPixelTransformParameterivEXT) mogl_glunsupported("glGetPixelTransformParameterivEXT");
	glGetPixelTransformParameterivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpixeltransformparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPixelTransformParameterfvEXT) mogl_glunsupported("glGetPixelTransformParameterfvEXT");
	glGetPixelTransformParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_pointparameterfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterfEXT) mogl_glunsupported("glPointParameterfEXT");
	glPointParameterfEXT((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointparameterfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterfvEXT) mogl_glunsupported("glPointParameterfvEXT");
	glPointParameterfvEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_polygonoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPolygonOffsetEXT) mogl_glunsupported("glPolygonOffsetEXT");
	glPolygonOffsetEXT((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_provokingvertexext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProvokingVertexEXT) mogl_glunsupported("glProvokingVertexEXT");
	glProvokingVertexEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_secondarycolor3bext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3bEXT) mogl_glunsupported("glSecondaryColor3bEXT");
	glSecondaryColor3bEXT((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3bvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3bvEXT) mogl_glunsupported("glSecondaryColor3bvEXT");
	glSecondaryColor3bvEXT((const GLbyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3dEXT) mogl_glunsupported("glSecondaryColor3dEXT");
	glSecondaryColor3dEXT((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3dvEXT) mogl_glunsupported("glSecondaryColor3dvEXT");
	glSecondaryColor3dvEXT((const GLdouble*)mxGetData(prhs[0]));

}

void gl_secondarycolor3fext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3fEXT) mogl_glunsupported("glSecondaryColor3fEXT");
	glSecondaryColor3fEXT((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3fvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3fvEXT) mogl_glunsupported("glSecondaryColor3fvEXT");
	glSecondaryColor3fvEXT((const GLfloat*)mxGetData(prhs[0]));

}

void gl_secondarycolor3iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3iEXT) mogl_glunsupported("glSecondaryColor3iEXT");
	glSecondaryColor3iEXT((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ivEXT) mogl_glunsupported("glSecondaryColor3ivEXT");
	glSecondaryColor3ivEXT((const GLint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3sext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3sEXT) mogl_glunsupported("glSecondaryColor3sEXT");
	glSecondaryColor3sEXT((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3svext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3svEXT) mogl_glunsupported("glSecondaryColor3svEXT");
	glSecondaryColor3svEXT((const GLshort*)mxGetData(prhs[0]));

}

void gl_secondarycolor3ubext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ubEXT) mogl_glunsupported("glSecondaryColor3ubEXT");
	glSecondaryColor3ubEXT((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3ubvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3ubvEXT) mogl_glunsupported("glSecondaryColor3ubvEXT");
	glSecondaryColor3ubvEXT((const GLubyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3uiEXT) mogl_glunsupported("glSecondaryColor3uiEXT");
	glSecondaryColor3uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3uivEXT) mogl_glunsupported("glSecondaryColor3uivEXT");
	glSecondaryColor3uivEXT((const GLuint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3usext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3usEXT) mogl_glunsupported("glSecondaryColor3usEXT");
	glSecondaryColor3usEXT((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3usvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColor3usvEXT) mogl_glunsupported("glSecondaryColor3usvEXT");
	glSecondaryColor3usvEXT((const GLushort*)mxGetData(prhs[0]));

}

void gl_secondarycolorpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColorPointerEXT) mogl_glunsupported("glSecondaryColorPointerEXT");
	glSecondaryColorPointerEXT((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_useshaderprogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUseShaderProgramEXT) mogl_glunsupported("glUseShaderProgramEXT");
	glUseShaderProgramEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_activeprogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveProgramEXT) mogl_glunsupported("glActiveProgramEXT");
	glActiveProgramEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_createshaderprogramext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreateShaderProgramEXT) mogl_glunsupported("glCreateShaderProgramEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateShaderProgramEXT((GLenum)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_bindimagetextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindImageTextureEXT) mogl_glunsupported("glBindImageTextureEXT");
	glBindImageTextureEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_memorybarrierext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMemoryBarrierEXT) mogl_glunsupported("glMemoryBarrierEXT");
	glMemoryBarrierEXT((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_activestencilfaceext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveStencilFaceEXT) mogl_glunsupported("glActiveStencilFaceEXT");
	glActiveStencilFaceEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_texsubimage1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage1DEXT) mogl_glunsupported("glTexSubImage1DEXT");
	glTexSubImage1DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]));

}

void gl_texsubimage2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage2DEXT) mogl_glunsupported("glTexSubImage2DEXT");
	glTexSubImage2DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_teximage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage3DEXT) mogl_glunsupported("glTexImage3DEXT");
	glTexImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(const void*)mxGetData(prhs[9]));

}

void gl_texsubimage3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage3DEXT) mogl_glunsupported("glTexSubImage3DEXT");
	glTexSubImage3DEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_framebuffertexturelayerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureLayerEXT) mogl_glunsupported("glFramebufferTextureLayerEXT");
	glFramebufferTextureLayerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_texbufferext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexBufferEXT) mogl_glunsupported("glTexBufferEXT");
	glTexBufferEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_texparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterIivEXT) mogl_glunsupported("glTexParameterIivEXT");
	glTexParameterIivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexParameterIuivEXT) mogl_glunsupported("glTexParameterIuivEXT");
	glTexParameterIuivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_gettexparameteriivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameterIivEXT) mogl_glunsupported("glGetTexParameterIivEXT");
	glGetTexParameterIivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettexparameteriuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexParameterIuivEXT) mogl_glunsupported("glGetTexParameterIuivEXT");
	glGetTexParameterIuivEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_clearcoloriiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearColorIiEXT) mogl_glunsupported("glClearColorIiEXT");
	glClearColorIiEXT((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_clearcoloriuiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearColorIuiEXT) mogl_glunsupported("glClearColorIuiEXT");
	glClearColorIuiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_aretexturesresidentext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAreTexturesResidentEXT) mogl_glunsupported("glAreTexturesResidentEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glAreTexturesResidentEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_bindtextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTextureEXT) mogl_glunsupported("glBindTextureEXT");
	glBindTextureEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletetexturesext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteTexturesEXT) mogl_glunsupported("glDeleteTexturesEXT");
	glDeleteTexturesEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_gentexturesext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenTexturesEXT) mogl_glunsupported("glGenTexturesEXT");
	glGenTexturesEXT((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_istextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTextureEXT) mogl_glunsupported("glIsTextureEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTextureEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_prioritizetexturesext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPrioritizeTexturesEXT) mogl_glunsupported("glPrioritizeTexturesEXT");
	glPrioritizeTexturesEXT((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(const GLclampf*)mxGetData(prhs[2]));

}

void gl_texturenormalext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureNormalEXT) mogl_glunsupported("glTextureNormalEXT");
	glTextureNormalEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_getqueryobjecti64vext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjecti64vEXT) mogl_glunsupported("glGetQueryObjecti64vEXT");
	glGetQueryObjecti64vEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64*)mxGetData(prhs[2]));

}

void gl_getqueryobjectui64vext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetQueryObjectui64vEXT) mogl_glunsupported("glGetQueryObjectui64vEXT");
	glGetQueryObjectui64vEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64*)mxGetData(prhs[2]));

}

void gl_begintransformfeedbackext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginTransformFeedbackEXT) mogl_glunsupported("glBeginTransformFeedbackEXT");
	glBeginTransformFeedbackEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_endtransformfeedbackext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndTransformFeedbackEXT) mogl_glunsupported("glEndTransformFeedbackEXT");
	glEndTransformFeedbackEXT();

}

void gl_bindbufferrangeext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferRangeEXT) mogl_glunsupported("glBindBufferRangeEXT");
	glBindBufferRangeEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_bindbufferoffsetext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferOffsetEXT) mogl_glunsupported("glBindBufferOffsetEXT");
	glBindBufferOffsetEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]));

}

void gl_bindbufferbaseext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferBaseEXT) mogl_glunsupported("glBindBufferBaseEXT");
	glBindBufferBaseEXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_gettransformfeedbackvaryingext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbackVaryingEXT) mogl_glunsupported("glGetTransformFeedbackVaryingEXT");
	glGetTransformFeedbackVaryingEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_arrayelementext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glArrayElementEXT) mogl_glunsupported("glArrayElementEXT");
	glArrayElementEXT((GLint)mxGetScalar(prhs[0]));

}

void gl_colorpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorPointerEXT) mogl_glunsupported("glColorPointerEXT");
	glColorPointerEXT((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_drawarraysext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawArraysEXT) mogl_glunsupported("glDrawArraysEXT");
	glDrawArraysEXT((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_edgeflagpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEdgeFlagPointerEXT) mogl_glunsupported("glEdgeFlagPointerEXT");
	glEdgeFlagPointerEXT((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLboolean*)mxGetData(prhs[2]));

}

void gl_indexpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexPointerEXT) mogl_glunsupported("glIndexPointerEXT");
	glIndexPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_normalpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalPointerEXT) mogl_glunsupported("glNormalPointerEXT");
	glNormalPointerEXT((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_texcoordpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordPointerEXT) mogl_glunsupported("glTexCoordPointerEXT");
	glTexCoordPointerEXT((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_vertexpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexPointerEXT) mogl_glunsupported("glVertexPointerEXT");
	glVertexPointerEXT((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_vertexattribl1dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1dEXT) mogl_glunsupported("glVertexAttribL1dEXT");
	glVertexAttribL1dEXT((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattribl2dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2dEXT) mogl_glunsupported("glVertexAttribL2dEXT");
	glVertexAttribL2dEXT((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattribl3dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3dEXT) mogl_glunsupported("glVertexAttribL3dEXT");
	glVertexAttribL3dEXT((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattribl4dext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4dEXT) mogl_glunsupported("glVertexAttribL4dEXT");
	glVertexAttribL4dEXT((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattribl1dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1dvEXT) mogl_glunsupported("glVertexAttribL1dvEXT");
	glVertexAttribL1dvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl2dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2dvEXT) mogl_glunsupported("glVertexAttribL2dvEXT");
	glVertexAttribL2dvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl3dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3dvEXT) mogl_glunsupported("glVertexAttribL3dvEXT");
	glVertexAttribL3dvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattribl4dvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4dvEXT) mogl_glunsupported("glVertexAttribL4dvEXT");
	glVertexAttribL4dvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattriblpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribLPointerEXT) mogl_glunsupported("glVertexAttribLPointerEXT");
	glVertexAttribLPointerEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_getvertexattribldvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribLdvEXT) mogl_glunsupported("glGetVertexAttribLdvEXT");
	glGetVertexAttribLdvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_beginvertexshaderext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginVertexShaderEXT) mogl_glunsupported("glBeginVertexShaderEXT");
	glBeginVertexShaderEXT();

}

void gl_endvertexshaderext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndVertexShaderEXT) mogl_glunsupported("glEndVertexShaderEXT");
	glEndVertexShaderEXT();

}

void gl_bindvertexshaderext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVertexShaderEXT) mogl_glunsupported("glBindVertexShaderEXT");
	glBindVertexShaderEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_genvertexshadersext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenVertexShadersEXT) mogl_glunsupported("glGenVertexShadersEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenVertexShadersEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_deletevertexshaderext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteVertexShaderEXT) mogl_glunsupported("glDeleteVertexShaderEXT");
	glDeleteVertexShaderEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_shaderop1ext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShaderOp1EXT) mogl_glunsupported("glShaderOp1EXT");
	glShaderOp1EXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_shaderop2ext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShaderOp2EXT) mogl_glunsupported("glShaderOp2EXT");
	glShaderOp2EXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_shaderop3ext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glShaderOp3EXT) mogl_glunsupported("glShaderOp3EXT");
	glShaderOp3EXT((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_swizzleext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSwizzleEXT) mogl_glunsupported("glSwizzleEXT");
	glSwizzleEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]));

}

void gl_writemaskext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWriteMaskEXT) mogl_glunsupported("glWriteMaskEXT");
	glWriteMaskEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]));

}

void gl_insertcomponentext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInsertComponentEXT) mogl_glunsupported("glInsertComponentEXT");
	glInsertComponentEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_extractcomponentext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glExtractComponentEXT) mogl_glunsupported("glExtractComponentEXT");
	glExtractComponentEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_gensymbolsext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenSymbolsEXT) mogl_glunsupported("glGenSymbolsEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenSymbolsEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_setinvariantext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetInvariantEXT) mogl_glunsupported("glSetInvariantEXT");
	glSetInvariantEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_setlocalconstantext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetLocalConstantEXT) mogl_glunsupported("glSetLocalConstantEXT");
	glSetLocalConstantEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_variantbvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantbvEXT) mogl_glunsupported("glVariantbvEXT");
	glVariantbvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_variantsvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantsvEXT) mogl_glunsupported("glVariantsvEXT");
	glVariantsvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_variantivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantivEXT) mogl_glunsupported("glVariantivEXT");
	glVariantivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_variantfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantfvEXT) mogl_glunsupported("glVariantfvEXT");
	glVariantfvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_variantdvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantdvEXT) mogl_glunsupported("glVariantdvEXT");
	glVariantdvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_variantubvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantubvEXT) mogl_glunsupported("glVariantubvEXT");
	glVariantubvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_variantusvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantusvEXT) mogl_glunsupported("glVariantusvEXT");
	glVariantusvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_variantuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantuivEXT) mogl_glunsupported("glVariantuivEXT");
	glVariantuivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_variantpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVariantPointerEXT) mogl_glunsupported("glVariantPointerEXT");
	glVariantPointerEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_enablevariantclientstateext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEnableVariantClientStateEXT) mogl_glunsupported("glEnableVariantClientStateEXT");
	glEnableVariantClientStateEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_disablevariantclientstateext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDisableVariantClientStateEXT) mogl_glunsupported("glDisableVariantClientStateEXT");
	glDisableVariantClientStateEXT((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindlightparameterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindLightParameterEXT) mogl_glunsupported("glBindLightParameterEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glBindLightParameterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_bindmaterialparameterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindMaterialParameterEXT) mogl_glunsupported("glBindMaterialParameterEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glBindMaterialParameterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_bindtexgenparameterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTexGenParameterEXT) mogl_glunsupported("glBindTexGenParameterEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glBindTexGenParameterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_bindtextureunitparameterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTextureUnitParameterEXT) mogl_glunsupported("glBindTextureUnitParameterEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glBindTextureUnitParameterEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_bindparameterext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindParameterEXT) mogl_glunsupported("glBindParameterEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glBindParameterEXT((GLenum)mxGetScalar(prhs[0]));

}

void gl_isvariantenabledext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsVariantEnabledEXT) mogl_glunsupported("glIsVariantEnabledEXT");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsVariantEnabledEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_getvariantbooleanvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVariantBooleanvEXT) mogl_glunsupported("glGetVariantBooleanvEXT");
	glGetVariantBooleanvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_getvariantintegervext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVariantIntegervEXT) mogl_glunsupported("glGetVariantIntegervEXT");
	glGetVariantIntegervEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvariantfloatvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVariantFloatvEXT) mogl_glunsupported("glGetVariantFloatvEXT");
	glGetVariantFloatvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getinvariantbooleanvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInvariantBooleanvEXT) mogl_glunsupported("glGetInvariantBooleanvEXT");
	glGetInvariantBooleanvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_getinvariantintegervext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInvariantIntegervEXT) mogl_glunsupported("glGetInvariantIntegervEXT");
	glGetInvariantIntegervEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getinvariantfloatvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetInvariantFloatvEXT) mogl_glunsupported("glGetInvariantFloatvEXT");
	glGetInvariantFloatvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getlocalconstantbooleanvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetLocalConstantBooleanvEXT) mogl_glunsupported("glGetLocalConstantBooleanvEXT");
	glGetLocalConstantBooleanvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_getlocalconstantintegervext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetLocalConstantIntegervEXT) mogl_glunsupported("glGetLocalConstantIntegervEXT");
	glGetLocalConstantIntegervEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getlocalconstantfloatvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetLocalConstantFloatvEXT) mogl_glunsupported("glGetLocalConstantFloatvEXT");
	glGetLocalConstantFloatvEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexweightfext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexWeightfEXT) mogl_glunsupported("glVertexWeightfEXT");
	glVertexWeightfEXT((GLfloat)mxGetScalar(prhs[0]));

}

void gl_vertexweightfvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexWeightfvEXT) mogl_glunsupported("glVertexWeightfvEXT");
	glVertexWeightfvEXT((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertexweightpointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexWeightPointerEXT) mogl_glunsupported("glVertexWeightPointerEXT");
	glVertexWeightPointerEXT((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_importsyncext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glImportSyncEXT) mogl_glunsupported("glImportSyncEXT");
	plhs[0]=mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
	*(double *)mxGetData(plhs[0])=PsychPtrToDouble((void*) glImportSyncEXT((GLenum)mxGetScalar(prhs[0]),
		(GLintptr)mxGetScalar(prhs[1]),
		(GLbitfield)mxGetScalar(prhs[2])));

}

void gl_frameterminatorgremedy( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFrameTerminatorGREMEDY) mogl_glunsupported("glFrameTerminatorGREMEDY");
	glFrameTerminatorGREMEDY();

}

void gl_stringmarkergremedy( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStringMarkerGREMEDY) mogl_glunsupported("glStringMarkerGREMEDY");
	glStringMarkerGREMEDY((GLsizei)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_imagetransformparameterihp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glImageTransformParameteriHP) mogl_glunsupported("glImageTransformParameteriHP");
	glImageTransformParameteriHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_imagetransformparameterfhp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glImageTransformParameterfHP) mogl_glunsupported("glImageTransformParameterfHP");
	glImageTransformParameterfHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_imagetransformparameterivhp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glImageTransformParameterivHP) mogl_glunsupported("glImageTransformParameterivHP");
	glImageTransformParameterivHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_imagetransformparameterfvhp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glImageTransformParameterfvHP) mogl_glunsupported("glImageTransformParameterfvHP");
	glImageTransformParameterfvHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getimagetransformparameterivhp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetImageTransformParameterivHP) mogl_glunsupported("glGetImageTransformParameterivHP");
	glGetImageTransformParameterivHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getimagetransformparameterfvhp( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetImageTransformParameterfvHP) mogl_glunsupported("glGetImageTransformParameterfvHP");
	glGetImageTransformParameterfvHP((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_multimodedrawarraysibm( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiModeDrawArraysIBM) mogl_glunsupported("glMultiModeDrawArraysIBM");
	glMultiModeDrawArraysIBM((const GLenum*)mxGetData(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(const GLsizei*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_multimodedrawelementsibm( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiModeDrawElementsIBM) mogl_glunsupported("glMultiModeDrawElementsIBM");
	glMultiModeDrawElementsIBM((const GLenum*)mxGetData(prhs[0]),
		(const GLsizei*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_synctextureintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSyncTextureINTEL) mogl_glunsupported("glSyncTextureINTEL");
	glSyncTextureINTEL((GLuint)mxGetScalar(prhs[0]));

}

void gl_unmaptexture2dintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUnmapTexture2DINTEL) mogl_glunsupported("glUnmapTexture2DINTEL");
	glUnmapTexture2DINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_maptexture2dintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapTexture2DINTEL) mogl_glunsupported("glMapTexture2DINTEL");
	glMapTexture2DINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLbitfield)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]),
		(GLenum*)mxGetData(prhs[4]));

}

void gl_beginperfqueryintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginPerfQueryINTEL) mogl_glunsupported("glBeginPerfQueryINTEL");
	glBeginPerfQueryINTEL((GLuint)mxGetScalar(prhs[0]));

}

void gl_createperfqueryintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCreatePerfQueryINTEL) mogl_glunsupported("glCreatePerfQueryINTEL");
	glCreatePerfQueryINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deleteperfqueryintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeletePerfQueryINTEL) mogl_glunsupported("glDeletePerfQueryINTEL");
	glDeletePerfQueryINTEL((GLuint)mxGetScalar(prhs[0]));

}

void gl_endperfqueryintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndPerfQueryINTEL) mogl_glunsupported("glEndPerfQueryINTEL");
	glEndPerfQueryINTEL((GLuint)mxGetScalar(prhs[0]));

}

void gl_getfirstperfqueryidintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFirstPerfQueryIdINTEL) mogl_glunsupported("glGetFirstPerfQueryIdINTEL");
	glGetFirstPerfQueryIdINTEL((GLuint*)mxGetData(prhs[0]));

}

void gl_getnextperfqueryidintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNextPerfQueryIdINTEL) mogl_glunsupported("glGetNextPerfQueryIdINTEL");
	glGetNextPerfQueryIdINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getperfcounterinfointel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfCounterInfoINTEL) mogl_glunsupported("glGetPerfCounterInfoINTEL");
	glGetPerfCounterInfoINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLchar*)mxGetData(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLchar*)mxGetData(prhs[5]),
		(GLuint*)mxGetData(prhs[6]),
		(GLuint*)mxGetData(prhs[7]),
		(GLuint*)mxGetData(prhs[8]),
		(GLuint*)mxGetData(prhs[9]),
		(GLuint64*)mxGetData(prhs[10]));

}

void gl_getperfquerydataintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfQueryDataINTEL) mogl_glunsupported("glGetPerfQueryDataINTEL");
	glGetPerfQueryDataINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]),
		(GLuint*)mxGetData(prhs[4]));

}

void gl_getperfqueryidbynameintel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfQueryIdByNameINTEL) mogl_glunsupported("glGetPerfQueryIdByNameINTEL");
	glGetPerfQueryIdByNameINTEL((GLchar*)mxGetData(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getperfqueryinfointel( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPerfQueryInfoINTEL) mogl_glunsupported("glGetPerfQueryInfoINTEL");
	glGetPerfQueryInfoINTEL((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLchar*)mxGetData(prhs[2]),
		(GLuint*)mxGetData(prhs[3]),
		(GLuint*)mxGetData(prhs[4]),
		(GLuint*)mxGetData(prhs[5]),
		(GLuint*)mxGetData(prhs[6]));

}

void gl_resizebuffersmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResizeBuffersMESA) mogl_glunsupported("glResizeBuffersMESA");
	glResizeBuffersMESA();

}

void gl_windowpos2dmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2dMESA) mogl_glunsupported("glWindowPos2dMESA");
	glWindowPos2dMESA((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_windowpos2dvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2dvMESA) mogl_glunsupported("glWindowPos2dvMESA");
	glWindowPos2dvMESA((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos2fmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2fMESA) mogl_glunsupported("glWindowPos2fMESA");
	glWindowPos2fMESA((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_windowpos2fvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2fvMESA) mogl_glunsupported("glWindowPos2fvMESA");
	glWindowPos2fvMESA((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos2imesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2iMESA) mogl_glunsupported("glWindowPos2iMESA");
	glWindowPos2iMESA((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_windowpos2ivmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2ivMESA) mogl_glunsupported("glWindowPos2ivMESA");
	glWindowPos2ivMESA((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos2smesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2sMESA) mogl_glunsupported("glWindowPos2sMESA");
	glWindowPos2sMESA((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_windowpos2svmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos2svMESA) mogl_glunsupported("glWindowPos2svMESA");
	glWindowPos2svMESA((const GLshort*)mxGetData(prhs[0]));

}

void gl_windowpos3dmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3dMESA) mogl_glunsupported("glWindowPos3dMESA");
	glWindowPos3dMESA((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_windowpos3dvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3dvMESA) mogl_glunsupported("glWindowPos3dvMESA");
	glWindowPos3dvMESA((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos3fmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3fMESA) mogl_glunsupported("glWindowPos3fMESA");
	glWindowPos3fMESA((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_windowpos3fvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3fvMESA) mogl_glunsupported("glWindowPos3fvMESA");
	glWindowPos3fvMESA((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos3imesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3iMESA) mogl_glunsupported("glWindowPos3iMESA");
	glWindowPos3iMESA((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_windowpos3ivmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3ivMESA) mogl_glunsupported("glWindowPos3ivMESA");
	glWindowPos3ivMESA((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos3smesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3sMESA) mogl_glunsupported("glWindowPos3sMESA");
	glWindowPos3sMESA((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_windowpos3svmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos3svMESA) mogl_glunsupported("glWindowPos3svMESA");
	glWindowPos3svMESA((const GLshort*)mxGetData(prhs[0]));

}

void gl_windowpos4dmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4dMESA) mogl_glunsupported("glWindowPos4dMESA");
	glWindowPos4dMESA((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_windowpos4dvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4dvMESA) mogl_glunsupported("glWindowPos4dvMESA");
	glWindowPos4dvMESA((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos4fmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4fMESA) mogl_glunsupported("glWindowPos4fMESA");
	glWindowPos4fMESA((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_windowpos4fvmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4fvMESA) mogl_glunsupported("glWindowPos4fvMESA");
	glWindowPos4fvMESA((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos4imesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4iMESA) mogl_glunsupported("glWindowPos4iMESA");
	glWindowPos4iMESA((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_windowpos4ivmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4ivMESA) mogl_glunsupported("glWindowPos4ivMESA");
	glWindowPos4ivMESA((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos4smesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4sMESA) mogl_glunsupported("glWindowPos4sMESA");
	glWindowPos4sMESA((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_windowpos4svmesa( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWindowPos4svMESA) mogl_glunsupported("glWindowPos4svMESA");
	glWindowPos4svMESA((const GLshort*)mxGetData(prhs[0]));

}

void gl_beginconditionalrendernvx( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginConditionalRenderNVX) mogl_glunsupported("glBeginConditionalRenderNVX");
	glBeginConditionalRenderNVX((GLuint)mxGetScalar(prhs[0]));

}

void gl_endconditionalrendernvx( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndConditionalRenderNVX) mogl_glunsupported("glEndConditionalRenderNVX");
	glEndConditionalRenderNVX();

}

void gl_multidrawarraysindirectbindlessnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysIndirectBindlessNV) mogl_glunsupported("glMultiDrawArraysIndirectBindlessNV");
	glMultiDrawArraysIndirectBindlessNV((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_multidrawelementsindirectbindlessnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsIndirectBindlessNV) mogl_glunsupported("glMultiDrawElementsIndirectBindlessNV");
	glMultiDrawElementsIndirectBindlessNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_multidrawarraysindirectbindlesscountnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawArraysIndirectBindlessCountNV) mogl_glunsupported("glMultiDrawArraysIndirectBindlessCountNV");
	glMultiDrawArraysIndirectBindlessCountNV((GLenum)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_multidrawelementsindirectbindlesscountnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMultiDrawElementsIndirectBindlessCountNV) mogl_glunsupported("glMultiDrawElementsIndirectBindlessCountNV");
	glMultiDrawElementsIndirectBindlessCountNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_gettexturehandlenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureHandleNV) mogl_glunsupported("glGetTextureHandleNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetTextureHandleNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_gettexturesamplerhandlenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTextureSamplerHandleNV) mogl_glunsupported("glGetTextureSamplerHandleNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetTextureSamplerHandleNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_maketexturehandleresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeTextureHandleResidentNV) mogl_glunsupported("glMakeTextureHandleResidentNV");
	glMakeTextureHandleResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_maketexturehandlenonresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeTextureHandleNonResidentNV) mogl_glunsupported("glMakeTextureHandleNonResidentNV");
	glMakeTextureHandleNonResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_getimagehandlenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetImageHandleNV) mogl_glunsupported("glGetImageHandleNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetImageHandleNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_makeimagehandleresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeImageHandleResidentNV) mogl_glunsupported("glMakeImageHandleResidentNV");
	glMakeImageHandleResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_makeimagehandlenonresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeImageHandleNonResidentNV) mogl_glunsupported("glMakeImageHandleNonResidentNV");
	glMakeImageHandleNonResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_uniformhandleui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformHandleui64NV) mogl_glunsupported("glUniformHandleui64NV");
	glUniformHandleui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[1])));

}

void gl_uniformhandleui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformHandleui64vNV) mogl_glunsupported("glUniformHandleui64vNV");
	glUniformHandleui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64*)mxGetData(prhs[2]));

}

void gl_programuniformhandleui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformHandleui64NV) mogl_glunsupported("glProgramUniformHandleui64NV");
	glProgramUniformHandleui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64) *((GLuint64*) mxGetData(prhs[2])));

}

void gl_programuniformhandleui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformHandleui64vNV) mogl_glunsupported("glProgramUniformHandleui64vNV");
	glProgramUniformHandleui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64*)mxGetData(prhs[3]));

}

void gl_istexturehandleresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTextureHandleResidentNV) mogl_glunsupported("glIsTextureHandleResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTextureHandleResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_isimagehandleresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsImageHandleResidentNV) mogl_glunsupported("glIsImageHandleResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsImageHandleResidentNV((GLuint64) *((GLuint64*) mxGetData(prhs[0])));

}

void gl_blendparameterinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendParameteriNV) mogl_glunsupported("glBlendParameteriNV");
	glBlendParameteriNV((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_blendbarriernv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBlendBarrierNV) mogl_glunsupported("glBlendBarrierNV");
	glBlendBarrierNV();

}

void gl_copyimagesubdatanv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyImageSubDataNV) mogl_glunsupported("glCopyImageSubDataNV");
	glCopyImageSubDataNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLint)mxGetScalar(prhs[8]),
		(GLint)mxGetScalar(prhs[9]),
		(GLint)mxGetScalar(prhs[10]),
		(GLint)mxGetScalar(prhs[11]),
		(GLsizei)mxGetScalar(prhs[12]),
		(GLsizei)mxGetScalar(prhs[13]),
		(GLsizei)mxGetScalar(prhs[14]));

}

void gl_depthrangednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthRangedNV) mogl_glunsupported("glDepthRangedNV");
	glDepthRangedNV((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_cleardepthdnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glClearDepthdNV) mogl_glunsupported("glClearDepthdNV");
	glClearDepthdNV((GLdouble)mxGetScalar(prhs[0]));

}

void gl_depthboundsdnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDepthBoundsdNV) mogl_glunsupported("glDepthBoundsdNV");
	glDepthBoundsdNV((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_drawtexturenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTextureNV) mogl_glunsupported("glDrawTextureNV");
	glDrawTextureNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]),
		(GLfloat)mxGetScalar(prhs[10]));

}

void gl_mapcontrolpointsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapControlPointsNV) mogl_glunsupported("glMapControlPointsNV");
	glMapControlPointsNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]),
		(const void*)mxGetData(prhs[8]));

}

void gl_mapparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapParameterivNV) mogl_glunsupported("glMapParameterivNV");
	glMapParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_mapparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapParameterfvNV) mogl_glunsupported("glMapParameterfvNV");
	glMapParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getmapcontrolpointsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapControlPointsNV) mogl_glunsupported("glGetMapControlPointsNV");
	glGetMapControlPointsNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLboolean)mxGetScalar(prhs[5]),
		(void*)mxGetData(prhs[6]));

}

void gl_getmapparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapParameterivNV) mogl_glunsupported("glGetMapParameterivNV");
	glGetMapParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getmapparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapParameterfvNV) mogl_glunsupported("glGetMapParameterfvNV");
	glGetMapParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getmapattribparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapAttribParameterivNV) mogl_glunsupported("glGetMapAttribParameterivNV");
	glGetMapAttribParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getmapattribparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMapAttribParameterfvNV) mogl_glunsupported("glGetMapAttribParameterfvNV");
	glGetMapAttribParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_evalmapsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEvalMapsNV) mogl_glunsupported("glEvalMapsNV");
	glEvalMapsNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_getmultisamplefvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMultisamplefvNV) mogl_glunsupported("glGetMultisamplefvNV");
	glGetMultisamplefvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_samplemaskindexednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMaskIndexedNV) mogl_glunsupported("glSampleMaskIndexedNV");
	glSampleMaskIndexedNV((GLuint)mxGetScalar(prhs[0]),
		(GLbitfield)mxGetScalar(prhs[1]));

}

void gl_texrenderbuffernv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexRenderbufferNV) mogl_glunsupported("glTexRenderbufferNV");
	glTexRenderbufferNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletefencesnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteFencesNV) mogl_glunsupported("glDeleteFencesNV");
	glDeleteFencesNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genfencesnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenFencesNV) mogl_glunsupported("glGenFencesNV");
	glGenFencesNV((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isfencenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsFenceNV) mogl_glunsupported("glIsFenceNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsFenceNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_testfencenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTestFenceNV) mogl_glunsupported("glTestFenceNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glTestFenceNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_getfenceivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFenceivNV) mogl_glunsupported("glGetFenceivNV");
	glGetFenceivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_finishfencenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinishFenceNV) mogl_glunsupported("glFinishFenceNV");
	glFinishFenceNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_setfencenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSetFenceNV) mogl_glunsupported("glSetFenceNV");
	glSetFenceNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_programnamedparameter4fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramNamedParameter4fNV) mogl_glunsupported("glProgramNamedParameter4fNV");
	glProgramNamedParameter4fNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]));

}

void gl_programnamedparameter4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramNamedParameter4fvNV) mogl_glunsupported("glProgramNamedParameter4fvNV");
	glProgramNamedParameter4fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_programnamedparameter4dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramNamedParameter4dNV) mogl_glunsupported("glProgramNamedParameter4dNV");
	glProgramNamedParameter4dNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void gl_programnamedparameter4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramNamedParameter4dvNV) mogl_glunsupported("glProgramNamedParameter4dvNV");
	glProgramNamedParameter4dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_getprogramnamedparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramNamedParameterfvNV) mogl_glunsupported("glGetProgramNamedParameterfvNV");
	glGetProgramNamedParameterfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getprogramnamedparameterdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramNamedParameterdvNV) mogl_glunsupported("glGetProgramNamedParameterdvNV");
	glGetProgramNamedParameterdvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_renderbufferstoragemultisamplecoveragenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRenderbufferStorageMultisampleCoverageNV) mogl_glunsupported("glRenderbufferStorageMultisampleCoverageNV");
	glRenderbufferStorageMultisampleCoverageNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_programvertexlimitnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramVertexLimitNV) mogl_glunsupported("glProgramVertexLimitNV");
	glProgramVertexLimitNV((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_framebuffertextureext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureEXT) mogl_glunsupported("glFramebufferTextureEXT");
	glFramebufferTextureEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_framebuffertexturefaceext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFramebufferTextureFaceEXT) mogl_glunsupported("glFramebufferTextureFaceEXT");
	glFramebufferTextureFaceEXT((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_programlocalparameteri4inv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameterI4iNV) mogl_glunsupported("glProgramLocalParameterI4iNV");
	glProgramLocalParameterI4iNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_programlocalparameteri4ivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameterI4ivNV) mogl_glunsupported("glProgramLocalParameterI4ivNV");
	glProgramLocalParameterI4ivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_programlocalparametersi4ivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParametersI4ivNV) mogl_glunsupported("glProgramLocalParametersI4ivNV");
	glProgramLocalParametersI4ivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programlocalparameteri4uinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameterI4uiNV) mogl_glunsupported("glProgramLocalParameterI4uiNV");
	glProgramLocalParameterI4uiNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_programlocalparameteri4uivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParameterI4uivNV) mogl_glunsupported("glProgramLocalParameterI4uivNV");
	glProgramLocalParameterI4uivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_programlocalparametersi4uivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramLocalParametersI4uivNV) mogl_glunsupported("glProgramLocalParametersI4uivNV");
	glProgramLocalParametersI4uivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_programenvparameteri4inv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameterI4iNV) mogl_glunsupported("glProgramEnvParameterI4iNV");
	glProgramEnvParameterI4iNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void gl_programenvparameteri4ivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameterI4ivNV) mogl_glunsupported("glProgramEnvParameterI4ivNV");
	glProgramEnvParameterI4ivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_programenvparametersi4ivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParametersI4ivNV) mogl_glunsupported("glProgramEnvParametersI4ivNV");
	glProgramEnvParametersI4ivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_programenvparameteri4uinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameterI4uiNV) mogl_glunsupported("glProgramEnvParameterI4uiNV");
	glProgramEnvParameterI4uiNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]));

}

void gl_programenvparameteri4uivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParameterI4uivNV) mogl_glunsupported("glProgramEnvParameterI4uivNV");
	glProgramEnvParameterI4uivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_programenvparametersi4uivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramEnvParametersI4uivNV) mogl_glunsupported("glProgramEnvParametersI4uivNV");
	glProgramEnvParametersI4uivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint*)mxGetData(prhs[3]));

}

void gl_genocclusionqueriesnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenOcclusionQueriesNV) mogl_glunsupported("glGenOcclusionQueriesNV");
	glGenOcclusionQueriesNV((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deleteocclusionqueriesnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteOcclusionQueriesNV) mogl_glunsupported("glDeleteOcclusionQueriesNV");
	glDeleteOcclusionQueriesNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_isocclusionquerynv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsOcclusionQueryNV) mogl_glunsupported("glIsOcclusionQueryNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsOcclusionQueryNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_beginocclusionquerynv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginOcclusionQueryNV) mogl_glunsupported("glBeginOcclusionQueryNV");
	glBeginOcclusionQueryNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_endocclusionquerynv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndOcclusionQueryNV) mogl_glunsupported("glEndOcclusionQueryNV");
	glEndOcclusionQueryNV();

}

void gl_getocclusionqueryivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetOcclusionQueryivNV) mogl_glunsupported("glGetOcclusionQueryivNV");
	glGetOcclusionQueryivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getocclusionqueryuivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetOcclusionQueryuivNV) mogl_glunsupported("glGetOcclusionQueryuivNV");
	glGetOcclusionQueryuivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_programbufferparametersfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramBufferParametersfvNV) mogl_glunsupported("glProgramBufferParametersfvNV");
	glProgramBufferParametersfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

void gl_programbufferparametersiivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramBufferParametersIivNV) mogl_glunsupported("glProgramBufferParametersIivNV");
	glProgramBufferParametersIivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLint*)mxGetData(prhs[4]));

}

void gl_programbufferparametersiuivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramBufferParametersIuivNV) mogl_glunsupported("glProgramBufferParametersIuivNV");
	glProgramBufferParametersIuivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLuint*)mxGetData(prhs[4]));

}

void gl_genpathsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenPathsNV) mogl_glunsupported("glGenPathsNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenPathsNV((GLsizei)mxGetScalar(prhs[0]));

}

void gl_deletepathsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeletePathsNV) mogl_glunsupported("glDeletePathsNV");
	glDeletePathsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_ispathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsPathNV) mogl_glunsupported("glIsPathNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsPathNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_pathcommandsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathCommandsNV) mogl_glunsupported("glPathCommandsNV");
	glPathCommandsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_pathcoordsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathCoordsNV) mogl_glunsupported("glPathCoordsNV");
	glPathCoordsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_pathsubcommandsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathSubCommandsNV) mogl_glunsupported("glPathSubCommandsNV");
	glPathSubCommandsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const GLubyte*)mxGetData(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const void*)mxGetData(prhs[7]));

}

void gl_pathsubcoordsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathSubCoordsNV) mogl_glunsupported("glPathSubCoordsNV");
	glPathSubCoordsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_pathstringnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathStringNV) mogl_glunsupported("glPathStringNV");
	glPathStringNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]));

}

void gl_pathglyphsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathGlyphsNV) mogl_glunsupported("glPathGlyphsNV");
	glPathGlyphsNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const void*)mxGetData(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]));

}

void gl_pathglyphrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathGlyphRangeNV) mogl_glunsupported("glPathGlyphRangeNV");
	glPathGlyphRangeNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLbitfield)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]));

}

void gl_weightpathsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glWeightPathsNV) mogl_glunsupported("glWeightPathsNV");
	glWeightPathsNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_copypathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyPathNV) mogl_glunsupported("glCopyPathNV");
	glCopyPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_interpolatepathsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glInterpolatePathsNV) mogl_glunsupported("glInterpolatePathsNV");
	glInterpolatePathsNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_transformpathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTransformPathNV) mogl_glunsupported("glTransformPathNV");
	glTransformPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_pathparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathParameterivNV) mogl_glunsupported("glPathParameterivNV");
	glPathParameterivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_pathparameterinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathParameteriNV) mogl_glunsupported("glPathParameteriNV");
	glPathParameteriNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_pathparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathParameterfvNV) mogl_glunsupported("glPathParameterfvNV");
	glPathParameterfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_pathparameterfnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathParameterfNV) mogl_glunsupported("glPathParameterfNV");
	glPathParameterfNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_pathdasharraynv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathDashArrayNV) mogl_glunsupported("glPathDashArrayNV");
	glPathDashArrayNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_pathstencilfuncnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathStencilFuncNV) mogl_glunsupported("glPathStencilFuncNV");
	glPathStencilFuncNV((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_pathstencildepthoffsetnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathStencilDepthOffsetNV) mogl_glunsupported("glPathStencilDepthOffsetNV");
	glPathStencilDepthOffsetNV((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_stencilfillpathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilFillPathNV) mogl_glunsupported("glStencilFillPathNV");
	glStencilFillPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_stencilstrokepathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilStrokePathNV) mogl_glunsupported("glStencilStrokePathNV");
	glStencilStrokePathNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_stencilfillpathinstancednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilFillPathInstancedNV) mogl_glunsupported("glStencilFillPathInstancedNV");
	glStencilFillPathInstancedNV((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const GLfloat*)mxGetData(prhs[7]));

}

void gl_stencilstrokepathinstancednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glStencilStrokePathInstancedNV) mogl_glunsupported("glStencilStrokePathInstancedNV");
	glStencilStrokePathInstancedNV((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLuint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const GLfloat*)mxGetData(prhs[7]));

}

void gl_pathcoverdepthfuncnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathCoverDepthFuncNV) mogl_glunsupported("glPathCoverDepthFuncNV");
	glPathCoverDepthFuncNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_pathcolorgennv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathColorGenNV) mogl_glunsupported("glPathColorGenNV");
	glPathColorGenNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_pathtexgennv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathTexGenNV) mogl_glunsupported("glPathTexGenNV");
	glPathTexGenNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_pathfoggennv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPathFogGenNV) mogl_glunsupported("glPathFogGenNV");
	glPathFogGenNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_coverfillpathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCoverFillPathNV) mogl_glunsupported("glCoverFillPathNV");
	glCoverFillPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_coverstrokepathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCoverStrokePathNV) mogl_glunsupported("glCoverStrokePathNV");
	glCoverStrokePathNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_coverfillpathinstancednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCoverFillPathInstancedNV) mogl_glunsupported("glCoverFillPathInstancedNV");
	glCoverFillPathInstancedNV((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLfloat*)mxGetData(prhs[6]));

}

void gl_coverstrokepathinstancednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCoverStrokePathInstancedNV) mogl_glunsupported("glCoverStrokePathInstancedNV");
	glCoverStrokePathInstancedNV((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLfloat*)mxGetData(prhs[6]));

}

void gl_getpathparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathParameterivNV) mogl_glunsupported("glGetPathParameterivNV");
	glGetPathParameterivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpathparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathParameterfvNV) mogl_glunsupported("glGetPathParameterfvNV");
	glGetPathParameterfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getpathcommandsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathCommandsNV) mogl_glunsupported("glGetPathCommandsNV");
	glGetPathCommandsNV((GLuint)mxGetScalar(prhs[0]),
		(GLubyte*)mxGetData(prhs[1]));

}

void gl_getpathcoordsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathCoordsNV) mogl_glunsupported("glGetPathCoordsNV");
	glGetPathCoordsNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_getpathdasharraynv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathDashArrayNV) mogl_glunsupported("glGetPathDashArrayNV");
	glGetPathDashArrayNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_getpathmetricsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathMetricsNV) mogl_glunsupported("glGetPathMetricsNV");
	glGetPathMetricsNV((GLbitfield)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLfloat*)mxGetData(prhs[6]));

}

void gl_getpathmetricrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathMetricRangeNV) mogl_glunsupported("glGetPathMetricRangeNV");
	glGetPathMetricRangeNV((GLbitfield)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]));

}

void gl_getpathspacingnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathSpacingNV) mogl_glunsupported("glGetPathSpacingNV");
	glGetPathSpacingNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const void*)mxGetData(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLfloat*)mxGetData(prhs[8]));

}

void gl_getpathcolorgenivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathColorGenivNV) mogl_glunsupported("glGetPathColorGenivNV");
	glGetPathColorGenivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpathcolorgenfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathColorGenfvNV) mogl_glunsupported("glGetPathColorGenfvNV");
	glGetPathColorGenfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getpathtexgenivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathTexGenivNV) mogl_glunsupported("glGetPathTexGenivNV");
	glGetPathTexGenivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpathtexgenfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathTexGenfvNV) mogl_glunsupported("glGetPathTexGenfvNV");
	glGetPathTexGenfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_ispointinfillpathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsPointInFillPathNV) mogl_glunsupported("glIsPointInFillPathNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsPointInFillPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_ispointinstrokepathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsPointInStrokePathNV) mogl_glunsupported("glIsPointInStrokePathNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsPointInStrokePathNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_getpathlengthnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetPathLengthNV) mogl_glunsupported("glGetPathLengthNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetPathLengthNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_pointalongpathnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointAlongPathNV) mogl_glunsupported("glPointAlongPathNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glPointAlongPathNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]),
		(GLfloat*)mxGetData(prhs[5]),
		(GLfloat*)mxGetData(prhs[6]),
		(GLfloat*)mxGetData(prhs[7]));

}

void gl_pixeldatarangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelDataRangeNV) mogl_glunsupported("glPixelDataRangeNV");
	glPixelDataRangeNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const void*)mxGetData(prhs[2]));

}

void gl_flushpixeldatarangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushPixelDataRangeNV) mogl_glunsupported("glFlushPixelDataRangeNV");
	glFlushPixelDataRangeNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_pointparameterinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameteriNV) mogl_glunsupported("glPointParameteriNV");
	glPointParameteriNV((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pointparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPointParameterivNV) mogl_glunsupported("glPointParameterivNV");
	glPointParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_presentframekeyednv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPresentFrameKeyedNV) mogl_glunsupported("glPresentFrameKeyedNV");
	glPresentFrameKeyedNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLuint)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLuint)mxGetScalar(prhs[9]),
		(GLuint)mxGetScalar(prhs[10]));

}

void gl_presentframedualfillnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPresentFrameDualFillNV) mogl_glunsupported("glPresentFrameDualFillNV");
	glPresentFrameDualFillNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLuint)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(GLuint)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(GLuint)mxGetScalar(prhs[10]),
		(GLenum)mxGetScalar(prhs[11]),
		(GLuint)mxGetScalar(prhs[12]));

}

void gl_getvideoivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoivNV) mogl_glunsupported("glGetVideoivNV");
	glGetVideoivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvideouivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideouivNV) mogl_glunsupported("glGetVideouivNV");
	glGetVideouivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_getvideoi64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoi64vNV) mogl_glunsupported("glGetVideoi64vNV");
	glGetVideoi64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64EXT*)mxGetData(prhs[2]));

}

void gl_getvideoui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoui64vNV) mogl_glunsupported("glGetVideoui64vNV");
	glGetVideoui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_primitiverestartnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPrimitiveRestartNV) mogl_glunsupported("glPrimitiveRestartNV");
	glPrimitiveRestartNV();

}

void gl_primitiverestartindexnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPrimitiveRestartIndexNV) mogl_glunsupported("glPrimitiveRestartIndexNV");
	glPrimitiveRestartIndexNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_combinerparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerParameterfvNV) mogl_glunsupported("glCombinerParameterfvNV");
	glCombinerParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_combinerparameterfnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerParameterfNV) mogl_glunsupported("glCombinerParameterfNV");
	glCombinerParameterfNV((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_combinerparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerParameterivNV) mogl_glunsupported("glCombinerParameterivNV");
	glCombinerParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_combinerparameterinv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerParameteriNV) mogl_glunsupported("glCombinerParameteriNV");
	glCombinerParameteriNV((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_combinerinputnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerInputNV) mogl_glunsupported("glCombinerInputNV");
	glCombinerInputNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]));

}

void gl_combineroutputnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerOutputNV) mogl_glunsupported("glCombinerOutputNV");
	glCombinerOutputNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]),
		(GLboolean)mxGetScalar(prhs[8]),
		(GLboolean)mxGetScalar(prhs[9]));

}

void gl_finalcombinerinputnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinalCombinerInputNV) mogl_glunsupported("glFinalCombinerInputNV");
	glFinalCombinerInputNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_getcombinerinputparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCombinerInputParameterfvNV) mogl_glunsupported("glGetCombinerInputParameterfvNV");
	glGetCombinerInputParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]));

}

void gl_getcombinerinputparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCombinerInputParameterivNV) mogl_glunsupported("glGetCombinerInputParameterivNV");
	glGetCombinerInputParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void gl_getcombineroutputparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCombinerOutputParameterfvNV) mogl_glunsupported("glGetCombinerOutputParameterfvNV");
	glGetCombinerOutputParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getcombineroutputparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCombinerOutputParameterivNV) mogl_glunsupported("glGetCombinerOutputParameterivNV");
	glGetCombinerOutputParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getfinalcombinerinputparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFinalCombinerInputParameterfvNV) mogl_glunsupported("glGetFinalCombinerInputParameterfvNV");
	glGetFinalCombinerInputParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getfinalcombinerinputparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFinalCombinerInputParameterivNV) mogl_glunsupported("glGetFinalCombinerInputParameterivNV");
	glGetFinalCombinerInputParameterivNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_combinerstageparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCombinerStageParameterfvNV) mogl_glunsupported("glCombinerStageParameterfvNV");
	glCombinerStageParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getcombinerstageparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetCombinerStageParameterfvNV) mogl_glunsupported("glGetCombinerStageParameterfvNV");
	glGetCombinerStageParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_makebufferresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeBufferResidentNV) mogl_glunsupported("glMakeBufferResidentNV");
	glMakeBufferResidentNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_makebuffernonresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeBufferNonResidentNV) mogl_glunsupported("glMakeBufferNonResidentNV");
	glMakeBufferNonResidentNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_isbufferresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsBufferResidentNV) mogl_glunsupported("glIsBufferResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsBufferResidentNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_makenamedbufferresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeNamedBufferResidentNV) mogl_glunsupported("glMakeNamedBufferResidentNV");
	glMakeNamedBufferResidentNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_makenamedbuffernonresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMakeNamedBufferNonResidentNV) mogl_glunsupported("glMakeNamedBufferNonResidentNV");
	glMakeNamedBufferNonResidentNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_isnamedbufferresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsNamedBufferResidentNV) mogl_glunsupported("glIsNamedBufferResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsNamedBufferResidentNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_getbufferparameterui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferParameterui64vNV) mogl_glunsupported("glGetBufferParameterui64vNV");
	glGetBufferParameterui64vNV((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_getnamedbufferparameterui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetNamedBufferParameterui64vNV) mogl_glunsupported("glGetNamedBufferParameterui64vNV");
	glGetNamedBufferParameterui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_getintegerui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetIntegerui64vNV) mogl_glunsupported("glGetIntegerui64vNV");
	glGetIntegerui64vNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_uniformui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformui64NV) mogl_glunsupported("glUniformui64NV");
	glUniformui64NV((GLint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]));

}

void gl_uniformui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glUniformui64vNV) mogl_glunsupported("glUniformui64vNV");
	glUniformui64vNV((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_programuniformui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformui64NV) mogl_glunsupported("glProgramUniformui64NV");
	glProgramUniformui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]));

}

void gl_programuniformui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramUniformui64vNV) mogl_glunsupported("glProgramUniformui64vNV");
	glProgramUniformui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLuint64EXT*)mxGetData(prhs[3]));

}

void gl_texturebarriernv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureBarrierNV) mogl_glunsupported("glTextureBarrierNV");
	glTextureBarrierNV();

}

void gl_teximage2dmultisamplecoveragenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage2DMultisampleCoverageNV) mogl_glunsupported("glTexImage2DMultisampleCoverageNV");
	glTexImage2DMultisampleCoverageNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_teximage3dmultisamplecoveragenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage3DMultisampleCoverageNV) mogl_glunsupported("glTexImage3DMultisampleCoverageNV");
	glTexImage3DMultisampleCoverageNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]));

}

void gl_textureimage2dmultisamplenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage2DMultisampleNV) mogl_glunsupported("glTextureImage2DMultisampleNV");
	glTextureImage2DMultisampleNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLboolean)mxGetScalar(prhs[6]));

}

void gl_textureimage3dmultisamplenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage3DMultisampleNV) mogl_glunsupported("glTextureImage3DMultisampleNV");
	glTextureImage3DMultisampleNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]));

}

void gl_textureimage2dmultisamplecoveragenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage2DMultisampleCoverageNV) mogl_glunsupported("glTextureImage2DMultisampleCoverageNV");
	glTextureImage2DMultisampleCoverageNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLboolean)mxGetScalar(prhs[7]));

}

void gl_textureimage3dmultisamplecoveragenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTextureImage3DMultisampleCoverageNV) mogl_glunsupported("glTextureImage3DMultisampleCoverageNV");
	glTextureImage3DMultisampleCoverageNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLboolean)mxGetScalar(prhs[8]));

}

void gl_begintransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginTransformFeedbackNV) mogl_glunsupported("glBeginTransformFeedbackNV");
	glBeginTransformFeedbackNV((GLenum)mxGetScalar(prhs[0]));

}

void gl_endtransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndTransformFeedbackNV) mogl_glunsupported("glEndTransformFeedbackNV");
	glEndTransformFeedbackNV();

}

void gl_transformfeedbackattribsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTransformFeedbackAttribsNV) mogl_glunsupported("glTransformFeedbackAttribsNV");
	glTransformFeedbackAttribsNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_bindbufferrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferRangeNV) mogl_glunsupported("glBindBufferRangeNV");
	glBindBufferRangeNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]),
		(GLsizeiptr)mxGetScalar(prhs[4]));

}

void gl_bindbufferoffsetnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferOffsetNV) mogl_glunsupported("glBindBufferOffsetNV");
	glBindBufferOffsetNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]));

}

void gl_bindbufferbasenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindBufferBaseNV) mogl_glunsupported("glBindBufferBaseNV");
	glBindBufferBaseNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_activevaryingnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glActiveVaryingNV) mogl_glunsupported("glActiveVaryingNV");
	glActiveVaryingNV((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getvaryinglocationnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVaryingLocationNV) mogl_glunsupported("glGetVaryingLocationNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetVaryingLocationNV((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getactivevaryingnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetActiveVaryingNV) mogl_glunsupported("glGetActiveVaryingNV");
	glGetActiveVaryingNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_gettransformfeedbackvaryingnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTransformFeedbackVaryingNV) mogl_glunsupported("glGetTransformFeedbackVaryingNV");
	glGetTransformFeedbackVaryingNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_bindtransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindTransformFeedbackNV) mogl_glunsupported("glBindTransformFeedbackNV");
	glBindTransformFeedbackNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletetransformfeedbacksnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteTransformFeedbacksNV) mogl_glunsupported("glDeleteTransformFeedbacksNV");
	glDeleteTransformFeedbacksNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_gentransformfeedbacksnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenTransformFeedbacksNV) mogl_glunsupported("glGenTransformFeedbacksNV");
	glGenTransformFeedbacksNV((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_istransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsTransformFeedbackNV) mogl_glunsupported("glIsTransformFeedbackNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTransformFeedbackNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_pausetransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPauseTransformFeedbackNV) mogl_glunsupported("glPauseTransformFeedbackNV");
	glPauseTransformFeedbackNV();

}

void gl_resumetransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glResumeTransformFeedbackNV) mogl_glunsupported("glResumeTransformFeedbackNV");
	glResumeTransformFeedbackNV();

}

void gl_drawtransformfeedbacknv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawTransformFeedbackNV) mogl_glunsupported("glDrawTransformFeedbackNV");
	glDrawTransformFeedbackNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vdpauinitnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVDPAUInitNV) mogl_glunsupported("glVDPAUInitNV");
	glVDPAUInitNV((const void*)mxGetData(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_vdpaufininv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVDPAUFiniNV) mogl_glunsupported("glVDPAUFiniNV");
	glVDPAUFiniNV();

}

void gl_flushvertexarrayrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushVertexArrayRangeNV) mogl_glunsupported("glFlushVertexArrayRangeNV");
	glFlushVertexArrayRangeNV();

}

void gl_vertexarrayrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexArrayRangeNV) mogl_glunsupported("glVertexArrayRangeNV");
	glVertexArrayRangeNV((GLsizei)mxGetScalar(prhs[0]),
		(const void*)mxGetData(prhs[1]));

}

void gl_vertexattribl1i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1i64NV) mogl_glunsupported("glVertexAttribL1i64NV");
	glVertexAttribL1i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]));

}

void gl_vertexattribl2i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2i64NV) mogl_glunsupported("glVertexAttribL2i64NV");
	glVertexAttribL2i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]));

}

void gl_vertexattribl3i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3i64NV) mogl_glunsupported("glVertexAttribL3i64NV");
	glVertexAttribL3i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]));

}

void gl_vertexattribl4i64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4i64NV) mogl_glunsupported("glVertexAttribL4i64NV");
	glVertexAttribL4i64NV((GLuint)mxGetScalar(prhs[0]),
		(GLint64EXT)mxGetScalar(prhs[1]),
		(GLint64EXT)mxGetScalar(prhs[2]),
		(GLint64EXT)mxGetScalar(prhs[3]),
		(GLint64EXT)mxGetScalar(prhs[4]));

}

void gl_vertexattribl1i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1i64vNV) mogl_glunsupported("glVertexAttribL1i64vNV");
	glVertexAttribL1i64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl2i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2i64vNV) mogl_glunsupported("glVertexAttribL2i64vNV");
	glVertexAttribL2i64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl3i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3i64vNV) mogl_glunsupported("glVertexAttribL3i64vNV");
	glVertexAttribL3i64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl4i64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4i64vNV) mogl_glunsupported("glVertexAttribL4i64vNV");
	glVertexAttribL4i64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl1ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1ui64NV) mogl_glunsupported("glVertexAttribL1ui64NV");
	glVertexAttribL1ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]));

}

void gl_vertexattribl2ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2ui64NV) mogl_glunsupported("glVertexAttribL2ui64NV");
	glVertexAttribL2ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]));

}

void gl_vertexattribl3ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3ui64NV) mogl_glunsupported("glVertexAttribL3ui64NV");
	glVertexAttribL3ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]));

}

void gl_vertexattribl4ui64nv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4ui64NV) mogl_glunsupported("glVertexAttribL4ui64NV");
	glVertexAttribL4ui64NV((GLuint)mxGetScalar(prhs[0]),
		(GLuint64EXT)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLuint64EXT)mxGetScalar(prhs[3]),
		(GLuint64EXT)mxGetScalar(prhs[4]));

}

void gl_vertexattribl1ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL1ui64vNV) mogl_glunsupported("glVertexAttribL1ui64vNV");
	glVertexAttribL1ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl2ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL2ui64vNV) mogl_glunsupported("glVertexAttribL2ui64vNV");
	glVertexAttribL2ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl3ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL3ui64vNV) mogl_glunsupported("glVertexAttribL3ui64vNV");
	glVertexAttribL3ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_vertexattribl4ui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribL4ui64vNV) mogl_glunsupported("glVertexAttribL4ui64vNV");
	glVertexAttribL4ui64vNV((GLuint)mxGetScalar(prhs[0]),
		(const GLuint64EXT*)mxGetData(prhs[1]));

}

void gl_getvertexattribli64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribLi64vNV) mogl_glunsupported("glGetVertexAttribLi64vNV");
	glGetVertexAttribLi64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint64EXT*)mxGetData(prhs[2]));

}

void gl_getvertexattriblui64vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribLui64vNV) mogl_glunsupported("glGetVertexAttribLui64vNV");
	glGetVertexAttribLui64vNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_vertexattriblformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribLFormatNV) mogl_glunsupported("glVertexAttribLFormatNV");
	glVertexAttribLFormatNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_bufferaddressrangenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferAddressRangeNV) mogl_glunsupported("glBufferAddressRangeNV");
	glBufferAddressRangeNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint64EXT)mxGetScalar(prhs[2]),
		(GLsizeiptr)mxGetScalar(prhs[3]));

}

void gl_vertexformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexFormatNV) mogl_glunsupported("glVertexFormatNV");
	glVertexFormatNV((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_normalformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalFormatNV) mogl_glunsupported("glNormalFormatNV");
	glNormalFormatNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_colorformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorFormatNV) mogl_glunsupported("glColorFormatNV");
	glColorFormatNV((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_indexformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIndexFormatNV) mogl_glunsupported("glIndexFormatNV");
	glIndexFormatNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_texcoordformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordFormatNV) mogl_glunsupported("glTexCoordFormatNV");
	glTexCoordFormatNV((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_edgeflagformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEdgeFlagFormatNV) mogl_glunsupported("glEdgeFlagFormatNV");
	glEdgeFlagFormatNV((GLsizei)mxGetScalar(prhs[0]));

}

void gl_secondarycolorformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSecondaryColorFormatNV) mogl_glunsupported("glSecondaryColorFormatNV");
	glSecondaryColorFormatNV((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_fogcoordformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogCoordFormatNV) mogl_glunsupported("glFogCoordFormatNV");
	glFogCoordFormatNV((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_vertexattribformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribFormatNV) mogl_glunsupported("glVertexAttribFormatNV");
	glVertexAttribFormatNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_vertexattribiformatnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribIFormatNV) mogl_glunsupported("glVertexAttribIFormatNV");
	glVertexAttribIFormatNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_getintegerui64i_vnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetIntegerui64i_vNV) mogl_glunsupported("glGetIntegerui64i_vNV");
	glGetIntegerui64i_vNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_areprogramsresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAreProgramsResidentNV) mogl_glunsupported("glAreProgramsResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glAreProgramsResidentNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_bindprogramnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindProgramNV) mogl_glunsupported("glBindProgramNV");
	glBindProgramNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deleteprogramsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteProgramsNV) mogl_glunsupported("glDeleteProgramsNV");
	glDeleteProgramsNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_executeprogramnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glExecuteProgramNV) mogl_glunsupported("glExecuteProgramNV");
	glExecuteProgramNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_genprogramsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenProgramsNV) mogl_glunsupported("glGenProgramsNV");
	glGenProgramsNV((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getprogramparameterdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramParameterdvNV) mogl_glunsupported("glGetProgramParameterdvNV");
	glGetProgramParameterdvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_getprogramparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramParameterfvNV) mogl_glunsupported("glGetProgramParameterfvNV");
	glGetProgramParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getprogramivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramivNV) mogl_glunsupported("glGetProgramivNV");
	glGetProgramivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getprogramstringnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramStringNV) mogl_glunsupported("glGetProgramStringNV");
	glGetProgramStringNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLubyte*)mxGetData(prhs[2]));

}

void gl_gettrackmatrixivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTrackMatrixivNV) mogl_glunsupported("glGetTrackMatrixivNV");
	glGetTrackMatrixivNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getvertexattribdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribdvNV) mogl_glunsupported("glGetVertexAttribdvNV");
	glGetVertexAttribdvNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getvertexattribfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribfvNV) mogl_glunsupported("glGetVertexAttribfvNV");
	glGetVertexAttribfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvertexattribivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribivNV) mogl_glunsupported("glGetVertexAttribivNV");
	glGetVertexAttribivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_isprogramnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsProgramNV) mogl_glunsupported("glIsProgramNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsProgramNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_loadprogramnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glLoadProgramNV) mogl_glunsupported("glLoadProgramNV");
	glLoadProgramNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLubyte*)mxGetData(prhs[3]));

}

void gl_programparameter4dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameter4dNV) mogl_glunsupported("glProgramParameter4dNV");
	glProgramParameter4dNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_programparameter4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameter4dvNV) mogl_glunsupported("glProgramParameter4dvNV");
	glProgramParameter4dvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_programparameter4fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameter4fNV) mogl_glunsupported("glProgramParameter4fNV");
	glProgramParameter4fNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_programparameter4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameter4fvNV) mogl_glunsupported("glProgramParameter4fvNV");
	glProgramParameter4fvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_programparameters4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameters4dvNV) mogl_glunsupported("glProgramParameters4dvNV");
	glProgramParameters4dvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programparameters4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameters4fvNV) mogl_glunsupported("glProgramParameters4fvNV");
	glProgramParameters4fvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_requestresidentprogramsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRequestResidentProgramsNV) mogl_glunsupported("glRequestResidentProgramsNV");
	glRequestResidentProgramsNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_trackmatrixnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTrackMatrixNV) mogl_glunsupported("glTrackMatrixNV");
	glTrackMatrixNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_vertexattribpointernv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribPointerNV) mogl_glunsupported("glVertexAttribPointerNV");
	glVertexAttribPointerNV((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_vertexattrib1dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dNV) mogl_glunsupported("glVertexAttrib1dNV");
	glVertexAttrib1dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dvNV) mogl_glunsupported("glVertexAttrib1dvNV");
	glVertexAttrib1dvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib1fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fNV) mogl_glunsupported("glVertexAttrib1fNV");
	glVertexAttrib1fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fvNV) mogl_glunsupported("glVertexAttrib1fvNV");
	glVertexAttrib1fvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib1snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1sNV) mogl_glunsupported("glVertexAttrib1sNV");
	glVertexAttrib1sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1svNV) mogl_glunsupported("glVertexAttrib1svNV");
	glVertexAttrib1svNV((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib2dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dNV) mogl_glunsupported("glVertexAttrib2dNV");
	glVertexAttrib2dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dvNV) mogl_glunsupported("glVertexAttrib2dvNV");
	glVertexAttrib2dvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib2fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fNV) mogl_glunsupported("glVertexAttrib2fNV");
	glVertexAttrib2fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fvNV) mogl_glunsupported("glVertexAttrib2fvNV");
	glVertexAttrib2fvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib2snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2sNV) mogl_glunsupported("glVertexAttrib2sNV");
	glVertexAttrib2sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2svNV) mogl_glunsupported("glVertexAttrib2svNV");
	glVertexAttrib2svNV((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib3dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dNV) mogl_glunsupported("glVertexAttrib3dNV");
	glVertexAttrib3dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dvNV) mogl_glunsupported("glVertexAttrib3dvNV");
	glVertexAttrib3dvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib3fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fNV) mogl_glunsupported("glVertexAttrib3fNV");
	glVertexAttrib3fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fvNV) mogl_glunsupported("glVertexAttrib3fvNV");
	glVertexAttrib3fvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib3snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3sNV) mogl_glunsupported("glVertexAttrib3sNV");
	glVertexAttrib3sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3svNV) mogl_glunsupported("glVertexAttrib3svNV");
	glVertexAttrib3svNV((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dNV) mogl_glunsupported("glVertexAttrib4dNV");
	glVertexAttrib4dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dvNV) mogl_glunsupported("glVertexAttrib4dvNV");
	glVertexAttrib4dvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fNV) mogl_glunsupported("glVertexAttrib4fNV");
	glVertexAttrib4fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fvNV) mogl_glunsupported("glVertexAttrib4fvNV");
	glVertexAttrib4fvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib4snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4sNV) mogl_glunsupported("glVertexAttrib4sNV");
	glVertexAttrib4sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4svNV) mogl_glunsupported("glVertexAttrib4svNV");
	glVertexAttrib4svNV((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ubnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubNV) mogl_glunsupported("glVertexAttrib4ubNV");
	glVertexAttrib4ubNV((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4ubvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubvNV) mogl_glunsupported("glVertexAttrib4ubvNV");
	glVertexAttrib4ubvNV((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattribs1dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1dvNV) mogl_glunsupported("glVertexAttribs1dvNV");
	glVertexAttribs1dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs1fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1fvNV) mogl_glunsupported("glVertexAttribs1fvNV");
	glVertexAttribs1fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs1svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1svNV) mogl_glunsupported("glVertexAttribs1svNV");
	glVertexAttribs1svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs2dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2dvNV) mogl_glunsupported("glVertexAttribs2dvNV");
	glVertexAttribs2dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs2fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2fvNV) mogl_glunsupported("glVertexAttribs2fvNV");
	glVertexAttribs2fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs2svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2svNV) mogl_glunsupported("glVertexAttribs2svNV");
	glVertexAttribs2svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs3dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3dvNV) mogl_glunsupported("glVertexAttribs3dvNV");
	glVertexAttribs3dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs3fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3fvNV) mogl_glunsupported("glVertexAttribs3fvNV");
	glVertexAttribs3fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs3svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3svNV) mogl_glunsupported("glVertexAttribs3svNV");
	glVertexAttribs3svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4dvNV) mogl_glunsupported("glVertexAttribs4dvNV");
	glVertexAttribs4dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4fvNV) mogl_glunsupported("glVertexAttribs4fvNV");
	glVertexAttribs4fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs4svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4svNV) mogl_glunsupported("glVertexAttribs4svNV");
	glVertexAttribs4svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs4ubvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4ubvNV) mogl_glunsupported("glVertexAttribs4ubvNV");
	glVertexAttribs4ubvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLubyte*)mxGetData(prhs[2]));

}

void gl_vertexattribi1iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1iEXT) mogl_glunsupported("glVertexAttribI1iEXT");
	glVertexAttribI1iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertexattribi2iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2iEXT) mogl_glunsupported("glVertexAttribI2iEXT");
	glVertexAttribI2iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_vertexattribi3iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3iEXT) mogl_glunsupported("glVertexAttribI3iEXT");
	glVertexAttribI3iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_vertexattribi4iext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4iEXT) mogl_glunsupported("glVertexAttribI4iEXT");
	glVertexAttribI4iEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_vertexattribi1uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1uiEXT) mogl_glunsupported("glVertexAttribI1uiEXT");
	glVertexAttribI1uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_vertexattribi2uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2uiEXT) mogl_glunsupported("glVertexAttribI2uiEXT");
	glVertexAttribI2uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_vertexattribi3uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3uiEXT) mogl_glunsupported("glVertexAttribI3uiEXT");
	glVertexAttribI3uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_vertexattribi4uiext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4uiEXT) mogl_glunsupported("glVertexAttribI4uiEXT");
	glVertexAttribI4uiEXT((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_vertexattribi1ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1ivEXT) mogl_glunsupported("glVertexAttribI1ivEXT");
	glVertexAttribI1ivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi2ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2ivEXT) mogl_glunsupported("glVertexAttribI2ivEXT");
	glVertexAttribI2ivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi3ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3ivEXT) mogl_glunsupported("glVertexAttribI3ivEXT");
	glVertexAttribI3ivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4ivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4ivEXT) mogl_glunsupported("glVertexAttribI4ivEXT");
	glVertexAttribI4ivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattribi1uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI1uivEXT) mogl_glunsupported("glVertexAttribI1uivEXT");
	glVertexAttribI1uivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi2uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI2uivEXT) mogl_glunsupported("glVertexAttribI2uivEXT");
	glVertexAttribI2uivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi3uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI3uivEXT) mogl_glunsupported("glVertexAttribI3uivEXT");
	glVertexAttribI3uivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4uivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4uivEXT) mogl_glunsupported("glVertexAttribI4uivEXT");
	glVertexAttribI4uivEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattribi4bvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4bvEXT) mogl_glunsupported("glVertexAttribI4bvEXT");
	glVertexAttribI4bvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattribi4svext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4svEXT) mogl_glunsupported("glVertexAttribI4svEXT");
	glVertexAttribI4svEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattribi4ubvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4ubvEXT) mogl_glunsupported("glVertexAttribI4ubvEXT");
	glVertexAttribI4ubvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattribi4usvext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribI4usvEXT) mogl_glunsupported("glVertexAttribI4usvEXT");
	glVertexAttribI4usvEXT((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattribipointerext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribIPointerEXT) mogl_glunsupported("glVertexAttribIPointerEXT");
	glVertexAttribIPointerEXT((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(const void*)mxGetData(prhs[4]));

}

void gl_getvertexattribiivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribIivEXT) mogl_glunsupported("glGetVertexAttribIivEXT");
	glGetVertexAttribIivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvertexattribiuivext( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVertexAttribIuivEXT) mogl_glunsupported("glGetVertexAttribIuivEXT");
	glGetVertexAttribIuivEXT((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_beginvideocapturenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBeginVideoCaptureNV) mogl_glunsupported("glBeginVideoCaptureNV");
	glBeginVideoCaptureNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_bindvideocapturestreambuffernv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVideoCaptureStreamBufferNV) mogl_glunsupported("glBindVideoCaptureStreamBufferNV");
	glBindVideoCaptureStreamBufferNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLintptr)mxGetScalar(prhs[3]));

}

void gl_bindvideocapturestreamtexturenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBindVideoCaptureStreamTextureNV) mogl_glunsupported("glBindVideoCaptureStreamTextureNV");
	glBindVideoCaptureStreamTextureNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLuint)mxGetScalar(prhs[4]));

}

void gl_endvideocapturenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glEndVideoCaptureNV) mogl_glunsupported("glEndVideoCaptureNV");
	glEndVideoCaptureNV((GLuint)mxGetScalar(prhs[0]));

}

void gl_getvideocaptureivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoCaptureivNV) mogl_glunsupported("glGetVideoCaptureivNV");
	glGetVideoCaptureivNV((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getvideocapturestreamivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoCaptureStreamivNV) mogl_glunsupported("glGetVideoCaptureStreamivNV");
	glGetVideoCaptureStreamivNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_getvideocapturestreamfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoCaptureStreamfvNV) mogl_glunsupported("glGetVideoCaptureStreamfvNV");
	glGetVideoCaptureStreamfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getvideocapturestreamdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetVideoCaptureStreamdvNV) mogl_glunsupported("glGetVideoCaptureStreamdvNV");
	glGetVideoCaptureStreamdvNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

}

void gl_videocapturenv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVideoCaptureNV) mogl_glunsupported("glVideoCaptureNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glVideoCaptureNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]),
		(GLuint64EXT*)mxGetData(prhs[2]));

}

void gl_videocapturestreamparameterivnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVideoCaptureStreamParameterivNV) mogl_glunsupported("glVideoCaptureStreamParameterivNV");
	glVideoCaptureStreamParameterivNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void gl_videocapturestreamparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVideoCaptureStreamParameterfvNV) mogl_glunsupported("glVideoCaptureStreamParameterfvNV");
	glVideoCaptureStreamParameterfvNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_videocapturestreamparameterdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVideoCaptureStreamParameterdvNV) mogl_glunsupported("glVideoCaptureStreamParameterdvNV");
	glVideoCaptureStreamParameterdvNV((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_detailtexfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDetailTexFuncSGIS) mogl_glunsupported("glDetailTexFuncSGIS");
	glDetailTexFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getdetailtexfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetDetailTexFuncSGIS) mogl_glunsupported("glGetDetailTexFuncSGIS");
	glGetDetailTexFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_fogfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFogFuncSGIS) mogl_glunsupported("glFogFuncSGIS");
	glFogFuncSGIS((GLsizei)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_getfogfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFogFuncSGIS) mogl_glunsupported("glGetFogFuncSGIS");
	glGetFogFuncSGIS((GLfloat*)mxGetData(prhs[0]));

}

void gl_samplemasksgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMaskSGIS) mogl_glunsupported("glSampleMaskSGIS");
	glSampleMaskSGIS((GLclampf)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_samplepatternsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplePatternSGIS) mogl_glunsupported("glSamplePatternSGIS");
	glSamplePatternSGIS((GLenum)mxGetScalar(prhs[0]));

}

void gl_sharpentexfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSharpenTexFuncSGIS) mogl_glunsupported("glSharpenTexFuncSGIS");
	glSharpenTexFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_getsharpentexfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSharpenTexFuncSGIS) mogl_glunsupported("glGetSharpenTexFuncSGIS");
	glGetSharpenTexFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_teximage4dsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage4DSGIS) mogl_glunsupported("glTexImage4DSGIS");
	glTexImage4DSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLint)mxGetScalar(prhs[7]),
		(GLenum)mxGetScalar(prhs[8]),
		(GLenum)mxGetScalar(prhs[9]),
		(const void*)mxGetData(prhs[10]));

}

void gl_texsubimage4dsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexSubImage4DSGIS) mogl_glunsupported("glTexSubImage4DSGIS");
	glTexSubImage4DSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLsizei)mxGetScalar(prhs[6]),
		(GLsizei)mxGetScalar(prhs[7]),
		(GLsizei)mxGetScalar(prhs[8]),
		(GLsizei)mxGetScalar(prhs[9]),
		(GLenum)mxGetScalar(prhs[10]),
		(GLenum)mxGetScalar(prhs[11]),
		(const void*)mxGetData(prhs[12]));

}

void gl_gettexfilterfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetTexFilterFuncSGIS) mogl_glunsupported("glGetTexFilterFuncSGIS");
	glGetTexFilterFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_texfilterfuncsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexFilterFuncSGIS) mogl_glunsupported("glTexFilterFuncSGIS");
	glTexFilterFuncSGIS((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_asyncmarkersgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAsyncMarkerSGIX) mogl_glunsupported("glAsyncMarkerSGIX");
	glAsyncMarkerSGIX((GLuint)mxGetScalar(prhs[0]));

}

void gl_finishasyncsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinishAsyncSGIX) mogl_glunsupported("glFinishAsyncSGIX");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glFinishAsyncSGIX((GLuint*)mxGetData(prhs[0]));

}

void gl_pollasyncsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPollAsyncSGIX) mogl_glunsupported("glPollAsyncSGIX");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glPollAsyncSGIX((GLuint*)mxGetData(prhs[0]));

}

void gl_genasyncmarkerssgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGenAsyncMarkersSGIX) mogl_glunsupported("glGenAsyncMarkersSGIX");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenAsyncMarkersSGIX((GLsizei)mxGetScalar(prhs[0]));

}

void gl_deleteasyncmarkerssgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDeleteAsyncMarkersSGIX) mogl_glunsupported("glDeleteAsyncMarkersSGIX");
	glDeleteAsyncMarkersSGIX((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_isasyncmarkersgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glIsAsyncMarkerSGIX) mogl_glunsupported("glIsAsyncMarkerSGIX");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsAsyncMarkerSGIX((GLuint)mxGetScalar(prhs[0]));

}

void gl_flushrastersgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFlushRasterSGIX) mogl_glunsupported("glFlushRasterSGIX");
	glFlushRasterSGIX();

}

void gl_fragmentcolormaterialsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentColorMaterialSGIX) mogl_glunsupported("glFragmentColorMaterialSGIX");
	glFragmentColorMaterialSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_fragmentlightfsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightfSGIX) mogl_glunsupported("glFragmentLightfSGIX");
	glFragmentLightfSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_fragmentlightfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightfvSGIX) mogl_glunsupported("glFragmentLightfvSGIX");
	glFragmentLightfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_fragmentlightisgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightiSGIX) mogl_glunsupported("glFragmentLightiSGIX");
	glFragmentLightiSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_fragmentlightivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightivSGIX) mogl_glunsupported("glFragmentLightivSGIX");
	glFragmentLightivSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_fragmentlightmodelfsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightModelfSGIX) mogl_glunsupported("glFragmentLightModelfSGIX");
	glFragmentLightModelfSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_fragmentlightmodelfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightModelfvSGIX) mogl_glunsupported("glFragmentLightModelfvSGIX");
	glFragmentLightModelfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_fragmentlightmodelisgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightModeliSGIX) mogl_glunsupported("glFragmentLightModeliSGIX");
	glFragmentLightModeliSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_fragmentlightmodelivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentLightModelivSGIX) mogl_glunsupported("glFragmentLightModelivSGIX");
	glFragmentLightModelivSGIX((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_fragmentmaterialfsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentMaterialfSGIX) mogl_glunsupported("glFragmentMaterialfSGIX");
	glFragmentMaterialfSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_fragmentmaterialfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentMaterialfvSGIX) mogl_glunsupported("glFragmentMaterialfvSGIX");
	glFragmentMaterialfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_fragmentmaterialisgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentMaterialiSGIX) mogl_glunsupported("glFragmentMaterialiSGIX");
	glFragmentMaterialiSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_fragmentmaterialivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFragmentMaterialivSGIX) mogl_glunsupported("glFragmentMaterialivSGIX");
	glFragmentMaterialivSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_getfragmentlightfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragmentLightfvSGIX) mogl_glunsupported("glGetFragmentLightfvSGIX");
	glGetFragmentLightfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getfragmentlightivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragmentLightivSGIX) mogl_glunsupported("glGetFragmentLightivSGIX");
	glGetFragmentLightivSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getfragmentmaterialfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragmentMaterialfvSGIX) mogl_glunsupported("glGetFragmentMaterialfvSGIX");
	glGetFragmentMaterialfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getfragmentmaterialivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetFragmentMaterialivSGIX) mogl_glunsupported("glGetFragmentMaterialivSGIX");
	glGetFragmentMaterialivSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_framezoomsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFrameZoomSGIX) mogl_glunsupported("glFrameZoomSGIX");
	glFrameZoomSGIX((GLint)mxGetScalar(prhs[0]));

}

void gl_pixeltexgensgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glPixelTexGenSGIX) mogl_glunsupported("glPixelTexGenSGIX");
	glPixelTexGenSGIX((GLenum)mxGetScalar(prhs[0]));

}

void gl_referenceplanesgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReferencePlaneSGIX) mogl_glunsupported("glReferencePlaneSGIX");
	glReferencePlaneSGIX((const GLdouble*)mxGetData(prhs[0]));

}

void gl_spriteparameterfsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSpriteParameterfSGIX) mogl_glunsupported("glSpriteParameterfSGIX");
	glSpriteParameterfSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_spriteparameterfvsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSpriteParameterfvSGIX) mogl_glunsupported("glSpriteParameterfvSGIX");
	glSpriteParameterfvSGIX((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_spriteparameterisgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSpriteParameteriSGIX) mogl_glunsupported("glSpriteParameteriSGIX");
	glSpriteParameteriSGIX((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_spriteparameterivsgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSpriteParameterivSGIX) mogl_glunsupported("glSpriteParameterivSGIX");
	glSpriteParameterivSGIX((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_tagsamplebuffersgix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTagSampleBufferSGIX) mogl_glunsupported("glTagSampleBufferSGIX");
	glTagSampleBufferSGIX();

}

void gl_colortablesgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableSGI) mogl_glunsupported("glColorTableSGI");
	glColorTableSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void gl_colortableparameterfvsgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableParameterfvSGI) mogl_glunsupported("glColorTableParameterfvSGI");
	glColorTableParameterfvSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_colortableparameterivsgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorTableParameterivSGI) mogl_glunsupported("glColorTableParameterivSGI");
	glColorTableParameterivSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_copycolortablesgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glCopyColorTableSGI) mogl_glunsupported("glCopyColorTableSGI");
	glCopyColorTableSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_getcolortablesgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableSGI) mogl_glunsupported("glGetColorTableSGI");
	glGetColorTableSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(void*)mxGetData(prhs[3]));

}

void gl_getcolortableparameterfvsgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameterfvSGI) mogl_glunsupported("glGetColorTableParameterfvSGI");
	glGetColorTableParameterfvSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getcolortableparameterivsgi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameterivSGI) mogl_glunsupported("glGetColorTableParameterivSGI");
	glGetColorTableParameterivSGI((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_finishtexturesunx( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glFinishTextureSUNX) mogl_glunsupported("glFinishTextureSUNX");
	glFinishTextureSUNX();

}

void gl_globalalphafactorbsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactorbSUN) mogl_glunsupported("glGlobalAlphaFactorbSUN");
	glGlobalAlphaFactorbSUN((GLbyte)mxGetScalar(prhs[0]));

}

void gl_globalalphafactorssun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactorsSUN) mogl_glunsupported("glGlobalAlphaFactorsSUN");
	glGlobalAlphaFactorsSUN((GLshort)mxGetScalar(prhs[0]));

}

void gl_globalalphafactorisun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactoriSUN) mogl_glunsupported("glGlobalAlphaFactoriSUN");
	glGlobalAlphaFactoriSUN((GLint)mxGetScalar(prhs[0]));

}

void gl_globalalphafactorfsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactorfSUN) mogl_glunsupported("glGlobalAlphaFactorfSUN");
	glGlobalAlphaFactorfSUN((GLfloat)mxGetScalar(prhs[0]));

}

void gl_globalalphafactordsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactordSUN) mogl_glunsupported("glGlobalAlphaFactordSUN");
	glGlobalAlphaFactordSUN((GLdouble)mxGetScalar(prhs[0]));

}

void gl_globalalphafactorubsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactorubSUN) mogl_glunsupported("glGlobalAlphaFactorubSUN");
	glGlobalAlphaFactorubSUN((GLubyte)mxGetScalar(prhs[0]));

}

void gl_globalalphafactorussun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactorusSUN) mogl_glunsupported("glGlobalAlphaFactorusSUN");
	glGlobalAlphaFactorusSUN((GLushort)mxGetScalar(prhs[0]));

}

void gl_globalalphafactoruisun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGlobalAlphaFactoruiSUN) mogl_glunsupported("glGlobalAlphaFactoruiSUN");
	glGlobalAlphaFactoruiSUN((GLuint)mxGetScalar(prhs[0]));

}

void gl_replacementcodeuisun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiSUN) mogl_glunsupported("glReplacementCodeuiSUN");
	glReplacementCodeuiSUN((GLuint)mxGetScalar(prhs[0]));

}

void gl_replacementcodeussun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeusSUN) mogl_glunsupported("glReplacementCodeusSUN");
	glReplacementCodeusSUN((GLushort)mxGetScalar(prhs[0]));

}

void gl_replacementcodeubsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeubSUN) mogl_glunsupported("glReplacementCodeubSUN");
	glReplacementCodeubSUN((GLubyte)mxGetScalar(prhs[0]));

}

void gl_replacementcodeuivsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuivSUN) mogl_glunsupported("glReplacementCodeuivSUN");
	glReplacementCodeuivSUN((const GLuint*)mxGetData(prhs[0]));

}

void gl_replacementcodeusvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeusvSUN) mogl_glunsupported("glReplacementCodeusvSUN");
	glReplacementCodeusvSUN((const GLushort*)mxGetData(prhs[0]));

}

void gl_replacementcodeubvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeubvSUN) mogl_glunsupported("glReplacementCodeubvSUN");
	glReplacementCodeubvSUN((const GLubyte*)mxGetData(prhs[0]));

}

void gl_color4ubvertex2fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ubVertex2fSUN) mogl_glunsupported("glColor4ubVertex2fSUN");
	glColor4ubVertex2fSUN((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_color4ubvertex2fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ubVertex2fvSUN) mogl_glunsupported("glColor4ubVertex2fvSUN");
	glColor4ubVertex2fvSUN((const GLubyte*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_color4ubvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ubVertex3fSUN) mogl_glunsupported("glColor4ubVertex3fSUN");
	glColor4ubVertex3fSUN((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]));

}

void gl_color4ubvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4ubVertex3fvSUN) mogl_glunsupported("glColor4ubVertex3fvSUN");
	glColor4ubVertex3fvSUN((const GLubyte*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_color3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3fVertex3fSUN) mogl_glunsupported("glColor3fVertex3fSUN");
	glColor3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_color3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor3fVertex3fvSUN) mogl_glunsupported("glColor3fVertex3fvSUN");
	glColor3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_normal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3fVertex3fSUN) mogl_glunsupported("glNormal3fVertex3fSUN");
	glNormal3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_normal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormal3fVertex3fvSUN) mogl_glunsupported("glNormal3fVertex3fvSUN");
	glNormal3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_color4fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4fNormal3fVertex3fSUN) mogl_glunsupported("glColor4fNormal3fVertex3fSUN");
	glColor4fNormal3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]));

}

void gl_color4fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColor4fNormal3fVertex3fvSUN) mogl_glunsupported("glColor4fNormal3fVertex3fvSUN");
	glColor4fNormal3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texcoord2fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fVertex3fSUN) mogl_glunsupported("glTexCoord2fVertex3fSUN");
	glTexCoord2fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_texcoord2fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fVertex3fvSUN) mogl_glunsupported("glTexCoord2fVertex3fvSUN");
	glTexCoord2fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_texcoord4fvertex4fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4fVertex4fSUN) mogl_glunsupported("glTexCoord4fVertex4fSUN");
	glTexCoord4fVertex4fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]));

}

void gl_texcoord4fvertex4fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4fVertex4fvSUN) mogl_glunsupported("glTexCoord4fVertex4fvSUN");
	glTexCoord4fVertex4fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_texcoord2fcolor4ubvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor4ubVertex3fSUN) mogl_glunsupported("glTexCoord2fColor4ubVertex3fSUN");
	glTexCoord2fColor4ubVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]),
		(GLubyte)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]));

}

void gl_texcoord2fcolor4ubvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor4ubVertex3fvSUN) mogl_glunsupported("glTexCoord2fColor4ubVertex3fvSUN");
	glTexCoord2fColor4ubVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texcoord2fcolor3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor3fVertex3fSUN) mogl_glunsupported("glTexCoord2fColor3fVertex3fSUN");
	glTexCoord2fColor3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]));

}

void gl_texcoord2fcolor3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor3fVertex3fvSUN) mogl_glunsupported("glTexCoord2fColor3fVertex3fvSUN");
	glTexCoord2fColor3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texcoord2fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fNormal3fVertex3fSUN) mogl_glunsupported("glTexCoord2fNormal3fVertex3fSUN");
	glTexCoord2fNormal3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]));

}

void gl_texcoord2fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fNormal3fVertex3fvSUN) mogl_glunsupported("glTexCoord2fNormal3fVertex3fvSUN");
	glTexCoord2fNormal3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texcoord2fcolor4fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor4fNormal3fVertex3fSUN) mogl_glunsupported("glTexCoord2fColor4fNormal3fVertex3fSUN");
	glTexCoord2fColor4fNormal3fVertex3fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]),
		(GLfloat)mxGetScalar(prhs[10]),
		(GLfloat)mxGetScalar(prhs[11]));

}

void gl_texcoord2fcolor4fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord2fColor4fNormal3fVertex3fvSUN) mogl_glunsupported("glTexCoord2fColor4fNormal3fVertex3fvSUN");
	glTexCoord2fColor4fNormal3fVertex3fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_texcoord4fcolor4fnormal3fvertex4fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4fColor4fNormal3fVertex4fSUN) mogl_glunsupported("glTexCoord4fColor4fNormal3fVertex4fSUN");
	glTexCoord4fColor4fNormal3fVertex4fSUN((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]),
		(GLfloat)mxGetScalar(prhs[10]),
		(GLfloat)mxGetScalar(prhs[11]),
		(GLfloat)mxGetScalar(prhs[12]),
		(GLfloat)mxGetScalar(prhs[13]),
		(GLfloat)mxGetScalar(prhs[14]));

}

void gl_texcoord4fcolor4fnormal3fvertex4fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoord4fColor4fNormal3fVertex4fvSUN) mogl_glunsupported("glTexCoord4fColor4fNormal3fVertex4fvSUN");
	glTexCoord4fColor4fNormal3fVertex4fvSUN((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_replacementcodeuivertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiVertex3fSUN) mogl_glunsupported("glReplacementCodeuiVertex3fSUN");
	glReplacementCodeuiVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_replacementcodeuivertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiVertex3fvSUN");
	glReplacementCodeuiVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_replacementcodeuicolor4ubvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor4ubVertex3fSUN) mogl_glunsupported("glReplacementCodeuiColor4ubVertex3fSUN");
	glReplacementCodeuiColor4ubVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]));

}

void gl_replacementcodeuicolor4ubvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor4ubVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiColor4ubVertex3fvSUN");
	glReplacementCodeuiColor4ubVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_replacementcodeuicolor3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor3fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiColor3fVertex3fSUN");
	glReplacementCodeuiColor3fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]));

}

void gl_replacementcodeuicolor3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor3fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiColor3fVertex3fvSUN");
	glReplacementCodeuiColor3fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_replacementcodeuinormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiNormal3fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiNormal3fVertex3fSUN");
	glReplacementCodeuiNormal3fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]));

}

void gl_replacementcodeuinormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiNormal3fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiNormal3fVertex3fvSUN");
	glReplacementCodeuiNormal3fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_replacementcodeuicolor4fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor4fNormal3fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiColor4fNormal3fVertex3fSUN");
	glReplacementCodeuiColor4fNormal3fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]),
		(GLfloat)mxGetScalar(prhs[10]));

}

void gl_replacementcodeuicolor4fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiColor4fNormal3fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiColor4fNormal3fVertex3fvSUN");
	glReplacementCodeuiColor4fNormal3fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_replacementcodeuitexcoord2fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fVertex3fSUN");
	glReplacementCodeuiTexCoord2fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_replacementcodeuitexcoord2fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fVertex3fvSUN");
	glReplacementCodeuiTexCoord2fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_replacementcodeuitexcoord2fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN");
	glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]));

}

void gl_replacementcodeuitexcoord2fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN");
	glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_replacementcodeuitexcoord2fcolor4fnormal3fvertex3fsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN");
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(GLfloat)mxGetScalar(prhs[6]),
		(GLfloat)mxGetScalar(prhs[7]),
		(GLfloat)mxGetScalar(prhs[8]),
		(GLfloat)mxGetScalar(prhs[9]),
		(GLfloat)mxGetScalar(prhs[10]),
		(GLfloat)mxGetScalar(prhs[11]),
		(GLfloat)mxGetScalar(prhs[12]));

}

void gl_replacementcodeuitexcoord2fcolor4fnormal3fvertex3fvsun( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN) mogl_glunsupported("glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN");
	glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN((const GLuint*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]),
		(const GLfloat*)mxGetData(prhs[4]));

}

int gl_auto_map_count=2465;
cmdhandler gl_auto_map[] = {
{ "glAccum",                         gl_accum                            },
{ "glActiveProgramEXT",              gl_activeprogramext                 },
{ "glActiveShaderProgram",           gl_activeshaderprogram              },
{ "glActiveStencilFaceEXT",          gl_activestencilfaceext             },
{ "glActiveTexture",                 gl_activetexture                    },
{ "glActiveTextureARB",              gl_activetexturearb                 },
{ "glActiveVaryingNV",               gl_activevaryingnv                  },
{ "glAlphaFragmentOp1ATI",           gl_alphafragmentop1ati              },
{ "glAlphaFragmentOp2ATI",           gl_alphafragmentop2ati              },
{ "glAlphaFragmentOp3ATI",           gl_alphafragmentop3ati              },
{ "glAlphaFunc",                     gl_alphafunc                        },
{ "glApplyTextureEXT",               gl_applytextureext                  },
{ "glAreProgramsResidentNV",         gl_areprogramsresidentnv            },
{ "glAreTexturesResident",           gl_aretexturesresident              },
{ "glAreTexturesResidentEXT",        gl_aretexturesresidentext           },
{ "glArrayElement",                  gl_arrayelement                     },
{ "glArrayElementEXT",               gl_arrayelementext                  },
{ "glArrayObjectATI",                gl_arrayobjectati                   },
{ "glAsyncMarkerSGIX",               gl_asyncmarkersgix                  },
{ "glAttachObjectARB",               gl_attachobjectarb                  },
{ "glAttachShader",                  gl_attachshader                     },
{ "glBegin",                         gl_begin                            },
{ "glBeginConditionalRender",        gl_beginconditionalrender           },
{ "glBeginConditionalRenderNVX",     gl_beginconditionalrendernvx        },
{ "glBeginFragmentShaderATI",        gl_beginfragmentshaderati           },
{ "glBeginOcclusionQueryNV",         gl_beginocclusionquerynv            },
{ "glBeginPerfMonitorAMD",           gl_beginperfmonitoramd              },
{ "glBeginPerfQueryINTEL",           gl_beginperfqueryintel              },
{ "glBeginQuery",                    gl_beginquery                       },
{ "glBeginQueryARB",                 gl_beginqueryarb                    },
{ "glBeginQueryIndexed",             gl_beginqueryindexed                },
{ "glBeginTransformFeedback",        gl_begintransformfeedback           },
{ "glBeginTransformFeedbackEXT",     gl_begintransformfeedbackext        },
{ "glBeginTransformFeedbackNV",      gl_begintransformfeedbacknv         },
{ "glBeginVertexShaderEXT",          gl_beginvertexshaderext             },
{ "glBeginVideoCaptureNV",           gl_beginvideocapturenv              },
{ "glBindAttribLocation",            gl_bindattriblocation               },
{ "glBindAttribLocationARB",         gl_bindattriblocationarb            },
{ "glBindBuffer",                    gl_bindbuffer                       },
{ "glBindBufferARB",                 gl_bindbufferarb                    },
{ "glBindBufferBase",                gl_bindbufferbase                   },
{ "glBindBufferBaseEXT",             gl_bindbufferbaseext                },
{ "glBindBufferBaseNV",              gl_bindbufferbasenv                 },
{ "glBindBufferOffsetEXT",           gl_bindbufferoffsetext              },
{ "glBindBufferOffsetNV",            gl_bindbufferoffsetnv               },
{ "glBindBufferRange",               gl_bindbufferrange                  },
{ "glBindBufferRangeEXT",            gl_bindbufferrangeext               },
{ "glBindBufferRangeNV",             gl_bindbufferrangenv                },
{ "glBindBuffersBase",               gl_bindbuffersbase                  },
{ "glBindBuffersRange",              gl_bindbuffersrange                 },
{ "glBindFragDataLocation",          gl_bindfragdatalocation             },
{ "glBindFragDataLocationEXT",       gl_bindfragdatalocationext          },
{ "glBindFragDataLocationIndexed",   gl_bindfragdatalocationindexed      },
{ "glBindFragmentShaderATI",         gl_bindfragmentshaderati            },
{ "glBindFramebuffer",               gl_bindframebuffer                  },
{ "glBindFramebufferEXT",            gl_bindframebufferext               },
{ "glBindImageTexture",              gl_bindimagetexture                 },
{ "glBindImageTextureEXT",           gl_bindimagetextureext              },
{ "glBindImageTextures",             gl_bindimagetextures                },
{ "glBindLightParameterEXT",         gl_bindlightparameterext            },
{ "glBindMaterialParameterEXT",      gl_bindmaterialparameterext         },
{ "glBindMultiTextureEXT",           gl_bindmultitextureext              },
{ "glBindParameterEXT",              gl_bindparameterext                 },
{ "glBindProgramARB",                gl_bindprogramarb                   },
{ "glBindProgramNV",                 gl_bindprogramnv                    },
{ "glBindProgramPipeline",           gl_bindprogrampipeline              },
{ "glBindRenderbuffer",              gl_bindrenderbuffer                 },
{ "glBindRenderbufferEXT",           gl_bindrenderbufferext              },
{ "glBindSampler",                   gl_bindsampler                      },
{ "glBindSamplers",                  gl_bindsamplers                     },
{ "glBindTexGenParameterEXT",        gl_bindtexgenparameterext           },
{ "glBindTexture",                   gl_bindtexture                      },
{ "glBindTextureEXT",                gl_bindtextureext                   },
{ "glBindTextureUnit",               gl_bindtextureunit                  },
{ "glBindTextureUnitParameterEXT",   gl_bindtextureunitparameterext      },
{ "glBindTextures",                  gl_bindtextures                     },
{ "glBindTransformFeedback",         gl_bindtransformfeedback            },
{ "glBindTransformFeedbackNV",       gl_bindtransformfeedbacknv          },
{ "glBindVertexArray",               gl_bindvertexarray                  },
{ "glBindVertexArrayAPPLE",          gl_bindvertexarrayapple             },
{ "glBindVertexBuffer",              gl_bindvertexbuffer                 },
{ "glBindVertexBuffers",             gl_bindvertexbuffers                },
{ "glBindVertexShaderEXT",           gl_bindvertexshaderext              },
{ "glBindVideoCaptureStreamBufferNV",gl_bindvideocapturestreambuffernv   },
{ "glBindVideoCaptureStreamTextureNV",gl_bindvideocapturestreamtexturenv  },
{ "glBinormalPointerEXT",            gl_binormalpointerext               },
{ "glBitmap",                        gl_bitmap                           },
{ "glBlendBarrierKHR",               gl_blendbarrierkhr                  },
{ "glBlendBarrierNV",                gl_blendbarriernv                   },
{ "glBlendColor",                    gl_blendcolor                       },
{ "glBlendColorEXT",                 gl_blendcolorext                    },
{ "glBlendEquation",                 gl_blendequation                    },
{ "glBlendEquationEXT",              gl_blendequationext                 },
{ "glBlendEquationIndexedAMD",       gl_blendequationindexedamd          },
{ "glBlendEquationSeparate",         gl_blendequationseparate            },
{ "glBlendEquationSeparateEXT",      gl_blendequationseparateext         },
{ "glBlendEquationSeparateIndexedAMD",gl_blendequationseparateindexedamd  },
{ "glBlendEquationSeparatei",        gl_blendequationseparatei           },
{ "glBlendEquationSeparateiARB",     gl_blendequationseparateiarb        },
{ "glBlendEquationi",                gl_blendequationi                   },
{ "glBlendEquationiARB",             gl_blendequationiarb                },
{ "glBlendFunc",                     gl_blendfunc                        },
{ "glBlendFuncIndexedAMD",           gl_blendfuncindexedamd              },
{ "glBlendFuncSeparate",             gl_blendfuncseparate                },
{ "glBlendFuncSeparateEXT",          gl_blendfuncseparateext             },
{ "glBlendFuncSeparateIndexedAMD",   gl_blendfuncseparateindexedamd      },
{ "glBlendFuncSeparatei",            gl_blendfuncseparatei               },
{ "glBlendFuncSeparateiARB",         gl_blendfuncseparateiarb            },
{ "glBlendFunci",                    gl_blendfunci                       },
{ "glBlendFunciARB",                 gl_blendfunciarb                    },
{ "glBlendParameteriNV",             gl_blendparameterinv                },
{ "glBlitFramebuffer",               gl_blitframebuffer                  },
{ "glBlitFramebufferEXT",            gl_blitframebufferext               },
{ "glBlitNamedFramebuffer",          gl_blitnamedframebuffer             },
{ "glBufferAddressRangeNV",          gl_bufferaddressrangenv             },
{ "glBufferDataARB",                 gl_bufferdataarb                    },
{ "glBufferPageCommitmentARB",       gl_bufferpagecommitmentarb          },
{ "glBufferParameteriAPPLE",         gl_bufferparameteriapple            },
{ "glBufferStorage",                 gl_bufferstorage                    },
{ "glBufferSubData",                 gl_buffersubdata                    },
{ "glBufferSubDataARB",              gl_buffersubdataarb                 },
{ "glCallList",                      gl_calllist                         },
{ "glCallLists",                     gl_calllists                        },
{ "glCheckFramebufferStatus",        gl_checkframebufferstatus           },
{ "glCheckFramebufferStatusEXT",     gl_checkframebufferstatusext        },
{ "glCheckNamedFramebufferStatus",   gl_checknamedframebufferstatus      },
{ "glCheckNamedFramebufferStatusEXT",gl_checknamedframebufferstatusext   },
{ "glClampColor",                    gl_clampcolor                       },
{ "glClampColorARB",                 gl_clampcolorarb                    },
{ "glClear",                         gl_clear                            },
{ "glClearAccum",                    gl_clearaccum                       },
{ "glClearBufferData",               gl_clearbufferdata                  },
{ "glClearBufferSubData",            gl_clearbuffersubdata               },
{ "glClearBufferfi",                 gl_clearbufferfi                    },
{ "glClearBufferfv",                 gl_clearbufferfv                    },
{ "glClearBufferiv",                 gl_clearbufferiv                    },
{ "glClearBufferuiv",                gl_clearbufferuiv                   },
{ "glClearColor",                    gl_clearcolor                       },
{ "glClearColorIiEXT",               gl_clearcoloriiext                  },
{ "glClearColorIuiEXT",              gl_clearcoloriuiext                 },
{ "glClearDepth",                    gl_cleardepth                       },
{ "glClearDepthdNV",                 gl_cleardepthdnv                    },
{ "glClearDepthf",                   gl_cleardepthf                      },
{ "glClearDepthfOES",                gl_cleardepthfoes                   },
{ "glClearIndex",                    gl_clearindex                       },
{ "glClearNamedBufferData",          gl_clearnamedbufferdata             },
{ "glClearNamedBufferDataEXT",       gl_clearnamedbufferdataext          },
{ "glClearNamedBufferSubData",       gl_clearnamedbuffersubdata          },
{ "glClearNamedBufferSubDataEXT",    gl_clearnamedbuffersubdataext       },
{ "glClearNamedFramebufferfi",       gl_clearnamedframebufferfi          },
{ "glClearNamedFramebufferfv",       gl_clearnamedframebufferfv          },
{ "glClearNamedFramebufferiv",       gl_clearnamedframebufferiv          },
{ "glClearNamedFramebufferuiv",      gl_clearnamedframebufferuiv         },
{ "glClearStencil",                  gl_clearstencil                     },
{ "glClearTexImage",                 gl_clearteximage                    },
{ "glClearTexSubImage",              gl_cleartexsubimage                 },
{ "glClientActiveTexture",           gl_clientactivetexture              },
{ "glClientActiveTextureARB",        gl_clientactivetexturearb           },
{ "glClientActiveVertexStreamATI",   gl_clientactivevertexstreamati      },
{ "glClientAttribDefaultEXT",        gl_clientattribdefaultext           },
{ "glClientWaitSync",                gl_clientwaitsync                   },
{ "glClipControl",                   gl_clipcontrol                      },
{ "glClipPlane",                     gl_clipplane                        },
{ "glClipPlanefOES",                 gl_clipplanefoes                    },
{ "glColor3b",                       gl_color3b                          },
{ "glColor3bv",                      gl_color3bv                         },
{ "glColor3d",                       gl_color3d                          },
{ "glColor3dv",                      gl_color3dv                         },
{ "glColor3f",                       gl_color3f                          },
{ "glColor3fVertex3fSUN",            gl_color3fvertex3fsun               },
{ "glColor3fVertex3fvSUN",           gl_color3fvertex3fvsun              },
{ "glColor3fv",                      gl_color3fv                         },
{ "glColor3i",                       gl_color3i                          },
{ "glColor3iv",                      gl_color3iv                         },
{ "glColor3s",                       gl_color3s                          },
{ "glColor3sv",                      gl_color3sv                         },
{ "glColor3ub",                      gl_color3ub                         },
{ "glColor3ubv",                     gl_color3ubv                        },
{ "glColor3ui",                      gl_color3ui                         },
{ "glColor3uiv",                     gl_color3uiv                        },
{ "glColor3us",                      gl_color3us                         },
{ "glColor3usv",                     gl_color3usv                        },
{ "glColor4b",                       gl_color4b                          },
{ "glColor4bv",                      gl_color4bv                         },
{ "glColor4d",                       gl_color4d                          },
{ "glColor4dv",                      gl_color4dv                         },
{ "glColor4f",                       gl_color4f                          },
{ "glColor4fNormal3fVertex3fSUN",    gl_color4fnormal3fvertex3fsun       },
{ "glColor4fNormal3fVertex3fvSUN",   gl_color4fnormal3fvertex3fvsun      },
{ "glColor4fv",                      gl_color4fv                         },
{ "glColor4i",                       gl_color4i                          },
{ "glColor4iv",                      gl_color4iv                         },
{ "glColor4s",                       gl_color4s                          },
{ "glColor4sv",                      gl_color4sv                         },
{ "glColor4ub",                      gl_color4ub                         },
{ "glColor4ubVertex2fSUN",           gl_color4ubvertex2fsun              },
{ "glColor4ubVertex2fvSUN",          gl_color4ubvertex2fvsun             },
{ "glColor4ubVertex3fSUN",           gl_color4ubvertex3fsun              },
{ "glColor4ubVertex3fvSUN",          gl_color4ubvertex3fvsun             },
{ "glColor4ubv",                     gl_color4ubv                        },
{ "glColor4ui",                      gl_color4ui                         },
{ "glColor4uiv",                     gl_color4uiv                        },
{ "glColor4us",                      gl_color4us                         },
{ "glColor4usv",                     gl_color4usv                        },
{ "glColorFormatNV",                 gl_colorformatnv                    },
{ "glColorFragmentOp1ATI",           gl_colorfragmentop1ati              },
{ "glColorFragmentOp2ATI",           gl_colorfragmentop2ati              },
{ "glColorFragmentOp3ATI",           gl_colorfragmentop3ati              },
{ "glColorMask",                     gl_colormask                        },
{ "glColorMaskIndexedEXT",           gl_colormaskindexedext              },
{ "glColorMaski",                    gl_colormaski                       },
{ "glColorMaterial",                 gl_colormaterial                    },
{ "glColorP3ui",                     gl_colorp3ui                        },
{ "glColorP3uiv",                    gl_colorp3uiv                       },
{ "glColorP4ui",                     gl_colorp4ui                        },
{ "glColorP4uiv",                    gl_colorp4uiv                       },
{ "glColorPointerEXT",               gl_colorpointerext                  },
{ "glColorSubTable",                 gl_colorsubtable                    },
{ "glColorSubTableEXT",              gl_colorsubtableext                 },
{ "glColorTable",                    gl_colortable                       },
{ "glColorTableEXT",                 gl_colortableext                    },
{ "glColorTableParameterfv",         gl_colortableparameterfv            },
{ "glColorTableParameterfvSGI",      gl_colortableparameterfvsgi         },
{ "glColorTableParameteriv",         gl_colortableparameteriv            },
{ "glColorTableParameterivSGI",      gl_colortableparameterivsgi         },
{ "glColorTableSGI",                 gl_colortablesgi                    },
{ "glCombinerInputNV",               gl_combinerinputnv                  },
{ "glCombinerOutputNV",              gl_combineroutputnv                 },
{ "glCombinerParameterfNV",          gl_combinerparameterfnv             },
{ "glCombinerParameterfvNV",         gl_combinerparameterfvnv            },
{ "glCombinerParameteriNV",          gl_combinerparameterinv             },
{ "glCombinerParameterivNV",         gl_combinerparameterivnv            },
{ "glCombinerStageParameterfvNV",    gl_combinerstageparameterfvnv       },
{ "glCompileShader",                 gl_compileshader                    },
{ "glCompileShaderARB",              gl_compileshaderarb                 },
{ "glCompileShaderIncludeARB",       gl_compileshaderincludearb          },
{ "glCompressedMultiTexImage1DEXT",  gl_compressedmultiteximage1dext     },
{ "glCompressedMultiTexImage2DEXT",  gl_compressedmultiteximage2dext     },
{ "glCompressedMultiTexImage3DEXT",  gl_compressedmultiteximage3dext     },
{ "glCompressedMultiTexSubImage1DEXT",gl_compressedmultitexsubimage1dext  },
{ "glCompressedMultiTexSubImage2DEXT",gl_compressedmultitexsubimage2dext  },
{ "glCompressedMultiTexSubImage3DEXT",gl_compressedmultitexsubimage3dext  },
{ "glCompressedTexImage1D",          gl_compressedteximage1d             },
{ "glCompressedTexImage1DARB",       gl_compressedteximage1darb          },
{ "glCompressedTexImage2D",          gl_compressedteximage2d             },
{ "glCompressedTexImage2DARB",       gl_compressedteximage2darb          },
{ "glCompressedTexImage3D",          gl_compressedteximage3d             },
{ "glCompressedTexImage3DARB",       gl_compressedteximage3darb          },
{ "glCompressedTexSubImage1D",       gl_compressedtexsubimage1d          },
{ "glCompressedTexSubImage1DARB",    gl_compressedtexsubimage1darb       },
{ "glCompressedTexSubImage2D",       gl_compressedtexsubimage2d          },
{ "glCompressedTexSubImage2DARB",    gl_compressedtexsubimage2darb       },
{ "glCompressedTexSubImage3D",       gl_compressedtexsubimage3d          },
{ "glCompressedTexSubImage3DARB",    gl_compressedtexsubimage3darb       },
{ "glCompressedTextureImage1DEXT",   gl_compressedtextureimage1dext      },
{ "glCompressedTextureImage2DEXT",   gl_compressedtextureimage2dext      },
{ "glCompressedTextureImage3DEXT",   gl_compressedtextureimage3dext      },
{ "glCompressedTextureSubImage1D",   gl_compressedtexturesubimage1d      },
{ "glCompressedTextureSubImage1DEXT",gl_compressedtexturesubimage1dext   },
{ "glCompressedTextureSubImage2D",   gl_compressedtexturesubimage2d      },
{ "glCompressedTextureSubImage2DEXT",gl_compressedtexturesubimage2dext   },
{ "glCompressedTextureSubImage3D",   gl_compressedtexturesubimage3d      },
{ "glCompressedTextureSubImage3DEXT",gl_compressedtexturesubimage3dext   },
{ "glConvolutionFilter1D",           gl_convolutionfilter1d              },
{ "glConvolutionFilter1DEXT",        gl_convolutionfilter1dext           },
{ "glConvolutionFilter2D",           gl_convolutionfilter2d              },
{ "glConvolutionFilter2DEXT",        gl_convolutionfilter2dext           },
{ "glConvolutionParameterf",         gl_convolutionparameterf            },
{ "glConvolutionParameterfEXT",      gl_convolutionparameterfext         },
{ "glConvolutionParameterfv",        gl_convolutionparameterfv           },
{ "glConvolutionParameterfvEXT",     gl_convolutionparameterfvext        },
{ "glConvolutionParameteri",         gl_convolutionparameteri            },
{ "glConvolutionParameteriEXT",      gl_convolutionparameteriext         },
{ "glConvolutionParameteriv",        gl_convolutionparameteriv           },
{ "glConvolutionParameterivEXT",     gl_convolutionparameterivext        },
{ "glCopyBufferSubData",             gl_copybuffersubdata                },
{ "glCopyColorSubTable",             gl_copycolorsubtable                },
{ "glCopyColorSubTableEXT",          gl_copycolorsubtableext             },
{ "glCopyColorTable",                gl_copycolortable                   },
{ "glCopyColorTableSGI",             gl_copycolortablesgi                },
{ "glCopyConvolutionFilter1D",       gl_copyconvolutionfilter1d          },
{ "glCopyConvolutionFilter1DEXT",    gl_copyconvolutionfilter1dext       },
{ "glCopyConvolutionFilter2D",       gl_copyconvolutionfilter2d          },
{ "glCopyConvolutionFilter2DEXT",    gl_copyconvolutionfilter2dext       },
{ "glCopyImageSubData",              gl_copyimagesubdata                 },
{ "glCopyImageSubDataNV",            gl_copyimagesubdatanv               },
{ "glCopyMultiTexImage1DEXT",        gl_copymultiteximage1dext           },
{ "glCopyMultiTexImage2DEXT",        gl_copymultiteximage2dext           },
{ "glCopyMultiTexSubImage1DEXT",     gl_copymultitexsubimage1dext        },
{ "glCopyMultiTexSubImage2DEXT",     gl_copymultitexsubimage2dext        },
{ "glCopyMultiTexSubImage3DEXT",     gl_copymultitexsubimage3dext        },
{ "glCopyNamedBufferSubData",        gl_copynamedbuffersubdata           },
{ "glCopyPathNV",                    gl_copypathnv                       },
{ "glCopyPixels",                    gl_copypixels                       },
{ "glCopyTexImage1D",                gl_copyteximage1d                   },
{ "glCopyTexImage1DEXT",             gl_copyteximage1dext                },
{ "glCopyTexImage2D",                gl_copyteximage2d                   },
{ "glCopyTexImage2DEXT",             gl_copyteximage2dext                },
{ "glCopyTexSubImage1D",             gl_copytexsubimage1d                },
{ "glCopyTexSubImage1DEXT",          gl_copytexsubimage1dext             },
{ "glCopyTexSubImage2D",             gl_copytexsubimage2d                },
{ "glCopyTexSubImage2DEXT",          gl_copytexsubimage2dext             },
{ "glCopyTexSubImage3D",             gl_copytexsubimage3d                },
{ "glCopyTexSubImage3DEXT",          gl_copytexsubimage3dext             },
{ "glCopyTextureImage1DEXT",         gl_copytextureimage1dext            },
{ "glCopyTextureImage2DEXT",         gl_copytextureimage2dext            },
{ "glCopyTextureSubImage1D",         gl_copytexturesubimage1d            },
{ "glCopyTextureSubImage1DEXT",      gl_copytexturesubimage1dext         },
{ "glCopyTextureSubImage2D",         gl_copytexturesubimage2d            },
{ "glCopyTextureSubImage2DEXT",      gl_copytexturesubimage2dext         },
{ "glCopyTextureSubImage3D",         gl_copytexturesubimage3d            },
{ "glCopyTextureSubImage3DEXT",      gl_copytexturesubimage3dext         },
{ "glCoverFillPathInstancedNV",      gl_coverfillpathinstancednv         },
{ "glCoverFillPathNV",               gl_coverfillpathnv                  },
{ "glCoverStrokePathInstancedNV",    gl_coverstrokepathinstancednv       },
{ "glCoverStrokePathNV",             gl_coverstrokepathnv                },
{ "glCreateBuffers",                 gl_createbuffers                    },
{ "glCreateFramebuffers",            gl_createframebuffers               },
{ "glCreatePerfQueryINTEL",          gl_createperfqueryintel             },
{ "glCreateProgram",                 gl_createprogram                    },
{ "glCreateProgramObjectARB",        gl_createprogramobjectarb           },
{ "glCreateProgramPipelines",        gl_createprogrampipelines           },
{ "glCreateQueries",                 gl_createqueries                    },
{ "glCreateRenderbuffers",           gl_createrenderbuffers              },
{ "glCreateSamplers",                gl_createsamplers                   },
{ "glCreateShader",                  gl_createshader                     },
{ "glCreateShaderObjectARB",         gl_createshaderobjectarb            },
{ "glCreateShaderProgramEXT",        gl_createshaderprogramext           },
{ "glCreateTextures",                gl_createtextures                   },
{ "glCreateTransformFeedbacks",      gl_createtransformfeedbacks         },
{ "glCreateVertexArrays",            gl_createvertexarrays               },
{ "glCullFace",                      gl_cullface                         },
{ "glCullParameterdvEXT",            gl_cullparameterdvext               },
{ "glCullParameterfvEXT",            gl_cullparameterfvext               },
{ "glCurrentPaletteMatrixARB",       gl_currentpalettematrixarb          },
{ "glDebugMessageControl",           gl_debugmessagecontrol              },
{ "glDebugMessageControlARB",        gl_debugmessagecontrolarb           },
{ "glDebugMessageEnableAMD",         gl_debugmessageenableamd            },
{ "glDebugMessageInsert",            gl_debugmessageinsert               },
{ "glDebugMessageInsertAMD",         gl_debugmessageinsertamd            },
{ "glDebugMessageInsertARB",         gl_debugmessageinsertarb            },
{ "glDeleteAsyncMarkersSGIX",        gl_deleteasyncmarkerssgix           },
{ "glDeleteBuffers",                 gl_deletebuffers                    },
{ "glDeleteBuffersARB",              gl_deletebuffersarb                 },
{ "glDeleteFencesAPPLE",             gl_deletefencesapple                },
{ "glDeleteFencesNV",                gl_deletefencesnv                   },
{ "glDeleteFragmentShaderATI",       gl_deletefragmentshaderati          },
{ "glDeleteFramebuffers",            gl_deleteframebuffers               },
{ "glDeleteFramebuffersEXT",         gl_deleteframebuffersext            },
{ "glDeleteLists",                   gl_deletelists                      },
{ "glDeleteNamedStringARB",          gl_deletenamedstringarb             },
{ "glDeleteNamesAMD",                gl_deletenamesamd                   },
{ "glDeleteObjectARB",               gl_deleteobjectarb                  },
{ "glDeleteOcclusionQueriesNV",      gl_deleteocclusionqueriesnv         },
{ "glDeletePathsNV",                 gl_deletepathsnv                    },
{ "glDeletePerfMonitorsAMD",         gl_deleteperfmonitorsamd            },
{ "glDeletePerfQueryINTEL",          gl_deleteperfqueryintel             },
{ "glDeleteProgram",                 gl_deleteprogram                    },
{ "glDeleteProgramPipelines",        gl_deleteprogrampipelines           },
{ "glDeleteProgramsARB",             gl_deleteprogramsarb                },
{ "glDeleteProgramsNV",              gl_deleteprogramsnv                 },
{ "glDeleteQueries",                 gl_deletequeries                    },
{ "glDeleteQueriesARB",              gl_deletequeriesarb                 },
{ "glDeleteRenderbuffers",           gl_deleterenderbuffers              },
{ "glDeleteRenderbuffersEXT",        gl_deleterenderbuffersext           },
{ "glDeleteSamplers",                gl_deletesamplers                   },
{ "glDeleteShader",                  gl_deleteshader                     },
{ "glDeleteSync",                    gl_deletesync                       },
{ "glDeleteTextures",                gl_deletetextures                   },
{ "glDeleteTexturesEXT",             gl_deletetexturesext                },
{ "glDeleteTransformFeedbacks",      gl_deletetransformfeedbacks         },
{ "glDeleteTransformFeedbacksNV",    gl_deletetransformfeedbacksnv       },
{ "glDeleteVertexArrays",            gl_deletevertexarrays               },
{ "glDeleteVertexArraysAPPLE",       gl_deletevertexarraysapple          },
{ "glDeleteVertexShaderEXT",         gl_deletevertexshaderext            },
{ "glDepthBoundsEXT",                gl_depthboundsext                   },
{ "glDepthBoundsdNV",                gl_depthboundsdnv                   },
{ "glDepthFunc",                     gl_depthfunc                        },
{ "glDepthMask",                     gl_depthmask                        },
{ "glDepthRange",                    gl_depthrange                       },
{ "glDepthRangeArrayv",              gl_depthrangearrayv                 },
{ "glDepthRangeIndexed",             gl_depthrangeindexed                },
{ "glDepthRangedNV",                 gl_depthrangednv                    },
{ "glDepthRangef",                   gl_depthrangef                      },
{ "glDepthRangefOES",                gl_depthrangefoes                   },
{ "glDetachObjectARB",               gl_detachobjectarb                  },
{ "glDetachShader",                  gl_detachshader                     },
{ "glDetailTexFuncSGIS",             gl_detailtexfuncsgis                },
{ "glDisable",                       gl_disable                          },
{ "glDisableClientState",            gl_disableclientstate               },
{ "glDisableClientStateIndexedEXT",  gl_disableclientstateindexedext     },
{ "glDisableClientStateiEXT",        gl_disableclientstateiext           },
{ "glDisableIndexedEXT",             gl_disableindexedext                },
{ "glDisableVariantClientStateEXT",  gl_disablevariantclientstateext     },
{ "glDisableVertexArrayAttrib",      gl_disablevertexarrayattrib         },
{ "glDisableVertexArrayAttribEXT",   gl_disablevertexarrayattribext      },
{ "glDisableVertexArrayEXT",         gl_disablevertexarrayext            },
{ "glDisableVertexAttribAPPLE",      gl_disablevertexattribapple         },
{ "glDisableVertexAttribArray",      gl_disablevertexattribarray         },
{ "glDisableVertexAttribArrayARB",   gl_disablevertexattribarrayarb      },
{ "glDisablei",                      gl_disablei                         },
{ "glDispatchCompute",               gl_dispatchcompute                  },
{ "glDispatchComputeGroupSizeARB",   gl_dispatchcomputegroupsizearb      },
{ "glDispatchComputeIndirect",       gl_dispatchcomputeindirect          },
{ "glDrawArrays",                    gl_drawarrays                       },
{ "glDrawArraysEXT",                 gl_drawarraysext                    },
{ "glDrawArraysIndirect",            gl_drawarraysindirect               },
{ "glDrawArraysInstanced",           gl_drawarraysinstanced              },
{ "glDrawArraysInstancedARB",        gl_drawarraysinstancedarb           },
{ "glDrawArraysInstancedBaseInstance",gl_drawarraysinstancedbaseinstance  },
{ "glDrawArraysInstancedEXT",        gl_drawarraysinstancedext           },
{ "glDrawBuffer",                    gl_drawbuffer                       },
{ "glDrawBuffers",                   gl_drawbuffers                      },
{ "glDrawBuffersARB",                gl_drawbuffersarb                   },
{ "glDrawBuffersATI",                gl_drawbuffersati                   },
{ "glDrawElementArrayAPPLE",         gl_drawelementarrayapple            },
{ "glDrawElementArrayATI",           gl_drawelementarrayati              },
{ "glDrawElementsBaseVertex",        gl_drawelementsbasevertex           },
{ "glDrawElementsIndirect",          gl_drawelementsindirect             },
{ "glDrawElementsInstanced",         gl_drawelementsinstanced            },
{ "glDrawElementsInstancedARB",      gl_drawelementsinstancedarb         },
{ "glDrawElementsInstancedBaseInstance",gl_drawelementsinstancedbaseinstance },
{ "glDrawElementsInstancedBaseVertex",gl_drawelementsinstancedbasevertex  },
{ "glDrawElementsInstancedBaseVertexBaseInstance",gl_drawelementsinstancedbasevertexbaseinstance },
{ "glDrawElementsInstancedEXT",      gl_drawelementsinstancedext         },
{ "glDrawPixels",                    gl_drawpixels                       },
{ "glDrawRangeElementArrayAPPLE",    gl_drawrangeelementarrayapple       },
{ "glDrawRangeElementArrayATI",      gl_drawrangeelementarrayati         },
{ "glDrawRangeElementsBaseVertex",   gl_drawrangeelementsbasevertex      },
{ "glDrawRangeElementsEXT",          gl_drawrangeelementsext             },
{ "glDrawTextureNV",                 gl_drawtexturenv                    },
{ "glDrawTransformFeedback",         gl_drawtransformfeedback            },
{ "glDrawTransformFeedbackInstanced",gl_drawtransformfeedbackinstanced   },
{ "glDrawTransformFeedbackNV",       gl_drawtransformfeedbacknv          },
{ "glDrawTransformFeedbackStream",   gl_drawtransformfeedbackstream      },
{ "glDrawTransformFeedbackStreamInstanced",gl_drawtransformfeedbackstreaminstanced },
{ "glEdgeFlag",                      gl_edgeflag                         },
{ "glEdgeFlagFormatNV",              gl_edgeflagformatnv                 },
{ "glEdgeFlagPointer",               gl_edgeflagpointer                  },
{ "glEdgeFlagPointerEXT",            gl_edgeflagpointerext               },
{ "glEdgeFlagv",                     gl_edgeflagv                        },
{ "glElementPointerAPPLE",           gl_elementpointerapple              },
{ "glElementPointerATI",             gl_elementpointerati                },
{ "glEnable",                        gl_enable                           },
{ "glEnableClientState",             gl_enableclientstate                },
{ "glEnableClientStateIndexedEXT",   gl_enableclientstateindexedext      },
{ "glEnableClientStateiEXT",         gl_enableclientstateiext            },
{ "glEnableIndexedEXT",              gl_enableindexedext                 },
{ "glEnableVariantClientStateEXT",   gl_enablevariantclientstateext      },
{ "glEnableVertexArrayAttrib",       gl_enablevertexarrayattrib          },
{ "glEnableVertexArrayAttribEXT",    gl_enablevertexarrayattribext       },
{ "glEnableVertexArrayEXT",          gl_enablevertexarrayext             },
{ "glEnableVertexAttribAPPLE",       gl_enablevertexattribapple          },
{ "glEnableVertexAttribArray",       gl_enablevertexattribarray          },
{ "glEnableVertexAttribArrayARB",    gl_enablevertexattribarrayarb       },
{ "glEnablei",                       gl_enablei                          },
{ "glEnd",                           gl_end                              },
{ "glEndConditionalRender",          gl_endconditionalrender             },
{ "glEndConditionalRenderNVX",       gl_endconditionalrendernvx          },
{ "glEndFragmentShaderATI",          gl_endfragmentshaderati             },
{ "glEndList",                       gl_endlist                          },
{ "glEndOcclusionQueryNV",           gl_endocclusionquerynv              },
{ "glEndPerfMonitorAMD",             gl_endperfmonitoramd                },
{ "glEndPerfQueryINTEL",             gl_endperfqueryintel                },
{ "glEndQuery",                      gl_endquery                         },
{ "glEndQueryARB",                   gl_endqueryarb                      },
{ "glEndQueryIndexed",               gl_endqueryindexed                  },
{ "glEndTransformFeedback",          gl_endtransformfeedback             },
{ "glEndTransformFeedbackEXT",       gl_endtransformfeedbackext          },
{ "glEndTransformFeedbackNV",        gl_endtransformfeedbacknv           },
{ "glEndVertexShaderEXT",            gl_endvertexshaderext               },
{ "glEndVideoCaptureNV",             gl_endvideocapturenv                },
{ "glEvalCoord1d",                   gl_evalcoord1d                      },
{ "glEvalCoord1dv",                  gl_evalcoord1dv                     },
{ "glEvalCoord1f",                   gl_evalcoord1f                      },
{ "glEvalCoord1fv",                  gl_evalcoord1fv                     },
{ "glEvalCoord2d",                   gl_evalcoord2d                      },
{ "glEvalCoord2dv",                  gl_evalcoord2dv                     },
{ "glEvalCoord2f",                   gl_evalcoord2f                      },
{ "glEvalCoord2fv",                  gl_evalcoord2fv                     },
{ "glEvalMapsNV",                    gl_evalmapsnv                       },
{ "glEvalMesh1",                     gl_evalmesh1                        },
{ "glEvalMesh2",                     gl_evalmesh2                        },
{ "glEvalPoint1",                    gl_evalpoint1                       },
{ "glEvalPoint2",                    gl_evalpoint2                       },
{ "glExecuteProgramNV",              gl_executeprogramnv                 },
{ "glExtractComponentEXT",           gl_extractcomponentext              },
{ "glFenceSync",                     gl_fencesync                        },
{ "glFinalCombinerInputNV",          gl_finalcombinerinputnv             },
{ "glFinish",                        gl_finish                           },
{ "glFinishAsyncSGIX",               gl_finishasyncsgix                  },
{ "glFinishFenceAPPLE",              gl_finishfenceapple                 },
{ "glFinishFenceNV",                 gl_finishfencenv                    },
{ "glFinishObjectAPPLE",             gl_finishobjectapple                },
{ "glFinishTextureSUNX",             gl_finishtexturesunx                },
{ "glFlush",                         gl_flush                            },
{ "glFlushMappedBufferRange",        gl_flushmappedbufferrange           },
{ "glFlushMappedBufferRangeAPPLE",   gl_flushmappedbufferrangeapple      },
{ "glFlushMappedNamedBufferRange",   gl_flushmappednamedbufferrange      },
{ "glFlushMappedNamedBufferRangeEXT",gl_flushmappednamedbufferrangeext   },
{ "glFlushPixelDataRangeNV",         gl_flushpixeldatarangenv            },
{ "glFlushRasterSGIX",               gl_flushrastersgix                  },
{ "glFlushVertexArrayRangeAPPLE",    gl_flushvertexarrayrangeapple       },
{ "glFlushVertexArrayRangeNV",       gl_flushvertexarrayrangenv          },
{ "glFogCoordFormatNV",              gl_fogcoordformatnv                 },
{ "glFogCoordPointer",               gl_fogcoordpointer                  },
{ "glFogCoordPointerEXT",            gl_fogcoordpointerext               },
{ "glFogCoordd",                     gl_fogcoordd                        },
{ "glFogCoorddEXT",                  gl_fogcoorddext                     },
{ "glFogCoorddv",                    gl_fogcoorddv                       },
{ "glFogCoorddvEXT",                 gl_fogcoorddvext                    },
{ "glFogCoordf",                     gl_fogcoordf                        },
{ "glFogCoordfEXT",                  gl_fogcoordfext                     },
{ "glFogCoordfv",                    gl_fogcoordfv                       },
{ "glFogCoordfvEXT",                 gl_fogcoordfvext                    },
{ "glFogFuncSGIS",                   gl_fogfuncsgis                      },
{ "glFogf",                          gl_fogf                             },
{ "glFogfv",                         gl_fogfv                            },
{ "glFogi",                          gl_fogi                             },
{ "glFogiv",                         gl_fogiv                            },
{ "glFragmentColorMaterialSGIX",     gl_fragmentcolormaterialsgix        },
{ "glFragmentLightModelfSGIX",       gl_fragmentlightmodelfsgix          },
{ "glFragmentLightModelfvSGIX",      gl_fragmentlightmodelfvsgix         },
{ "glFragmentLightModeliSGIX",       gl_fragmentlightmodelisgix          },
{ "glFragmentLightModelivSGIX",      gl_fragmentlightmodelivsgix         },
{ "glFragmentLightfSGIX",            gl_fragmentlightfsgix               },
{ "glFragmentLightfvSGIX",           gl_fragmentlightfvsgix              },
{ "glFragmentLightiSGIX",            gl_fragmentlightisgix               },
{ "glFragmentLightivSGIX",           gl_fragmentlightivsgix              },
{ "glFragmentMaterialfSGIX",         gl_fragmentmaterialfsgix            },
{ "glFragmentMaterialfvSGIX",        gl_fragmentmaterialfvsgix           },
{ "glFragmentMaterialiSGIX",         gl_fragmentmaterialisgix            },
{ "glFragmentMaterialivSGIX",        gl_fragmentmaterialivsgix           },
{ "glFrameTerminatorGREMEDY",        gl_frameterminatorgremedy           },
{ "glFrameZoomSGIX",                 gl_framezoomsgix                    },
{ "glFramebufferDrawBufferEXT",      gl_framebufferdrawbufferext         },
{ "glFramebufferDrawBuffersEXT",     gl_framebufferdrawbuffersext        },
{ "glFramebufferParameteri",         gl_framebufferparameteri            },
{ "glFramebufferReadBufferEXT",      gl_framebufferreadbufferext         },
{ "glFramebufferRenderbuffer",       gl_framebufferrenderbuffer          },
{ "glFramebufferRenderbufferEXT",    gl_framebufferrenderbufferext       },
{ "glFramebufferTexture1D",          gl_framebuffertexture1d             },
{ "glFramebufferTexture1DEXT",       gl_framebuffertexture1dext          },
{ "glFramebufferTexture2D",          gl_framebuffertexture2d             },
{ "glFramebufferTexture2DEXT",       gl_framebuffertexture2dext          },
{ "glFramebufferTexture3D",          gl_framebuffertexture3d             },
{ "glFramebufferTexture3DEXT",       gl_framebuffertexture3dext          },
{ "glFramebufferTextureARB",         gl_framebuffertexturearb            },
{ "glFramebufferTextureEXT",         gl_framebuffertextureext            },
{ "glFramebufferTextureFaceARB",     gl_framebuffertexturefacearb        },
{ "glFramebufferTextureFaceEXT",     gl_framebuffertexturefaceext        },
{ "glFramebufferTextureLayer",       gl_framebuffertexturelayer          },
{ "glFramebufferTextureLayerARB",    gl_framebuffertexturelayerarb       },
{ "glFramebufferTextureLayerEXT",    gl_framebuffertexturelayerext       },
{ "glFreeObjectBufferATI",           gl_freeobjectbufferati              },
{ "glFrontFace",                     gl_frontface                        },
{ "glFrustum",                       gl_frustum                          },
{ "glFrustumfOES",                   gl_frustumfoes                      },
{ "glGenAsyncMarkersSGIX",           gl_genasyncmarkerssgix              },
{ "glGenBuffers",                    gl_genbuffers                       },
{ "glGenBuffersARB",                 gl_genbuffersarb                    },
{ "glGenFencesAPPLE",                gl_genfencesapple                   },
{ "glGenFencesNV",                   gl_genfencesnv                      },
{ "glGenFragmentShadersATI",         gl_genfragmentshadersati            },
{ "glGenFramebuffers",               gl_genframebuffers                  },
{ "glGenFramebuffersEXT",            gl_genframebuffersext               },
{ "glGenLists",                      gl_genlists                         },
{ "glGenNamesAMD",                   gl_gennamesamd                      },
{ "glGenOcclusionQueriesNV",         gl_genocclusionqueriesnv            },
{ "glGenPathsNV",                    gl_genpathsnv                       },
{ "glGenPerfMonitorsAMD",            gl_genperfmonitorsamd               },
{ "glGenProgramPipelines",           gl_genprogrampipelines              },
{ "glGenProgramsARB",                gl_genprogramsarb                   },
{ "glGenProgramsNV",                 gl_genprogramsnv                    },
{ "glGenQueries",                    gl_genqueries                       },
{ "glGenQueriesARB",                 gl_genqueriesarb                    },
{ "glGenRenderbuffers",              gl_genrenderbuffers                 },
{ "glGenRenderbuffersEXT",           gl_genrenderbuffersext              },
{ "glGenSamplers",                   gl_gensamplers                      },
{ "glGenSymbolsEXT",                 gl_gensymbolsext                    },
{ "glGenTextures",                   gl_gentextures                      },
{ "glGenTexturesEXT",                gl_gentexturesext                   },
{ "glGenTransformFeedbacks",         gl_gentransformfeedbacks            },
{ "glGenTransformFeedbacksNV",       gl_gentransformfeedbacksnv          },
{ "glGenVertexArrays",               gl_genvertexarrays                  },
{ "glGenVertexArraysAPPLE",          gl_genvertexarraysapple             },
{ "glGenVertexShadersEXT",           gl_genvertexshadersext              },
{ "glGenerateMipmap",                gl_generatemipmap                   },
{ "glGenerateMipmapEXT",             gl_generatemipmapext                },
{ "glGenerateMultiTexMipmapEXT",     gl_generatemultitexmipmapext        },
{ "glGenerateTextureMipmap",         gl_generatetexturemipmap            },
{ "glGenerateTextureMipmapEXT",      gl_generatetexturemipmapext         },
{ "glGetActiveAtomicCounterBufferiv",gl_getactiveatomiccounterbufferiv   },
{ "glGetActiveAttrib",               gl_getactiveattrib                  },
{ "glGetActiveAttribARB",            gl_getactiveattribarb               },
{ "glGetActiveSubroutineName",       gl_getactivesubroutinename          },
{ "glGetActiveSubroutineUniformName",gl_getactivesubroutineuniformname   },
{ "glGetActiveSubroutineUniformiv",  gl_getactivesubroutineuniformiv     },
{ "glGetActiveUniform",              gl_getactiveuniform                 },
{ "glGetActiveUniformARB",           gl_getactiveuniformarb              },
{ "glGetActiveUniformBlockName",     gl_getactiveuniformblockname        },
{ "glGetActiveUniformBlockiv",       gl_getactiveuniformblockiv          },
{ "glGetActiveUniformName",          gl_getactiveuniformname             },
{ "glGetActiveUniformsiv",           gl_getactiveuniformsiv              },
{ "glGetActiveVaryingNV",            gl_getactivevaryingnv               },
{ "glGetArrayObjectfvATI",           gl_getarrayobjectfvati              },
{ "glGetArrayObjectivATI",           gl_getarrayobjectivati              },
{ "glGetAttachedObjectsARB",         gl_getattachedobjectsarb            },
{ "glGetAttachedShaders",            gl_getattachedshaders               },
{ "glGetAttribLocation",             gl_getattriblocation                },
{ "glGetAttribLocationARB",          gl_getattriblocationarb             },
{ "glGetBooleanIndexedvEXT",         gl_getbooleanindexedvext            },
{ "glGetBooleani_v",                 gl_getbooleani_v                    },
{ "glGetBooleanv",                   gl_getbooleanv                      },
{ "glGetBufferParameteri64v",        gl_getbufferparameteri64v           },
{ "glGetBufferParameteriv",          gl_getbufferparameteriv             },
{ "glGetBufferParameterivARB",       gl_getbufferparameterivarb          },
{ "glGetBufferParameterui64vNV",     gl_getbufferparameterui64vnv        },
{ "glGetBufferSubData",              gl_getbuffersubdata                 },
{ "glGetBufferSubDataARB",           gl_getbuffersubdataarb              },
{ "glGetClipPlane",                  gl_getclipplane                     },
{ "glGetClipPlanefOES",              gl_getclipplanefoes                 },
{ "glGetColorTableEXT",              gl_getcolortableext                 },
{ "glGetColorTableParameterfvEXT",   gl_getcolortableparameterfvext      },
{ "glGetColorTableParameterfvSGI",   gl_getcolortableparameterfvsgi      },
{ "glGetColorTableParameterivEXT",   gl_getcolortableparameterivext      },
{ "glGetColorTableParameterivSGI",   gl_getcolortableparameterivsgi      },
{ "glGetColorTableSGI",              gl_getcolortablesgi                 },
{ "glGetCombinerInputParameterfvNV", gl_getcombinerinputparameterfvnv    },
{ "glGetCombinerInputParameterivNV", gl_getcombinerinputparameterivnv    },
{ "glGetCombinerOutputParameterfvNV",gl_getcombineroutputparameterfvnv   },
{ "glGetCombinerOutputParameterivNV",gl_getcombineroutputparameterivnv   },
{ "glGetCombinerStageParameterfvNV", gl_getcombinerstageparameterfvnv    },
{ "glGetCompressedMultiTexImageEXT", gl_getcompressedmultiteximageext    },
{ "glGetCompressedTexImage",         gl_getcompressedteximage            },
{ "glGetCompressedTexImageARB",      gl_getcompressedteximagearb         },
{ "glGetCompressedTextureImage",     gl_getcompressedtextureimage        },
{ "glGetCompressedTextureImageEXT",  gl_getcompressedtextureimageext     },
{ "glGetCompressedTextureSubImage",  gl_getcompressedtexturesubimage     },
{ "glGetConvolutionFilterEXT",       gl_getconvolutionfilterext          },
{ "glGetConvolutionParameterfvEXT",  gl_getconvolutionparameterfvext     },
{ "glGetConvolutionParameterivEXT",  gl_getconvolutionparameterivext     },
{ "glGetDebugMessageLog",            gl_getdebugmessagelog               },
{ "glGetDebugMessageLogAMD",         gl_getdebugmessagelogamd            },
{ "glGetDebugMessageLogARB",         gl_getdebugmessagelogarb            },
{ "glGetDetailTexFuncSGIS",          gl_getdetailtexfuncsgis             },
{ "glGetDoubleIndexedvEXT",          gl_getdoubleindexedvext             },
{ "glGetDoublei_v",                  gl_getdoublei_v                     },
{ "glGetDoublei_vEXT",               gl_getdoublei_vext                  },
{ "glGetDoublev",                    gl_getdoublev                       },
{ "glGetError",                      gl_geterror                         },
{ "glGetFenceivNV",                  gl_getfenceivnv                     },
{ "glGetFinalCombinerInputParameterfvNV",gl_getfinalcombinerinputparameterfvnv },
{ "glGetFinalCombinerInputParameterivNV",gl_getfinalcombinerinputparameterivnv },
{ "glGetFirstPerfQueryIdINTEL",      gl_getfirstperfqueryidintel         },
{ "glGetFloatIndexedvEXT",           gl_getfloatindexedvext              },
{ "glGetFloati_v",                   gl_getfloati_v                      },
{ "glGetFloati_vEXT",                gl_getfloati_vext                   },
{ "glGetFloatv",                     gl_getfloatv                        },
{ "glGetFogFuncSGIS",                gl_getfogfuncsgis                   },
{ "glGetFragDataIndex",              gl_getfragdataindex                 },
{ "glGetFragDataLocation",           gl_getfragdatalocation              },
{ "glGetFragDataLocationEXT",        gl_getfragdatalocationext           },
{ "glGetFragmentLightfvSGIX",        gl_getfragmentlightfvsgix           },
{ "glGetFragmentLightivSGIX",        gl_getfragmentlightivsgix           },
{ "glGetFragmentMaterialfvSGIX",     gl_getfragmentmaterialfvsgix        },
{ "glGetFragmentMaterialivSGIX",     gl_getfragmentmaterialivsgix        },
{ "glGetFramebufferAttachmentParameteriv",gl_getframebufferattachmentparameteriv },
{ "glGetFramebufferAttachmentParameterivEXT",gl_getframebufferattachmentparameterivext },
{ "glGetFramebufferParameteriv",     gl_getframebufferparameteriv        },
{ "glGetFramebufferParameterivEXT",  gl_getframebufferparameterivext     },
{ "glGetGraphicsResetStatusARB",     gl_getgraphicsresetstatusarb        },
{ "glGetHandleARB",                  gl_gethandlearb                     },
{ "glGetHistogramEXT",               gl_gethistogramext                  },
{ "glGetHistogramParameterfvEXT",    gl_gethistogramparameterfvext       },
{ "glGetHistogramParameterivEXT",    gl_gethistogramparameterivext       },
{ "glGetImageHandleARB",             gl_getimagehandlearb                },
{ "glGetImageHandleNV",              gl_getimagehandlenv                 },
{ "glGetImageTransformParameterfvHP",gl_getimagetransformparameterfvhp   },
{ "glGetImageTransformParameterivHP",gl_getimagetransformparameterivhp   },
{ "glGetInfoLogARB",                 gl_getinfologarb                    },
{ "glGetInteger64i_v",               gl_getinteger64i_v                  },
{ "glGetInteger64v",                 gl_getinteger64v                    },
{ "glGetIntegerIndexedvEXT",         gl_getintegerindexedvext            },
{ "glGetIntegeri_v",                 gl_getintegeri_v                    },
{ "glGetIntegerui64i_vNV",           gl_getintegerui64i_vnv              },
{ "glGetIntegerui64vNV",             gl_getintegerui64vnv                },
{ "glGetIntegerv",                   gl_getintegerv                      },
{ "glGetInternalformati64v",         gl_getinternalformati64v            },
{ "glGetInternalformativ",           gl_getinternalformativ              },
{ "glGetInvariantBooleanvEXT",       gl_getinvariantbooleanvext          },
{ "glGetInvariantFloatvEXT",         gl_getinvariantfloatvext            },
{ "glGetInvariantIntegervEXT",       gl_getinvariantintegervext          },
{ "glGetLightfv",                    gl_getlightfv                       },
{ "glGetLightiv",                    gl_getlightiv                       },
{ "glGetLocalConstantBooleanvEXT",   gl_getlocalconstantbooleanvext      },
{ "glGetLocalConstantFloatvEXT",     gl_getlocalconstantfloatvext        },
{ "glGetLocalConstantIntegervEXT",   gl_getlocalconstantintegervext      },
{ "glGetMapAttribParameterfvNV",     gl_getmapattribparameterfvnv        },
{ "glGetMapAttribParameterivNV",     gl_getmapattribparameterivnv        },
{ "glGetMapControlPointsNV",         gl_getmapcontrolpointsnv            },
{ "glGetMapParameterfvNV",           gl_getmapparameterfvnv              },
{ "glGetMapParameterivNV",           gl_getmapparameterivnv              },
{ "glGetMapdv",                      gl_getmapdv                         },
{ "glGetMapfv",                      gl_getmapfv                         },
{ "glGetMapiv",                      gl_getmapiv                         },
{ "glGetMaterialfv",                 gl_getmaterialfv                    },
{ "glGetMaterialiv",                 gl_getmaterialiv                    },
{ "glGetMinmaxEXT",                  gl_getminmaxext                     },
{ "glGetMinmaxParameterfv",          gl_getminmaxparameterfv             },
{ "glGetMinmaxParameterfvEXT",       gl_getminmaxparameterfvext          },
{ "glGetMinmaxParameteriv",          gl_getminmaxparameteriv             },
{ "glGetMinmaxParameterivEXT",       gl_getminmaxparameterivext          },
{ "glGetMultiTexEnvfvEXT",           gl_getmultitexenvfvext              },
{ "glGetMultiTexEnvivEXT",           gl_getmultitexenvivext              },
{ "glGetMultiTexGendvEXT",           gl_getmultitexgendvext              },
{ "glGetMultiTexGenfvEXT",           gl_getmultitexgenfvext              },
{ "glGetMultiTexGenivEXT",           gl_getmultitexgenivext              },
{ "glGetMultiTexImageEXT",           gl_getmultiteximageext              },
{ "glGetMultiTexLevelParameterfvEXT",gl_getmultitexlevelparameterfvext   },
{ "glGetMultiTexLevelParameterivEXT",gl_getmultitexlevelparameterivext   },
{ "glGetMultiTexParameterIivEXT",    gl_getmultitexparameteriivext       },
{ "glGetMultiTexParameterIuivEXT",   gl_getmultitexparameteriuivext      },
{ "glGetMultiTexParameterfvEXT",     gl_getmultitexparameterfvext        },
{ "glGetMultiTexParameterivEXT",     gl_getmultitexparameterivext        },
{ "glGetMultisamplefv",              gl_getmultisamplefv                 },
{ "glGetMultisamplefvNV",            gl_getmultisamplefvnv               },
{ "glGetNamedBufferParameteri64v",   gl_getnamedbufferparameteri64v      },
{ "glGetNamedBufferParameteriv",     gl_getnamedbufferparameteriv        },
{ "glGetNamedBufferParameterivEXT",  gl_getnamedbufferparameterivext     },
{ "glGetNamedBufferParameterui64vNV",gl_getnamedbufferparameterui64vnv   },
{ "glGetNamedBufferSubData",         gl_getnamedbuffersubdata            },
{ "glGetNamedBufferSubDataEXT",      gl_getnamedbuffersubdataext         },
{ "glGetNamedFramebufferAttachmentParameteriv",gl_getnamedframebufferattachmentparameteriv },
{ "glGetNamedFramebufferAttachmentParameterivEXT",gl_getnamedframebufferattachmentparameterivext },
{ "glGetNamedFramebufferParameteriv",gl_getnamedframebufferparameteriv   },
{ "glGetNamedFramebufferParameterivEXT",gl_getnamedframebufferparameterivext },
{ "glGetNamedProgramLocalParameterIivEXT",gl_getnamedprogramlocalparameteriivext },
{ "glGetNamedProgramLocalParameterIuivEXT",gl_getnamedprogramlocalparameteriuivext },
{ "glGetNamedProgramLocalParameterdvEXT",gl_getnamedprogramlocalparameterdvext },
{ "glGetNamedProgramLocalParameterfvEXT",gl_getnamedprogramlocalparameterfvext },
{ "glGetNamedProgramStringEXT",      gl_getnamedprogramstringext         },
{ "glGetNamedProgramivEXT",          gl_getnamedprogramivext             },
{ "glGetNamedRenderbufferParameteriv",gl_getnamedrenderbufferparameteriv  },
{ "glGetNamedRenderbufferParameterivEXT",gl_getnamedrenderbufferparameterivext },
{ "glGetNamedStringARB",             gl_getnamedstringarb                },
{ "glGetNamedStringivARB",           gl_getnamedstringivarb              },
{ "glGetNextPerfQueryIdINTEL",       gl_getnextperfqueryidintel          },
{ "glGetObjectBufferfvATI",          gl_getobjectbufferfvati             },
{ "glGetObjectBufferivATI",          gl_getobjectbufferivati             },
{ "glGetObjectLabel",                gl_getobjectlabel                   },
{ "glGetObjectLabelEXT",             gl_getobjectlabelext                },
{ "glGetObjectParameterfvARB",       gl_getobjectparameterfvarb          },
{ "glGetObjectParameterivAPPLE",     gl_getobjectparameterivapple        },
{ "glGetObjectParameterivARB",       gl_getobjectparameterivarb          },
{ "glGetObjectPtrLabel",             gl_getobjectptrlabel                },
{ "glGetOcclusionQueryivNV",         gl_getocclusionqueryivnv            },
{ "glGetOcclusionQueryuivNV",        gl_getocclusionqueryuivnv           },
{ "glGetPathColorGenfvNV",           gl_getpathcolorgenfvnv              },
{ "glGetPathColorGenivNV",           gl_getpathcolorgenivnv              },
{ "glGetPathCommandsNV",             gl_getpathcommandsnv                },
{ "glGetPathCoordsNV",               gl_getpathcoordsnv                  },
{ "glGetPathDashArrayNV",            gl_getpathdasharraynv               },
{ "glGetPathLengthNV",               gl_getpathlengthnv                  },
{ "glGetPathMetricRangeNV",          gl_getpathmetricrangenv             },
{ "glGetPathMetricsNV",              gl_getpathmetricsnv                 },
{ "glGetPathParameterfvNV",          gl_getpathparameterfvnv             },
{ "glGetPathParameterivNV",          gl_getpathparameterivnv             },
{ "glGetPathSpacingNV",              gl_getpathspacingnv                 },
{ "glGetPathTexGenfvNV",             gl_getpathtexgenfvnv                },
{ "glGetPathTexGenivNV",             gl_getpathtexgenivnv                },
{ "glGetPerfCounterInfoINTEL",       gl_getperfcounterinfointel          },
{ "glGetPerfMonitorCounterDataAMD",  gl_getperfmonitorcounterdataamd     },
{ "glGetPerfMonitorCounterInfoAMD",  gl_getperfmonitorcounterinfoamd     },
{ "glGetPerfMonitorCounterStringAMD",gl_getperfmonitorcounterstringamd   },
{ "glGetPerfMonitorCountersAMD",     gl_getperfmonitorcountersamd        },
{ "glGetPerfMonitorGroupStringAMD",  gl_getperfmonitorgroupstringamd     },
{ "glGetPerfMonitorGroupsAMD",       gl_getperfmonitorgroupsamd          },
{ "glGetPerfQueryDataINTEL",         gl_getperfquerydataintel            },
{ "glGetPerfQueryIdByNameINTEL",     gl_getperfqueryidbynameintel        },
{ "glGetPerfQueryInfoINTEL",         gl_getperfqueryinfointel            },
{ "glGetPixelMapfv",                 gl_getpixelmapfv                    },
{ "glGetPixelMapuiv",                gl_getpixelmapuiv                   },
{ "glGetPixelMapusv",                gl_getpixelmapusv                   },
{ "glGetPixelTransformParameterfvEXT",gl_getpixeltransformparameterfvext  },
{ "glGetPixelTransformParameterivEXT",gl_getpixeltransformparameterivext  },
{ "glGetPolygonStipple",             gl_getpolygonstipple                },
{ "glGetProgramBinary",              gl_getprogrambinary                 },
{ "glGetProgramEnvParameterdvARB",   gl_getprogramenvparameterdvarb      },
{ "glGetProgramEnvParameterfvARB",   gl_getprogramenvparameterfvarb      },
{ "glGetProgramInfoLog",             gl_getprograminfolog                },
{ "glGetProgramInterfaceiv",         gl_getprograminterfaceiv            },
{ "glGetProgramLocalParameterdvARB", gl_getprogramlocalparameterdvarb    },
{ "glGetProgramLocalParameterfvARB", gl_getprogramlocalparameterfvarb    },
{ "glGetProgramNamedParameterdvNV",  gl_getprogramnamedparameterdvnv     },
{ "glGetProgramNamedParameterfvNV",  gl_getprogramnamedparameterfvnv     },
{ "glGetProgramParameterdvNV",       gl_getprogramparameterdvnv          },
{ "glGetProgramParameterfvNV",       gl_getprogramparameterfvnv          },
{ "glGetProgramPipelineInfoLog",     gl_getprogrampipelineinfolog        },
{ "glGetProgramPipelineiv",          gl_getprogrampipelineiv             },
{ "glGetProgramResourceIndex",       gl_getprogramresourceindex          },
{ "glGetProgramResourceLocation",    gl_getprogramresourcelocation       },
{ "glGetProgramResourceLocationIndex",gl_getprogramresourcelocationindex  },
{ "glGetProgramResourceName",        gl_getprogramresourcename           },
{ "glGetProgramResourceiv",          gl_getprogramresourceiv             },
{ "glGetProgramStageiv",             gl_getprogramstageiv                },
{ "glGetProgramStringARB",           gl_getprogramstringarb              },
{ "glGetProgramStringNV",            gl_getprogramstringnv               },
{ "glGetProgramiv",                  gl_getprogramiv                     },
{ "glGetProgramivARB",               gl_getprogramivarb                  },
{ "glGetProgramivNV",                gl_getprogramivnv                   },
{ "glGetQueryIndexediv",             gl_getqueryindexediv                },
{ "glGetQueryObjecti64v",            gl_getqueryobjecti64v               },
{ "glGetQueryObjecti64vEXT",         gl_getqueryobjecti64vext            },
{ "glGetQueryObjectiv",              gl_getqueryobjectiv                 },
{ "glGetQueryObjectivARB",           gl_getqueryobjectivarb              },
{ "glGetQueryObjectui64v",           gl_getqueryobjectui64v              },
{ "glGetQueryObjectui64vEXT",        gl_getqueryobjectui64vext           },
{ "glGetQueryObjectuiv",             gl_getqueryobjectuiv                },
{ "glGetQueryObjectuivARB",          gl_getqueryobjectuivarb             },
{ "glGetQueryiv",                    gl_getqueryiv                       },
{ "glGetQueryivARB",                 gl_getqueryivarb                    },
{ "glGetRenderbufferParameteriv",    gl_getrenderbufferparameteriv       },
{ "glGetRenderbufferParameterivEXT", gl_getrenderbufferparameterivext    },
{ "glGetSamplerParameterIiv",        gl_getsamplerparameteriiv           },
{ "glGetSamplerParameterIuiv",       gl_getsamplerparameteriuiv          },
{ "glGetSamplerParameterfv",         gl_getsamplerparameterfv            },
{ "glGetSamplerParameteriv",         gl_getsamplerparameteriv            },
{ "glGetSeparableFilterEXT",         gl_getseparablefilterext            },
{ "glGetShaderInfoLog",              gl_getshaderinfolog                 },
{ "glGetShaderPrecisionFormat",      gl_getshaderprecisionformat         },
{ "glGetShaderSource",               gl_getshadersource                  },
{ "glGetShaderSourceARB",            gl_getshadersourcearb               },
{ "glGetShaderiv",                   gl_getshaderiv                      },
{ "glGetSharpenTexFuncSGIS",         gl_getsharpentexfuncsgis            },
{ "glGetStringi",                    gl_getstringi                       },
{ "glGetSubroutineIndex",            gl_getsubroutineindex               },
{ "glGetSubroutineUniformLocation",  gl_getsubroutineuniformlocation     },
{ "glGetSynciv",                     gl_getsynciv                        },
{ "glGetTexBumpParameterfvATI",      gl_gettexbumpparameterfvati         },
{ "glGetTexBumpParameterivATI",      gl_gettexbumpparameterivati         },
{ "glGetTexEnvfv",                   gl_gettexenvfv                      },
{ "glGetTexEnviv",                   gl_gettexenviv                      },
{ "glGetTexFilterFuncSGIS",          gl_gettexfilterfuncsgis             },
{ "glGetTexGendv",                   gl_gettexgendv                      },
{ "glGetTexGenfv",                   gl_gettexgenfv                      },
{ "glGetTexGeniv",                   gl_gettexgeniv                      },
{ "glGetTexImage",                   gl_getteximage                      },
{ "glGetTexLevelParameterfv",        gl_gettexlevelparameterfv           },
{ "glGetTexLevelParameteriv",        gl_gettexlevelparameteriv           },
{ "glGetTexParameterIiv",            gl_gettexparameteriiv               },
{ "glGetTexParameterIivEXT",         gl_gettexparameteriivext            },
{ "glGetTexParameterIuiv",           gl_gettexparameteriuiv              },
{ "glGetTexParameterIuivEXT",        gl_gettexparameteriuivext           },
{ "glGetTexParameterfv",             gl_gettexparameterfv                },
{ "glGetTexParameteriv",             gl_gettexparameteriv                },
{ "glGetTextureHandleARB",           gl_gettexturehandlearb              },
{ "glGetTextureHandleNV",            gl_gettexturehandlenv               },
{ "glGetTextureImage",               gl_gettextureimage                  },
{ "glGetTextureImageEXT",            gl_gettextureimageext               },
{ "glGetTextureLevelParameterfv",    gl_gettexturelevelparameterfv       },
{ "glGetTextureLevelParameterfvEXT", gl_gettexturelevelparameterfvext    },
{ "glGetTextureLevelParameteriv",    gl_gettexturelevelparameteriv       },
{ "glGetTextureLevelParameterivEXT", gl_gettexturelevelparameterivext    },
{ "glGetTextureParameterIiv",        gl_gettextureparameteriiv           },
{ "glGetTextureParameterIivEXT",     gl_gettextureparameteriivext        },
{ "glGetTextureParameterIuiv",       gl_gettextureparameteriuiv          },
{ "glGetTextureParameterIuivEXT",    gl_gettextureparameteriuivext       },
{ "glGetTextureParameterfv",         gl_gettextureparameterfv            },
{ "glGetTextureParameterfvEXT",      gl_gettextureparameterfvext         },
{ "glGetTextureParameteriv",         gl_gettextureparameteriv            },
{ "glGetTextureParameterivEXT",      gl_gettextureparameterivext         },
{ "glGetTextureSamplerHandleARB",    gl_gettexturesamplerhandlearb       },
{ "glGetTextureSamplerHandleNV",     gl_gettexturesamplerhandlenv        },
{ "glGetTextureSubImage",            gl_gettexturesubimage               },
{ "glGetTrackMatrixivNV",            gl_gettrackmatrixivnv               },
{ "glGetTransformFeedbackVarying",   gl_gettransformfeedbackvarying      },
{ "glGetTransformFeedbackVaryingEXT",gl_gettransformfeedbackvaryingext   },
{ "glGetTransformFeedbackVaryingNV", gl_gettransformfeedbackvaryingnv    },
{ "glGetTransformFeedbacki64_v",     gl_gettransformfeedbacki64_v        },
{ "glGetTransformFeedbacki_v",       gl_gettransformfeedbacki_v          },
{ "glGetTransformFeedbackiv",        gl_gettransformfeedbackiv           },
{ "glGetUniformBlockIndex",          gl_getuniformblockindex             },
{ "glGetUniformBufferSizeEXT",       gl_getuniformbuffersizeext          },
{ "glGetUniformLocation",            gl_getuniformlocation               },
{ "glGetUniformLocationARB",         gl_getuniformlocationarb            },
{ "glGetUniformOffsetEXT",           gl_getuniformoffsetext              },
{ "glGetUniformSubroutineuiv",       gl_getuniformsubroutineuiv          },
{ "glGetUniformdv",                  gl_getuniformdv                     },
{ "glGetUniformfv",                  gl_getuniformfv                     },
{ "glGetUniformfvARB",               gl_getuniformfvarb                  },
{ "glGetUniformi64vNV",              gl_getuniformi64vnv                 },
{ "glGetUniformiv",                  gl_getuniformiv                     },
{ "glGetUniformivARB",               gl_getuniformivarb                  },
{ "glGetUniformui64vNV",             gl_getuniformui64vnv                },
{ "glGetUniformuiv",                 gl_getuniformuiv                    },
{ "glGetUniformuivEXT",              gl_getuniformuivext                 },
{ "glGetVariantArrayObjectfvATI",    gl_getvariantarrayobjectfvati       },
{ "glGetVariantArrayObjectivATI",    gl_getvariantarrayobjectivati       },
{ "glGetVariantBooleanvEXT",         gl_getvariantbooleanvext            },
{ "glGetVariantFloatvEXT",           gl_getvariantfloatvext              },
{ "glGetVariantIntegervEXT",         gl_getvariantintegervext            },
{ "glGetVaryingLocationNV",          gl_getvaryinglocationnv             },
{ "glGetVertexArrayIndexed64iv",     gl_getvertexarrayindexed64iv        },
{ "glGetVertexArrayIndexediv",       gl_getvertexarrayindexediv          },
{ "glGetVertexArrayIntegeri_vEXT",   gl_getvertexarrayintegeri_vext      },
{ "glGetVertexArrayIntegervEXT",     gl_getvertexarrayintegervext        },
{ "glGetVertexArrayiv",              gl_getvertexarrayiv                 },
{ "glGetVertexAttribArrayObjectfvATI",gl_getvertexattribarrayobjectfvati  },
{ "glGetVertexAttribArrayObjectivATI",gl_getvertexattribarrayobjectivati  },
{ "glGetVertexAttribIiv",            gl_getvertexattribiiv               },
{ "glGetVertexAttribIivEXT",         gl_getvertexattribiivext            },
{ "glGetVertexAttribIuiv",           gl_getvertexattribiuiv              },
{ "glGetVertexAttribIuivEXT",        gl_getvertexattribiuivext           },
{ "glGetVertexAttribLdv",            gl_getvertexattribldv               },
{ "glGetVertexAttribLdvEXT",         gl_getvertexattribldvext            },
{ "glGetVertexAttribLi64vNV",        gl_getvertexattribli64vnv           },
{ "glGetVertexAttribLui64vARB",      gl_getvertexattriblui64varb         },
{ "glGetVertexAttribLui64vNV",       gl_getvertexattriblui64vnv          },
{ "glGetVertexAttribdv",             gl_getvertexattribdv                },
{ "glGetVertexAttribdvARB",          gl_getvertexattribdvarb             },
{ "glGetVertexAttribdvNV",           gl_getvertexattribdvnv              },
{ "glGetVertexAttribfv",             gl_getvertexattribfv                },
{ "glGetVertexAttribfvARB",          gl_getvertexattribfvarb             },
{ "glGetVertexAttribfvNV",           gl_getvertexattribfvnv              },
{ "glGetVertexAttribiv",             gl_getvertexattribiv                },
{ "glGetVertexAttribivARB",          gl_getvertexattribivarb             },
{ "glGetVertexAttribivNV",           gl_getvertexattribivnv              },
{ "glGetVideoCaptureStreamdvNV",     gl_getvideocapturestreamdvnv        },
{ "glGetVideoCaptureStreamfvNV",     gl_getvideocapturestreamfvnv        },
{ "glGetVideoCaptureStreamivNV",     gl_getvideocapturestreamivnv        },
{ "glGetVideoCaptureivNV",           gl_getvideocaptureivnv              },
{ "glGetVideoi64vNV",                gl_getvideoi64vnv                   },
{ "glGetVideoivNV",                  gl_getvideoivnv                     },
{ "glGetVideoui64vNV",               gl_getvideoui64vnv                  },
{ "glGetVideouivNV",                 gl_getvideouivnv                    },
{ "glGetnCompressedTexImageARB",     gl_getncompressedteximagearb        },
{ "glGetnMapdvARB",                  gl_getnmapdvarb                     },
{ "glGetnMapfvARB",                  gl_getnmapfvarb                     },
{ "glGetnMapivARB",                  gl_getnmapivarb                     },
{ "glGetnPolygonStippleARB",         gl_getnpolygonstipplearb            },
{ "glGetnTexImageARB",               gl_getnteximagearb                  },
{ "glGetnUniformdvARB",              gl_getnuniformdvarb                 },
{ "glGetnUniformfv",                 gl_getnuniformfv                    },
{ "glGetnUniformfvARB",              gl_getnuniformfvarb                 },
{ "glGetnUniformiv",                 gl_getnuniformiv                    },
{ "glGetnUniformivARB",              gl_getnuniformivarb                 },
{ "glGetnUniformuiv",                gl_getnuniformuiv                   },
{ "glGetnUniformuivARB",             gl_getnuniformuivarb                },
{ "glGlobalAlphaFactorbSUN",         gl_globalalphafactorbsun            },
{ "glGlobalAlphaFactordSUN",         gl_globalalphafactordsun            },
{ "glGlobalAlphaFactorfSUN",         gl_globalalphafactorfsun            },
{ "glGlobalAlphaFactoriSUN",         gl_globalalphafactorisun            },
{ "glGlobalAlphaFactorsSUN",         gl_globalalphafactorssun            },
{ "glGlobalAlphaFactorubSUN",        gl_globalalphafactorubsun           },
{ "glGlobalAlphaFactoruiSUN",        gl_globalalphafactoruisun           },
{ "glGlobalAlphaFactorusSUN",        gl_globalalphafactorussun           },
{ "glHint",                          gl_hint                             },
{ "glHistogram",                     gl_histogram                        },
{ "glHistogramEXT",                  gl_histogramext                     },
{ "glImageTransformParameterfHP",    gl_imagetransformparameterfhp       },
{ "glImageTransformParameterfvHP",   gl_imagetransformparameterfvhp      },
{ "glImageTransformParameteriHP",    gl_imagetransformparameterihp       },
{ "glImageTransformParameterivHP",   gl_imagetransformparameterivhp      },
{ "glImportSyncEXT",                 gl_importsyncext                    },
{ "glIndexFormatNV",                 gl_indexformatnv                    },
{ "glIndexFuncEXT",                  gl_indexfuncext                     },
{ "glIndexMask",                     gl_indexmask                        },
{ "glIndexMaterialEXT",              gl_indexmaterialext                 },
{ "glIndexPointer",                  gl_indexpointer                     },
{ "glIndexPointerEXT",               gl_indexpointerext                  },
{ "glIndexd",                        gl_indexd                           },
{ "glIndexdv",                       gl_indexdv                          },
{ "glIndexf",                        gl_indexf                           },
{ "glIndexfv",                       gl_indexfv                          },
{ "glIndexi",                        gl_indexi                           },
{ "glIndexiv",                       gl_indexiv                          },
{ "glIndexs",                        gl_indexs                           },
{ "glIndexsv",                       gl_indexsv                          },
{ "glIndexub",                       gl_indexub                          },
{ "glIndexubv",                      gl_indexubv                         },
{ "glInitNames",                     gl_initnames                        },
{ "glInsertComponentEXT",            gl_insertcomponentext               },
{ "glInsertEventMarkerEXT",          gl_inserteventmarkerext             },
{ "glInterleavedArrays",             gl_interleavedarrays                },
{ "glInterpolatePathsNV",            gl_interpolatepathsnv               },
{ "glInvalidateBufferData",          gl_invalidatebufferdata             },
{ "glInvalidateBufferSubData",       gl_invalidatebuffersubdata          },
{ "glInvalidateFramebuffer",         gl_invalidateframebuffer            },
{ "glInvalidateNamedFramebufferData",gl_invalidatenamedframebufferdata   },
{ "glInvalidateNamedFramebufferSubData",gl_invalidatenamedframebuffersubdata },
{ "glInvalidateSubFramebuffer",      gl_invalidatesubframebuffer         },
{ "glInvalidateTexImage",            gl_invalidateteximage               },
{ "glInvalidateTexSubImage",         gl_invalidatetexsubimage            },
{ "glIsAsyncMarkerSGIX",             gl_isasyncmarkersgix                },
{ "glIsBuffer",                      gl_isbuffer                         },
{ "glIsBufferARB",                   gl_isbufferarb                      },
{ "glIsBufferResidentNV",            gl_isbufferresidentnv               },
{ "glIsEnabled",                     gl_isenabled                        },
{ "glIsEnabledIndexedEXT",           gl_isenabledindexedext              },
{ "glIsEnabledi",                    gl_isenabledi                       },
{ "glIsFenceAPPLE",                  gl_isfenceapple                     },
{ "glIsFenceNV",                     gl_isfencenv                        },
{ "glIsFramebuffer",                 gl_isframebuffer                    },
{ "glIsFramebufferEXT",              gl_isframebufferext                 },
{ "glIsImageHandleResidentARB",      gl_isimagehandleresidentarb         },
{ "glIsImageHandleResidentNV",       gl_isimagehandleresidentnv          },
{ "glIsList",                        gl_islist                           },
{ "glIsNameAMD",                     gl_isnameamd                        },
{ "glIsNamedBufferResidentNV",       gl_isnamedbufferresidentnv          },
{ "glIsNamedStringARB",              gl_isnamedstringarb                 },
{ "glIsObjectBufferATI",             gl_isobjectbufferati                },
{ "glIsOcclusionQueryNV",            gl_isocclusionquerynv               },
{ "glIsPathNV",                      gl_ispathnv                         },
{ "glIsPointInFillPathNV",           gl_ispointinfillpathnv              },
{ "glIsPointInStrokePathNV",         gl_ispointinstrokepathnv            },
{ "glIsProgram",                     gl_isprogram                        },
{ "glIsProgramARB",                  gl_isprogramarb                     },
{ "glIsProgramNV",                   gl_isprogramnv                      },
{ "glIsProgramPipeline",             gl_isprogrampipeline                },
{ "glIsQuery",                       gl_isquery                          },
{ "glIsQueryARB",                    gl_isqueryarb                       },
{ "glIsRenderbuffer",                gl_isrenderbuffer                   },
{ "glIsRenderbufferEXT",             gl_isrenderbufferext                },
{ "glIsSampler",                     gl_issampler                        },
{ "glIsShader",                      gl_isshader                         },
{ "glIsSync",                        gl_issync                           },
{ "glIsTexture",                     gl_istexture                        },
{ "glIsTextureEXT",                  gl_istextureext                     },
{ "glIsTextureHandleResidentARB",    gl_istexturehandleresidentarb       },
{ "glIsTextureHandleResidentNV",     gl_istexturehandleresidentnv        },
{ "glIsTransformFeedback",           gl_istransformfeedback              },
{ "glIsTransformFeedbackNV",         gl_istransformfeedbacknv            },
{ "glIsVariantEnabledEXT",           gl_isvariantenabledext              },
{ "glIsVertexArray",                 gl_isvertexarray                    },
{ "glIsVertexArrayAPPLE",            gl_isvertexarrayapple               },
{ "glIsVertexAttribEnabledAPPLE",    gl_isvertexattribenabledapple       },
{ "glLabelObjectEXT",                gl_labelobjectext                   },
{ "glLightModelf",                   gl_lightmodelf                      },
{ "glLightModelfv",                  gl_lightmodelfv                     },
{ "glLightModeli",                   gl_lightmodeli                      },
{ "glLightModeliv",                  gl_lightmodeliv                     },
{ "glLightf",                        gl_lightf                           },
{ "glLightfv",                       gl_lightfv                          },
{ "glLighti",                        gl_lighti                           },
{ "glLightiv",                       gl_lightiv                          },
{ "glLineStipple",                   gl_linestipple                      },
{ "glLineWidth",                     gl_linewidth                        },
{ "glLinkProgram",                   gl_linkprogram                      },
{ "glLinkProgramARB",                gl_linkprogramarb                   },
{ "glListBase",                      gl_listbase                         },
{ "glLoadIdentity",                  gl_loadidentity                     },
{ "glLoadMatrixd",                   gl_loadmatrixd                      },
{ "glLoadMatrixf",                   gl_loadmatrixf                      },
{ "glLoadName",                      gl_loadname                         },
{ "glLoadProgramNV",                 gl_loadprogramnv                    },
{ "glLoadTransposeMatrixd",          gl_loadtransposematrixd             },
{ "glLoadTransposeMatrixdARB",       gl_loadtransposematrixdarb          },
{ "glLoadTransposeMatrixf",          gl_loadtransposematrixf             },
{ "glLoadTransposeMatrixfARB",       gl_loadtransposematrixfarb          },
{ "glLockArraysEXT",                 gl_lockarraysext                    },
{ "glLogicOp",                       gl_logicop                          },
{ "glMakeBufferNonResidentNV",       gl_makebuffernonresidentnv          },
{ "glMakeBufferResidentNV",          gl_makebufferresidentnv             },
{ "glMakeImageHandleNonResidentARB", gl_makeimagehandlenonresidentarb    },
{ "glMakeImageHandleNonResidentNV",  gl_makeimagehandlenonresidentnv     },
{ "glMakeImageHandleResidentARB",    gl_makeimagehandleresidentarb       },
{ "glMakeImageHandleResidentNV",     gl_makeimagehandleresidentnv        },
{ "glMakeNamedBufferNonResidentNV",  gl_makenamedbuffernonresidentnv     },
{ "glMakeNamedBufferResidentNV",     gl_makenamedbufferresidentnv        },
{ "glMakeTextureHandleNonResidentARB",gl_maketexturehandlenonresidentarb  },
{ "glMakeTextureHandleNonResidentNV",gl_maketexturehandlenonresidentnv   },
{ "glMakeTextureHandleResidentARB",  gl_maketexturehandleresidentarb     },
{ "glMakeTextureHandleResidentNV",   gl_maketexturehandleresidentnv      },
{ "glMap1d",                         gl_map1d                            },
{ "glMap1f",                         gl_map1f                            },
{ "glMap2d",                         gl_map2d                            },
{ "glMap2f",                         gl_map2f                            },
{ "glMapBuffer",                     gl_mapbuffer                        },
{ "glMapBufferARB",                  gl_mapbufferarb                     },
{ "glMapBufferRange",                gl_mapbufferrange                   },
{ "glMapControlPointsNV",            gl_mapcontrolpointsnv               },
{ "glMapGrid1d",                     gl_mapgrid1d                        },
{ "glMapGrid1f",                     gl_mapgrid1f                        },
{ "glMapGrid2d",                     gl_mapgrid2d                        },
{ "glMapGrid2f",                     gl_mapgrid2f                        },
{ "glMapNamedBuffer",                gl_mapnamedbuffer                   },
{ "glMapNamedBufferEXT",             gl_mapnamedbufferext                },
{ "glMapNamedBufferRange",           gl_mapnamedbufferrange              },
{ "glMapNamedBufferRangeEXT",        gl_mapnamedbufferrangeext           },
{ "glMapObjectBufferATI",            gl_mapobjectbufferati               },
{ "glMapParameterfvNV",              gl_mapparameterfvnv                 },
{ "glMapParameterivNV",              gl_mapparameterivnv                 },
{ "glMapTexture2DINTEL",             gl_maptexture2dintel                },
{ "glMapVertexAttrib1dAPPLE",        gl_mapvertexattrib1dapple           },
{ "glMapVertexAttrib1fAPPLE",        gl_mapvertexattrib1fapple           },
{ "glMapVertexAttrib2dAPPLE",        gl_mapvertexattrib2dapple           },
{ "glMapVertexAttrib2fAPPLE",        gl_mapvertexattrib2fapple           },
{ "glMaterialf",                     gl_materialf                        },
{ "glMaterialfv",                    gl_materialfv                       },
{ "glMateriali",                     gl_materiali                        },
{ "glMaterialiv",                    gl_materialiv                       },
{ "glMatrixFrustumEXT",              gl_matrixfrustumext                 },
{ "glMatrixIndexPointerARB",         gl_matrixindexpointerarb            },
{ "glMatrixIndexubvARB",             gl_matrixindexubvarb                },
{ "glMatrixIndexuivARB",             gl_matrixindexuivarb                },
{ "glMatrixIndexusvARB",             gl_matrixindexusvarb                },
{ "glMatrixLoadIdentityEXT",         gl_matrixloadidentityext            },
{ "glMatrixLoadTransposedEXT",       gl_matrixloadtransposedext          },
{ "glMatrixLoadTransposefEXT",       gl_matrixloadtransposefext          },
{ "glMatrixLoaddEXT",                gl_matrixloaddext                   },
{ "glMatrixLoadfEXT",                gl_matrixloadfext                   },
{ "glMatrixMode",                    gl_matrixmode                       },
{ "glMatrixMultTransposedEXT",       gl_matrixmulttransposedext          },
{ "glMatrixMultTransposefEXT",       gl_matrixmulttransposefext          },
{ "glMatrixMultdEXT",                gl_matrixmultdext                   },
{ "glMatrixMultfEXT",                gl_matrixmultfext                   },
{ "glMatrixOrthoEXT",                gl_matrixorthoext                   },
{ "glMatrixPopEXT",                  gl_matrixpopext                     },
{ "glMatrixPushEXT",                 gl_matrixpushext                    },
{ "glMatrixRotatedEXT",              gl_matrixrotatedext                 },
{ "glMatrixRotatefEXT",              gl_matrixrotatefext                 },
{ "glMatrixScaledEXT",               gl_matrixscaledext                  },
{ "glMatrixScalefEXT",               gl_matrixscalefext                  },
{ "glMatrixTranslatedEXT",           gl_matrixtranslatedext              },
{ "glMatrixTranslatefEXT",           gl_matrixtranslatefext              },
{ "glMemoryBarrier",                 gl_memorybarrier                    },
{ "glMemoryBarrierByRegion",         gl_memorybarrierbyregion            },
{ "glMemoryBarrierEXT",              gl_memorybarrierext                 },
{ "glMinSampleShading",              gl_minsampleshading                 },
{ "glMinSampleShadingARB",           gl_minsampleshadingarb              },
{ "glMinmax",                        gl_minmax                           },
{ "glMinmaxEXT",                     gl_minmaxext                        },
{ "glMultMatrixd",                   gl_multmatrixd                      },
{ "glMultMatrixf",                   gl_multmatrixf                      },
{ "glMultTransposeMatrixd",          gl_multtransposematrixd             },
{ "glMultTransposeMatrixdARB",       gl_multtransposematrixdarb          },
{ "glMultTransposeMatrixf",          gl_multtransposematrixf             },
{ "glMultTransposeMatrixfARB",       gl_multtransposematrixfarb          },
{ "glMultiDrawArrays",               gl_multidrawarrays                  },
{ "glMultiDrawArraysEXT",            gl_multidrawarraysext               },
{ "glMultiDrawArraysIndirect",       gl_multidrawarraysindirect          },
{ "glMultiDrawArraysIndirectAMD",    gl_multidrawarraysindirectamd       },
{ "glMultiDrawArraysIndirectBindlessCountNV",gl_multidrawarraysindirectbindlesscountnv },
{ "glMultiDrawArraysIndirectBindlessNV",gl_multidrawarraysindirectbindlessnv },
{ "glMultiDrawArraysIndirectCountARB",gl_multidrawarraysindirectcountarb  },
{ "glMultiDrawElementArrayAPPLE",    gl_multidrawelementarrayapple       },
{ "glMultiDrawElementsEXT",          gl_multidrawelementsext             },
{ "glMultiDrawElementsIndirect",     gl_multidrawelementsindirect        },
{ "glMultiDrawElementsIndirectAMD",  gl_multidrawelementsindirectamd     },
{ "glMultiDrawElementsIndirectBindlessCountNV",gl_multidrawelementsindirectbindlesscountnv },
{ "glMultiDrawElementsIndirectBindlessNV",gl_multidrawelementsindirectbindlessnv },
{ "glMultiDrawElementsIndirectCountARB",gl_multidrawelementsindirectcountarb },
{ "glMultiDrawRangeElementArrayAPPLE",gl_multidrawrangeelementarrayapple  },
{ "glMultiModeDrawArraysIBM",        gl_multimodedrawarraysibm           },
{ "glMultiModeDrawElementsIBM",      gl_multimodedrawelementsibm         },
{ "glMultiTexBufferEXT",             gl_multitexbufferext                },
{ "glMultiTexCoord1d",               gl_multitexcoord1d                  },
{ "glMultiTexCoord1dARB",            gl_multitexcoord1darb               },
{ "glMultiTexCoord1dv",              gl_multitexcoord1dv                 },
{ "glMultiTexCoord1dvARB",           gl_multitexcoord1dvarb              },
{ "glMultiTexCoord1f",               gl_multitexcoord1f                  },
{ "glMultiTexCoord1fARB",            gl_multitexcoord1farb               },
{ "glMultiTexCoord1fv",              gl_multitexcoord1fv                 },
{ "glMultiTexCoord1fvARB",           gl_multitexcoord1fvarb              },
{ "glMultiTexCoord1i",               gl_multitexcoord1i                  },
{ "glMultiTexCoord1iARB",            gl_multitexcoord1iarb               },
{ "glMultiTexCoord1iv",              gl_multitexcoord1iv                 },
{ "glMultiTexCoord1ivARB",           gl_multitexcoord1ivarb              },
{ "glMultiTexCoord1s",               gl_multitexcoord1s                  },
{ "glMultiTexCoord1sARB",            gl_multitexcoord1sarb               },
{ "glMultiTexCoord1sv",              gl_multitexcoord1sv                 },
{ "glMultiTexCoord1svARB",           gl_multitexcoord1svarb              },
{ "glMultiTexCoord2d",               gl_multitexcoord2d                  },
{ "glMultiTexCoord2dARB",            gl_multitexcoord2darb               },
{ "glMultiTexCoord2dv",              gl_multitexcoord2dv                 },
{ "glMultiTexCoord2dvARB",           gl_multitexcoord2dvarb              },
{ "glMultiTexCoord2f",               gl_multitexcoord2f                  },
{ "glMultiTexCoord2fARB",            gl_multitexcoord2farb               },
{ "glMultiTexCoord2fv",              gl_multitexcoord2fv                 },
{ "glMultiTexCoord2fvARB",           gl_multitexcoord2fvarb              },
{ "glMultiTexCoord2i",               gl_multitexcoord2i                  },
{ "glMultiTexCoord2iARB",            gl_multitexcoord2iarb               },
{ "glMultiTexCoord2iv",              gl_multitexcoord2iv                 },
{ "glMultiTexCoord2ivARB",           gl_multitexcoord2ivarb              },
{ "glMultiTexCoord2s",               gl_multitexcoord2s                  },
{ "glMultiTexCoord2sARB",            gl_multitexcoord2sarb               },
{ "glMultiTexCoord2sv",              gl_multitexcoord2sv                 },
{ "glMultiTexCoord2svARB",           gl_multitexcoord2svarb              },
{ "glMultiTexCoord3d",               gl_multitexcoord3d                  },
{ "glMultiTexCoord3dARB",            gl_multitexcoord3darb               },
{ "glMultiTexCoord3dv",              gl_multitexcoord3dv                 },
{ "glMultiTexCoord3dvARB",           gl_multitexcoord3dvarb              },
{ "glMultiTexCoord3f",               gl_multitexcoord3f                  },
{ "glMultiTexCoord3fARB",            gl_multitexcoord3farb               },
{ "glMultiTexCoord3fv",              gl_multitexcoord3fv                 },
{ "glMultiTexCoord3fvARB",           gl_multitexcoord3fvarb              },
{ "glMultiTexCoord3i",               gl_multitexcoord3i                  },
{ "glMultiTexCoord3iARB",            gl_multitexcoord3iarb               },
{ "glMultiTexCoord3iv",              gl_multitexcoord3iv                 },
{ "glMultiTexCoord3ivARB",           gl_multitexcoord3ivarb              },
{ "glMultiTexCoord3s",               gl_multitexcoord3s                  },
{ "glMultiTexCoord3sARB",            gl_multitexcoord3sarb               },
{ "glMultiTexCoord3sv",              gl_multitexcoord3sv                 },
{ "glMultiTexCoord3svARB",           gl_multitexcoord3svarb              },
{ "glMultiTexCoord4d",               gl_multitexcoord4d                  },
{ "glMultiTexCoord4dARB",            gl_multitexcoord4darb               },
{ "glMultiTexCoord4dv",              gl_multitexcoord4dv                 },
{ "glMultiTexCoord4dvARB",           gl_multitexcoord4dvarb              },
{ "glMultiTexCoord4f",               gl_multitexcoord4f                  },
{ "glMultiTexCoord4fARB",            gl_multitexcoord4farb               },
{ "glMultiTexCoord4fv",              gl_multitexcoord4fv                 },
{ "glMultiTexCoord4fvARB",           gl_multitexcoord4fvarb              },
{ "glMultiTexCoord4i",               gl_multitexcoord4i                  },
{ "glMultiTexCoord4iARB",            gl_multitexcoord4iarb               },
{ "glMultiTexCoord4iv",              gl_multitexcoord4iv                 },
{ "glMultiTexCoord4ivARB",           gl_multitexcoord4ivarb              },
{ "glMultiTexCoord4s",               gl_multitexcoord4s                  },
{ "glMultiTexCoord4sARB",            gl_multitexcoord4sarb               },
{ "glMultiTexCoord4sv",              gl_multitexcoord4sv                 },
{ "glMultiTexCoord4svARB",           gl_multitexcoord4svarb              },
{ "glMultiTexCoordP1ui",             gl_multitexcoordp1ui                },
{ "glMultiTexCoordP1uiv",            gl_multitexcoordp1uiv               },
{ "glMultiTexCoordP2ui",             gl_multitexcoordp2ui                },
{ "glMultiTexCoordP2uiv",            gl_multitexcoordp2uiv               },
{ "glMultiTexCoordP3ui",             gl_multitexcoordp3ui                },
{ "glMultiTexCoordP3uiv",            gl_multitexcoordp3uiv               },
{ "glMultiTexCoordP4ui",             gl_multitexcoordp4ui                },
{ "glMultiTexCoordP4uiv",            gl_multitexcoordp4uiv               },
{ "glMultiTexCoordPointerEXT",       gl_multitexcoordpointerext          },
{ "glMultiTexEnvfEXT",               gl_multitexenvfext                  },
{ "glMultiTexEnvfvEXT",              gl_multitexenvfvext                 },
{ "glMultiTexEnviEXT",               gl_multitexenviext                  },
{ "glMultiTexEnvivEXT",              gl_multitexenvivext                 },
{ "glMultiTexGendEXT",               gl_multitexgendext                  },
{ "glMultiTexGendvEXT",              gl_multitexgendvext                 },
{ "glMultiTexGenfEXT",               gl_multitexgenfext                  },
{ "glMultiTexGenfvEXT",              gl_multitexgenfvext                 },
{ "glMultiTexGeniEXT",               gl_multitexgeniext                  },
{ "glMultiTexGenivEXT",              gl_multitexgenivext                 },
{ "glMultiTexImage1DEXT",            gl_multiteximage1dext               },
{ "glMultiTexImage2DEXT",            gl_multiteximage2dext               },
{ "glMultiTexImage3DEXT",            gl_multiteximage3dext               },
{ "glMultiTexParameterIivEXT",       gl_multitexparameteriivext          },
{ "glMultiTexParameterIuivEXT",      gl_multitexparameteriuivext         },
{ "glMultiTexParameterfEXT",         gl_multitexparameterfext            },
{ "glMultiTexParameterfvEXT",        gl_multitexparameterfvext           },
{ "glMultiTexParameteriEXT",         gl_multitexparameteriext            },
{ "glMultiTexParameterivEXT",        gl_multitexparameterivext           },
{ "glMultiTexRenderbufferEXT",       gl_multitexrenderbufferext          },
{ "glMultiTexSubImage1DEXT",         gl_multitexsubimage1dext            },
{ "glMultiTexSubImage2DEXT",         gl_multitexsubimage2dext            },
{ "glMultiTexSubImage3DEXT",         gl_multitexsubimage3dext            },
{ "glNamedBufferData",               gl_namedbufferdata                  },
{ "glNamedBufferDataEXT",            gl_namedbufferdataext               },
{ "glNamedBufferStorage",            gl_namedbufferstorage               },
{ "glNamedBufferStorageEXT",         gl_namedbufferstorageext            },
{ "glNamedBufferSubData",            gl_namedbuffersubdata               },
{ "glNamedBufferSubDataEXT",         gl_namedbuffersubdataext            },
{ "glNamedCopyBufferSubDataEXT",     gl_namedcopybuffersubdataext        },
{ "glNamedFramebufferDrawBuffer",    gl_namedframebufferdrawbuffer       },
{ "glNamedFramebufferDrawBuffers",   gl_namedframebufferdrawbuffers      },
{ "glNamedFramebufferParameteri",    gl_namedframebufferparameteri       },
{ "glNamedFramebufferParameteriEXT", gl_namedframebufferparameteriext    },
{ "glNamedFramebufferReadBuffer",    gl_namedframebufferreadbuffer       },
{ "glNamedFramebufferRenderbuffer",  gl_namedframebufferrenderbuffer     },
{ "glNamedFramebufferRenderbufferEXT",gl_namedframebufferrenderbufferext  },
{ "glNamedFramebufferTexture",       gl_namedframebuffertexture          },
{ "glNamedFramebufferTexture1DEXT",  gl_namedframebuffertexture1dext     },
{ "glNamedFramebufferTexture2DEXT",  gl_namedframebuffertexture2dext     },
{ "glNamedFramebufferTexture3DEXT",  gl_namedframebuffertexture3dext     },
{ "glNamedFramebufferTextureEXT",    gl_namedframebuffertextureext       },
{ "glNamedFramebufferTextureFaceEXT",gl_namedframebuffertexturefaceext   },
{ "glNamedFramebufferTextureLayer",  gl_namedframebuffertexturelayer     },
{ "glNamedFramebufferTextureLayerEXT",gl_namedframebuffertexturelayerext  },
{ "glNamedProgramLocalParameter4dEXT",gl_namedprogramlocalparameter4dext  },
{ "glNamedProgramLocalParameter4dvEXT",gl_namedprogramlocalparameter4dvext },
{ "glNamedProgramLocalParameter4fEXT",gl_namedprogramlocalparameter4fext  },
{ "glNamedProgramLocalParameter4fvEXT",gl_namedprogramlocalparameter4fvext },
{ "glNamedProgramLocalParameterI4iEXT",gl_namedprogramlocalparameteri4iext },
{ "glNamedProgramLocalParameterI4ivEXT",gl_namedprogramlocalparameteri4ivext },
{ "glNamedProgramLocalParameterI4uiEXT",gl_namedprogramlocalparameteri4uiext },
{ "glNamedProgramLocalParameterI4uivEXT",gl_namedprogramlocalparameteri4uivext },
{ "glNamedProgramLocalParameters4fvEXT",gl_namedprogramlocalparameters4fvext },
{ "glNamedProgramLocalParametersI4ivEXT",gl_namedprogramlocalparametersi4ivext },
{ "glNamedProgramLocalParametersI4uivEXT",gl_namedprogramlocalparametersi4uivext },
{ "glNamedProgramStringEXT",         gl_namedprogramstringext            },
{ "glNamedRenderbufferStorage",      gl_namedrenderbufferstorage         },
{ "glNamedRenderbufferStorageEXT",   gl_namedrenderbufferstorageext      },
{ "glNamedRenderbufferStorageMultisample",gl_namedrenderbufferstoragemultisample },
{ "glNamedRenderbufferStorageMultisampleCoverageEXT",gl_namedrenderbufferstoragemultisamplecoverageext },
{ "glNamedRenderbufferStorageMultisampleEXT",gl_namedrenderbufferstoragemultisampleext },
{ "glNamedStringARB",                gl_namedstringarb                   },
{ "glNewList",                       gl_newlist                          },
{ "glNewObjectBufferATI",            gl_newobjectbufferati               },
{ "glNormal3b",                      gl_normal3b                         },
{ "glNormal3bv",                     gl_normal3bv                        },
{ "glNormal3d",                      gl_normal3d                         },
{ "glNormal3dv",                     gl_normal3dv                        },
{ "glNormal3f",                      gl_normal3f                         },
{ "glNormal3fVertex3fSUN",           gl_normal3fvertex3fsun              },
{ "glNormal3fVertex3fvSUN",          gl_normal3fvertex3fvsun             },
{ "glNormal3fv",                     gl_normal3fv                        },
{ "glNormal3i",                      gl_normal3i                         },
{ "glNormal3iv",                     gl_normal3iv                        },
{ "glNormal3s",                      gl_normal3s                         },
{ "glNormal3sv",                     gl_normal3sv                        },
{ "glNormalFormatNV",                gl_normalformatnv                   },
{ "glNormalP3ui",                    gl_normalp3ui                       },
{ "glNormalP3uiv",                   gl_normalp3uiv                      },
{ "glNormalPointerEXT",              gl_normalpointerext                 },
{ "glNormalStream3bATI",             gl_normalstream3bati                },
{ "glNormalStream3bvATI",            gl_normalstream3bvati               },
{ "glNormalStream3dATI",             gl_normalstream3dati                },
{ "glNormalStream3dvATI",            gl_normalstream3dvati               },
{ "glNormalStream3fATI",             gl_normalstream3fati                },
{ "glNormalStream3fvATI",            gl_normalstream3fvati               },
{ "glNormalStream3iATI",             gl_normalstream3iati                },
{ "glNormalStream3ivATI",            gl_normalstream3ivati               },
{ "glNormalStream3sATI",             gl_normalstream3sati                },
{ "glNormalStream3svATI",            gl_normalstream3svati               },
{ "glObjectLabel",                   gl_objectlabel                      },
{ "glObjectPtrLabel",                gl_objectptrlabel                   },
{ "glObjectPurgeableAPPLE",          gl_objectpurgeableapple             },
{ "glObjectUnpurgeableAPPLE",        gl_objectunpurgeableapple           },
{ "glOrtho",                         gl_ortho                            },
{ "glOrthofOES",                     gl_orthofoes                        },
{ "glPNTrianglesfATI",               gl_pntrianglesfati                  },
{ "glPNTrianglesiATI",               gl_pntrianglesiati                  },
{ "glPassTexCoordATI",               gl_passtexcoordati                  },
{ "glPassThrough",                   gl_passthrough                      },
{ "glPatchParameterfv",              gl_patchparameterfv                 },
{ "glPatchParameteri",               gl_patchparameteri                  },
{ "glPathColorGenNV",                gl_pathcolorgennv                   },
{ "glPathCommandsNV",                gl_pathcommandsnv                   },
{ "glPathCoordsNV",                  gl_pathcoordsnv                     },
{ "glPathCoverDepthFuncNV",          gl_pathcoverdepthfuncnv             },
{ "glPathDashArrayNV",               gl_pathdasharraynv                  },
{ "glPathFogGenNV",                  gl_pathfoggennv                     },
{ "glPathGlyphRangeNV",              gl_pathglyphrangenv                 },
{ "glPathGlyphsNV",                  gl_pathglyphsnv                     },
{ "glPathParameterfNV",              gl_pathparameterfnv                 },
{ "glPathParameterfvNV",             gl_pathparameterfvnv                },
{ "glPathParameteriNV",              gl_pathparameterinv                 },
{ "glPathParameterivNV",             gl_pathparameterivnv                },
{ "glPathStencilDepthOffsetNV",      gl_pathstencildepthoffsetnv         },
{ "glPathStencilFuncNV",             gl_pathstencilfuncnv                },
{ "glPathStringNV",                  gl_pathstringnv                     },
{ "glPathSubCommandsNV",             gl_pathsubcommandsnv                },
{ "glPathSubCoordsNV",               gl_pathsubcoordsnv                  },
{ "glPathTexGenNV",                  gl_pathtexgennv                     },
{ "glPauseTransformFeedback",        gl_pausetransformfeedback           },
{ "glPauseTransformFeedbackNV",      gl_pausetransformfeedbacknv         },
{ "glPixelDataRangeNV",              gl_pixeldatarangenv                 },
{ "glPixelMapfv",                    gl_pixelmapfv                       },
{ "glPixelMapuiv",                   gl_pixelmapuiv                      },
{ "glPixelMapusv",                   gl_pixelmapusv                      },
{ "glPixelStoref",                   gl_pixelstoref                      },
{ "glPixelStorei",                   gl_pixelstorei                      },
{ "glPixelTexGenSGIX",               gl_pixeltexgensgix                  },
{ "glPixelTransferf",                gl_pixeltransferf                   },
{ "glPixelTransferi",                gl_pixeltransferi                   },
{ "glPixelTransformParameterfEXT",   gl_pixeltransformparameterfext      },
{ "glPixelTransformParameterfvEXT",  gl_pixeltransformparameterfvext     },
{ "glPixelTransformParameteriEXT",   gl_pixeltransformparameteriext      },
{ "glPixelTransformParameterivEXT",  gl_pixeltransformparameterivext     },
{ "glPixelZoom",                     gl_pixelzoom                        },
{ "glPointAlongPathNV",              gl_pointalongpathnv                 },
{ "glPointParameterf",               gl_pointparameterf                  },
{ "glPointParameterfARB",            gl_pointparameterfarb               },
{ "glPointParameterfEXT",            gl_pointparameterfext               },
{ "glPointParameterfv",              gl_pointparameterfv                 },
{ "glPointParameterfvARB",           gl_pointparameterfvarb              },
{ "glPointParameterfvEXT",           gl_pointparameterfvext              },
{ "glPointParameteri",               gl_pointparameteri                  },
{ "glPointParameteriNV",             gl_pointparameterinv                },
{ "glPointParameteriv",              gl_pointparameteriv                 },
{ "glPointParameterivNV",            gl_pointparameterivnv               },
{ "glPointSize",                     gl_pointsize                        },
{ "glPollAsyncSGIX",                 gl_pollasyncsgix                    },
{ "glPolygonMode",                   gl_polygonmode                      },
{ "glPolygonOffset",                 gl_polygonoffset                    },
{ "glPolygonOffsetEXT",              gl_polygonoffsetext                 },
{ "glPolygonStipple",                gl_polygonstipple                   },
{ "glPopAttrib",                     gl_popattrib                        },
{ "glPopClientAttrib",               gl_popclientattrib                  },
{ "glPopDebugGroup",                 gl_popdebuggroup                    },
{ "glPopGroupMarkerEXT",             gl_popgroupmarkerext                },
{ "glPopMatrix",                     gl_popmatrix                        },
{ "glPopName",                       gl_popname                          },
{ "glPresentFrameDualFillNV",        gl_presentframedualfillnv           },
{ "glPresentFrameKeyedNV",           gl_presentframekeyednv              },
{ "glPrimitiveRestartIndex",         gl_primitiverestartindex            },
{ "glPrimitiveRestartIndexNV",       gl_primitiverestartindexnv          },
{ "glPrimitiveRestartNV",            gl_primitiverestartnv               },
{ "glPrioritizeTextures",            gl_prioritizetextures               },
{ "glPrioritizeTexturesEXT",         gl_prioritizetexturesext            },
{ "glProgramBinary",                 gl_programbinary                    },
{ "glProgramBufferParametersIivNV",  gl_programbufferparametersiivnv     },
{ "glProgramBufferParametersIuivNV", gl_programbufferparametersiuivnv    },
{ "glProgramBufferParametersfvNV",   gl_programbufferparametersfvnv      },
{ "glProgramEnvParameter4dARB",      gl_programenvparameter4darb         },
{ "glProgramEnvParameter4dvARB",     gl_programenvparameter4dvarb        },
{ "glProgramEnvParameter4fARB",      gl_programenvparameter4farb         },
{ "glProgramEnvParameter4fvARB",     gl_programenvparameter4fvarb        },
{ "glProgramEnvParameterI4iNV",      gl_programenvparameteri4inv         },
{ "glProgramEnvParameterI4ivNV",     gl_programenvparameteri4ivnv        },
{ "glProgramEnvParameterI4uiNV",     gl_programenvparameteri4uinv        },
{ "glProgramEnvParameterI4uivNV",    gl_programenvparameteri4uivnv       },
{ "glProgramEnvParameters4fvEXT",    gl_programenvparameters4fvext       },
{ "glProgramEnvParametersI4ivNV",    gl_programenvparametersi4ivnv       },
{ "glProgramEnvParametersI4uivNV",   gl_programenvparametersi4uivnv      },
{ "glProgramLocalParameter4dARB",    gl_programlocalparameter4darb       },
{ "glProgramLocalParameter4dvARB",   gl_programlocalparameter4dvarb      },
{ "glProgramLocalParameter4fARB",    gl_programlocalparameter4farb       },
{ "glProgramLocalParameter4fvARB",   gl_programlocalparameter4fvarb      },
{ "glProgramLocalParameterI4iNV",    gl_programlocalparameteri4inv       },
{ "glProgramLocalParameterI4ivNV",   gl_programlocalparameteri4ivnv      },
{ "glProgramLocalParameterI4uiNV",   gl_programlocalparameteri4uinv      },
{ "glProgramLocalParameterI4uivNV",  gl_programlocalparameteri4uivnv     },
{ "glProgramLocalParameters4fvEXT",  gl_programlocalparameters4fvext     },
{ "glProgramLocalParametersI4ivNV",  gl_programlocalparametersi4ivnv     },
{ "glProgramLocalParametersI4uivNV", gl_programlocalparametersi4uivnv    },
{ "glProgramNamedParameter4dNV",     gl_programnamedparameter4dnv        },
{ "glProgramNamedParameter4dvNV",    gl_programnamedparameter4dvnv       },
{ "glProgramNamedParameter4fNV",     gl_programnamedparameter4fnv        },
{ "glProgramNamedParameter4fvNV",    gl_programnamedparameter4fvnv       },
{ "glProgramParameter4dNV",          gl_programparameter4dnv             },
{ "glProgramParameter4dvNV",         gl_programparameter4dvnv            },
{ "glProgramParameter4fNV",          gl_programparameter4fnv             },
{ "glProgramParameter4fvNV",         gl_programparameter4fvnv            },
{ "glProgramParameteri",             gl_programparameteri                },
{ "glProgramParameteriARB",          gl_programparameteriarb             },
{ "glProgramParameteriEXT",          gl_programparameteriext             },
{ "glProgramParameters4dvNV",        gl_programparameters4dvnv           },
{ "glProgramParameters4fvNV",        gl_programparameters4fvnv           },
{ "glProgramStringARB",              gl_programstringarb                 },
{ "glProgramUniform1d",              gl_programuniform1d                 },
{ "glProgramUniform1dv",             gl_programuniform1dv                },
{ "glProgramUniform1f",              gl_programuniform1f                 },
{ "glProgramUniform1fEXT",           gl_programuniform1fext              },
{ "glProgramUniform1fv",             gl_programuniform1fv                },
{ "glProgramUniform1fvEXT",          gl_programuniform1fvext             },
{ "glProgramUniform1i",              gl_programuniform1i                 },
{ "glProgramUniform1i64NV",          gl_programuniform1i64nv             },
{ "glProgramUniform1i64vNV",         gl_programuniform1i64vnv            },
{ "glProgramUniform1iEXT",           gl_programuniform1iext              },
{ "glProgramUniform1iv",             gl_programuniform1iv                },
{ "glProgramUniform1ivEXT",          gl_programuniform1ivext             },
{ "glProgramUniform1ui",             gl_programuniform1ui                },
{ "glProgramUniform1ui64NV",         gl_programuniform1ui64nv            },
{ "glProgramUniform1ui64vNV",        gl_programuniform1ui64vnv           },
{ "glProgramUniform1uiEXT",          gl_programuniform1uiext             },
{ "glProgramUniform1uiv",            gl_programuniform1uiv               },
{ "glProgramUniform1uivEXT",         gl_programuniform1uivext            },
{ "glProgramUniform2d",              gl_programuniform2d                 },
{ "glProgramUniform2dv",             gl_programuniform2dv                },
{ "glProgramUniform2f",              gl_programuniform2f                 },
{ "glProgramUniform2fEXT",           gl_programuniform2fext              },
{ "glProgramUniform2fv",             gl_programuniform2fv                },
{ "glProgramUniform2fvEXT",          gl_programuniform2fvext             },
{ "glProgramUniform2i",              gl_programuniform2i                 },
{ "glProgramUniform2i64NV",          gl_programuniform2i64nv             },
{ "glProgramUniform2i64vNV",         gl_programuniform2i64vnv            },
{ "glProgramUniform2iEXT",           gl_programuniform2iext              },
{ "glProgramUniform2iv",             gl_programuniform2iv                },
{ "glProgramUniform2ivEXT",          gl_programuniform2ivext             },
{ "glProgramUniform2ui",             gl_programuniform2ui                },
{ "glProgramUniform2ui64NV",         gl_programuniform2ui64nv            },
{ "glProgramUniform2ui64vNV",        gl_programuniform2ui64vnv           },
{ "glProgramUniform2uiEXT",          gl_programuniform2uiext             },
{ "glProgramUniform2uiv",            gl_programuniform2uiv               },
{ "glProgramUniform2uivEXT",         gl_programuniform2uivext            },
{ "glProgramUniform3d",              gl_programuniform3d                 },
{ "glProgramUniform3dv",             gl_programuniform3dv                },
{ "glProgramUniform3f",              gl_programuniform3f                 },
{ "glProgramUniform3fEXT",           gl_programuniform3fext              },
{ "glProgramUniform3fv",             gl_programuniform3fv                },
{ "glProgramUniform3fvEXT",          gl_programuniform3fvext             },
{ "glProgramUniform3i",              gl_programuniform3i                 },
{ "glProgramUniform3i64NV",          gl_programuniform3i64nv             },
{ "glProgramUniform3i64vNV",         gl_programuniform3i64vnv            },
{ "glProgramUniform3iEXT",           gl_programuniform3iext              },
{ "glProgramUniform3iv",             gl_programuniform3iv                },
{ "glProgramUniform3ivEXT",          gl_programuniform3ivext             },
{ "glProgramUniform3ui",             gl_programuniform3ui                },
{ "glProgramUniform3ui64NV",         gl_programuniform3ui64nv            },
{ "glProgramUniform3ui64vNV",        gl_programuniform3ui64vnv           },
{ "glProgramUniform3uiEXT",          gl_programuniform3uiext             },
{ "glProgramUniform3uiv",            gl_programuniform3uiv               },
{ "glProgramUniform3uivEXT",         gl_programuniform3uivext            },
{ "glProgramUniform4d",              gl_programuniform4d                 },
{ "glProgramUniform4dv",             gl_programuniform4dv                },
{ "glProgramUniform4f",              gl_programuniform4f                 },
{ "glProgramUniform4fEXT",           gl_programuniform4fext              },
{ "glProgramUniform4fv",             gl_programuniform4fv                },
{ "glProgramUniform4fvEXT",          gl_programuniform4fvext             },
{ "glProgramUniform4i",              gl_programuniform4i                 },
{ "glProgramUniform4i64NV",          gl_programuniform4i64nv             },
{ "glProgramUniform4i64vNV",         gl_programuniform4i64vnv            },
{ "glProgramUniform4iEXT",           gl_programuniform4iext              },
{ "glProgramUniform4iv",             gl_programuniform4iv                },
{ "glProgramUniform4ivEXT",          gl_programuniform4ivext             },
{ "glProgramUniform4ui",             gl_programuniform4ui                },
{ "glProgramUniform4ui64NV",         gl_programuniform4ui64nv            },
{ "glProgramUniform4ui64vNV",        gl_programuniform4ui64vnv           },
{ "glProgramUniform4uiEXT",          gl_programuniform4uiext             },
{ "glProgramUniform4uiv",            gl_programuniform4uiv               },
{ "glProgramUniform4uivEXT",         gl_programuniform4uivext            },
{ "glProgramUniformHandleui64ARB",   gl_programuniformhandleui64arb      },
{ "glProgramUniformHandleui64NV",    gl_programuniformhandleui64nv       },
{ "glProgramUniformHandleui64vARB",  gl_programuniformhandleui64varb     },
{ "glProgramUniformHandleui64vNV",   gl_programuniformhandleui64vnv      },
{ "glProgramUniformMatrix2dv",       gl_programuniformmatrix2dv          },
{ "glProgramUniformMatrix2fv",       gl_programuniformmatrix2fv          },
{ "glProgramUniformMatrix2fvEXT",    gl_programuniformmatrix2fvext       },
{ "glProgramUniformMatrix2x3dv",     gl_programuniformmatrix2x3dv        },
{ "glProgramUniformMatrix2x3fv",     gl_programuniformmatrix2x3fv        },
{ "glProgramUniformMatrix2x3fvEXT",  gl_programuniformmatrix2x3fvext     },
{ "glProgramUniformMatrix2x4dv",     gl_programuniformmatrix2x4dv        },
{ "glProgramUniformMatrix2x4fv",     gl_programuniformmatrix2x4fv        },
{ "glProgramUniformMatrix2x4fvEXT",  gl_programuniformmatrix2x4fvext     },
{ "glProgramUniformMatrix3dv",       gl_programuniformmatrix3dv          },
{ "glProgramUniformMatrix3fv",       gl_programuniformmatrix3fv          },
{ "glProgramUniformMatrix3fvEXT",    gl_programuniformmatrix3fvext       },
{ "glProgramUniformMatrix3x2dv",     gl_programuniformmatrix3x2dv        },
{ "glProgramUniformMatrix3x2fv",     gl_programuniformmatrix3x2fv        },
{ "glProgramUniformMatrix3x2fvEXT",  gl_programuniformmatrix3x2fvext     },
{ "glProgramUniformMatrix3x4dv",     gl_programuniformmatrix3x4dv        },
{ "glProgramUniformMatrix3x4fv",     gl_programuniformmatrix3x4fv        },
{ "glProgramUniformMatrix3x4fvEXT",  gl_programuniformmatrix3x4fvext     },
{ "glProgramUniformMatrix4dv",       gl_programuniformmatrix4dv          },
{ "glProgramUniformMatrix4fv",       gl_programuniformmatrix4fv          },
{ "glProgramUniformMatrix4fvEXT",    gl_programuniformmatrix4fvext       },
{ "glProgramUniformMatrix4x2dv",     gl_programuniformmatrix4x2dv        },
{ "glProgramUniformMatrix4x2fv",     gl_programuniformmatrix4x2fv        },
{ "glProgramUniformMatrix4x2fvEXT",  gl_programuniformmatrix4x2fvext     },
{ "glProgramUniformMatrix4x3dv",     gl_programuniformmatrix4x3dv        },
{ "glProgramUniformMatrix4x3fv",     gl_programuniformmatrix4x3fv        },
{ "glProgramUniformMatrix4x3fvEXT",  gl_programuniformmatrix4x3fvext     },
{ "glProgramUniformui64NV",          gl_programuniformui64nv             },
{ "glProgramUniformui64vNV",         gl_programuniformui64vnv            },
{ "glProgramVertexLimitNV",          gl_programvertexlimitnv             },
{ "glProvokingVertex",               gl_provokingvertex                  },
{ "glProvokingVertexEXT",            gl_provokingvertexext               },
{ "glPushAttrib",                    gl_pushattrib                       },
{ "glPushClientAttrib",              gl_pushclientattrib                 },
{ "glPushClientAttribDefaultEXT",    gl_pushclientattribdefaultext       },
{ "glPushDebugGroup",                gl_pushdebuggroup                   },
{ "glPushGroupMarkerEXT",            gl_pushgroupmarkerext               },
{ "glPushMatrix",                    gl_pushmatrix                       },
{ "glPushName",                      gl_pushname                         },
{ "glQueryCounter",                  gl_querycounter                     },
{ "glQueryObjectParameteruiAMD",     gl_queryobjectparameteruiamd        },
{ "glRasterPos2d",                   gl_rasterpos2d                      },
{ "glRasterPos2dv",                  gl_rasterpos2dv                     },
{ "glRasterPos2f",                   gl_rasterpos2f                      },
{ "glRasterPos2fv",                  gl_rasterpos2fv                     },
{ "glRasterPos2i",                   gl_rasterpos2i                      },
{ "glRasterPos2iv",                  gl_rasterpos2iv                     },
{ "glRasterPos2s",                   gl_rasterpos2s                      },
{ "glRasterPos2sv",                  gl_rasterpos2sv                     },
{ "glRasterPos3d",                   gl_rasterpos3d                      },
{ "glRasterPos3dv",                  gl_rasterpos3dv                     },
{ "glRasterPos3f",                   gl_rasterpos3f                      },
{ "glRasterPos3fv",                  gl_rasterpos3fv                     },
{ "glRasterPos3i",                   gl_rasterpos3i                      },
{ "glRasterPos3iv",                  gl_rasterpos3iv                     },
{ "glRasterPos3s",                   gl_rasterpos3s                      },
{ "glRasterPos3sv",                  gl_rasterpos3sv                     },
{ "glRasterPos4d",                   gl_rasterpos4d                      },
{ "glRasterPos4dv",                  gl_rasterpos4dv                     },
{ "glRasterPos4f",                   gl_rasterpos4f                      },
{ "glRasterPos4fv",                  gl_rasterpos4fv                     },
{ "glRasterPos4i",                   gl_rasterpos4i                      },
{ "glRasterPos4iv",                  gl_rasterpos4iv                     },
{ "glRasterPos4s",                   gl_rasterpos4s                      },
{ "glRasterPos4sv",                  gl_rasterpos4sv                     },
{ "glReadBuffer",                    gl_readbuffer                       },
{ "glReadnPixels",                   gl_readnpixels                      },
{ "glReadnPixelsARB",                gl_readnpixelsarb                   },
{ "glRectd",                         gl_rectd                            },
{ "glRectdv",                        gl_rectdv                           },
{ "glRectf",                         gl_rectf                            },
{ "glRectfv",                        gl_rectfv                           },
{ "glRecti",                         gl_recti                            },
{ "glRectiv",                        gl_rectiv                           },
{ "glRects",                         gl_rects                            },
{ "glRectsv",                        gl_rectsv                           },
{ "glReferencePlaneSGIX",            gl_referenceplanesgix               },
{ "glReleaseShaderCompiler",         gl_releaseshadercompiler            },
{ "glRenderMode",                    gl_rendermode                       },
{ "glRenderbufferStorage",           gl_renderbufferstorage              },
{ "glRenderbufferStorageEXT",        gl_renderbufferstorageext           },
{ "glRenderbufferStorageMultisample",gl_renderbufferstoragemultisample   },
{ "glRenderbufferStorageMultisampleCoverageNV",gl_renderbufferstoragemultisamplecoveragenv },
{ "glRenderbufferStorageMultisampleEXT",gl_renderbufferstoragemultisampleext },
{ "glReplacementCodeubSUN",          gl_replacementcodeubsun             },
{ "glReplacementCodeubvSUN",         gl_replacementcodeubvsun            },
{ "glReplacementCodeuiColor3fVertex3fSUN",gl_replacementcodeuicolor3fvertex3fsun },
{ "glReplacementCodeuiColor3fVertex3fvSUN",gl_replacementcodeuicolor3fvertex3fvsun },
{ "glReplacementCodeuiColor4fNormal3fVertex3fSUN",gl_replacementcodeuicolor4fnormal3fvertex3fsun },
{ "glReplacementCodeuiColor4fNormal3fVertex3fvSUN",gl_replacementcodeuicolor4fnormal3fvertex3fvsun },
{ "glReplacementCodeuiColor4ubVertex3fSUN",gl_replacementcodeuicolor4ubvertex3fsun },
{ "glReplacementCodeuiColor4ubVertex3fvSUN",gl_replacementcodeuicolor4ubvertex3fvsun },
{ "glReplacementCodeuiNormal3fVertex3fSUN",gl_replacementcodeuinormal3fvertex3fsun },
{ "glReplacementCodeuiNormal3fVertex3fvSUN",gl_replacementcodeuinormal3fvertex3fvsun },
{ "glReplacementCodeuiSUN",          gl_replacementcodeuisun             },
{ "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fSUN",gl_replacementcodeuitexcoord2fcolor4fnormal3fvertex3fsun },
{ "glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN",gl_replacementcodeuitexcoord2fcolor4fnormal3fvertex3fvsun },
{ "glReplacementCodeuiTexCoord2fNormal3fVertex3fSUN",gl_replacementcodeuitexcoord2fnormal3fvertex3fsun },
{ "glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN",gl_replacementcodeuitexcoord2fnormal3fvertex3fvsun },
{ "glReplacementCodeuiTexCoord2fVertex3fSUN",gl_replacementcodeuitexcoord2fvertex3fsun },
{ "glReplacementCodeuiTexCoord2fVertex3fvSUN",gl_replacementcodeuitexcoord2fvertex3fvsun },
{ "glReplacementCodeuiVertex3fSUN",  gl_replacementcodeuivertex3fsun     },
{ "glReplacementCodeuiVertex3fvSUN", gl_replacementcodeuivertex3fvsun    },
{ "glReplacementCodeuivSUN",         gl_replacementcodeuivsun            },
{ "glReplacementCodeusSUN",          gl_replacementcodeussun             },
{ "glReplacementCodeusvSUN",         gl_replacementcodeusvsun            },
{ "glRequestResidentProgramsNV",     gl_requestresidentprogramsnv        },
{ "glResetHistogram",                gl_resethistogram                   },
{ "glResetHistogramEXT",             gl_resethistogramext                },
{ "glResetMinmax",                   gl_resetminmax                      },
{ "glResetMinmaxEXT",                gl_resetminmaxext                   },
{ "glResizeBuffersMESA",             gl_resizebuffersmesa                },
{ "glResumeTransformFeedback",       gl_resumetransformfeedback          },
{ "glResumeTransformFeedbackNV",     gl_resumetransformfeedbacknv        },
{ "glRotated",                       gl_rotated                          },
{ "glRotatef",                       gl_rotatef                          },
{ "glSampleCoverage",                gl_samplecoverage                   },
{ "glSampleCoverageARB",             gl_samplecoveragearb                },
{ "glSampleMapATI",                  gl_samplemapati                     },
{ "glSampleMaskEXT",                 gl_samplemaskext                    },
{ "glSampleMaskIndexedNV",           gl_samplemaskindexednv              },
{ "glSampleMaskSGIS",                gl_samplemasksgis                   },
{ "glSampleMaski",                   gl_samplemaski                      },
{ "glSamplePatternEXT",              gl_samplepatternext                 },
{ "glSamplePatternSGIS",             gl_samplepatternsgis                },
{ "glSamplerParameterIiv",           gl_samplerparameteriiv              },
{ "glSamplerParameterIuiv",          gl_samplerparameteriuiv             },
{ "glSamplerParameterf",             gl_samplerparameterf                },
{ "glSamplerParameterfv",            gl_samplerparameterfv               },
{ "glSamplerParameteri",             gl_samplerparameteri                },
{ "glSamplerParameteriv",            gl_samplerparameteriv               },
{ "glScaled",                        gl_scaled                           },
{ "glScalef",                        gl_scalef                           },
{ "glScissor",                       gl_scissor                          },
{ "glScissorArrayv",                 gl_scissorarrayv                    },
{ "glScissorIndexed",                gl_scissorindexed                   },
{ "glScissorIndexedv",               gl_scissorindexedv                  },
{ "glSecondaryColor3b",              gl_secondarycolor3b                 },
{ "glSecondaryColor3bEXT",           gl_secondarycolor3bext              },
{ "glSecondaryColor3bv",             gl_secondarycolor3bv                },
{ "glSecondaryColor3bvEXT",          gl_secondarycolor3bvext             },
{ "glSecondaryColor3d",              gl_secondarycolor3d                 },
{ "glSecondaryColor3dEXT",           gl_secondarycolor3dext              },
{ "glSecondaryColor3dv",             gl_secondarycolor3dv                },
{ "glSecondaryColor3dvEXT",          gl_secondarycolor3dvext             },
{ "glSecondaryColor3f",              gl_secondarycolor3f                 },
{ "glSecondaryColor3fEXT",           gl_secondarycolor3fext              },
{ "glSecondaryColor3fv",             gl_secondarycolor3fv                },
{ "glSecondaryColor3fvEXT",          gl_secondarycolor3fvext             },
{ "glSecondaryColor3i",              gl_secondarycolor3i                 },
{ "glSecondaryColor3iEXT",           gl_secondarycolor3iext              },
{ "glSecondaryColor3iv",             gl_secondarycolor3iv                },
{ "glSecondaryColor3ivEXT",          gl_secondarycolor3ivext             },
{ "glSecondaryColor3s",              gl_secondarycolor3s                 },
{ "glSecondaryColor3sEXT",           gl_secondarycolor3sext              },
{ "glSecondaryColor3sv",             gl_secondarycolor3sv                },
{ "glSecondaryColor3svEXT",          gl_secondarycolor3svext             },
{ "glSecondaryColor3ub",             gl_secondarycolor3ub                },
{ "glSecondaryColor3ubEXT",          gl_secondarycolor3ubext             },
{ "glSecondaryColor3ubv",            gl_secondarycolor3ubv               },
{ "glSecondaryColor3ubvEXT",         gl_secondarycolor3ubvext            },
{ "glSecondaryColor3ui",             gl_secondarycolor3ui                },
{ "glSecondaryColor3uiEXT",          gl_secondarycolor3uiext             },
{ "glSecondaryColor3uiv",            gl_secondarycolor3uiv               },
{ "glSecondaryColor3uivEXT",         gl_secondarycolor3uivext            },
{ "glSecondaryColor3us",             gl_secondarycolor3us                },
{ "glSecondaryColor3usEXT",          gl_secondarycolor3usext             },
{ "glSecondaryColor3usv",            gl_secondarycolor3usv               },
{ "glSecondaryColor3usvEXT",         gl_secondarycolor3usvext            },
{ "glSecondaryColorFormatNV",        gl_secondarycolorformatnv           },
{ "glSecondaryColorP3ui",            gl_secondarycolorp3ui               },
{ "glSecondaryColorP3uiv",           gl_secondarycolorp3uiv              },
{ "glSecondaryColorPointer",         gl_secondarycolorpointer            },
{ "glSecondaryColorPointerEXT",      gl_secondarycolorpointerext         },
{ "glSelectPerfMonitorCountersAMD",  gl_selectperfmonitorcountersamd     },
{ "glSeparableFilter2D",             gl_separablefilter2d                },
{ "glSeparableFilter2DEXT",          gl_separablefilter2dext             },
{ "glSetFenceAPPLE",                 gl_setfenceapple                    },
{ "glSetFenceNV",                    gl_setfencenv                       },
{ "glSetFragmentShaderConstantATI",  gl_setfragmentshaderconstantati     },
{ "glSetInvariantEXT",               gl_setinvariantext                  },
{ "glSetLocalConstantEXT",           gl_setlocalconstantext              },
{ "glSetMultisamplefvAMD",           gl_setmultisamplefvamd              },
{ "glShadeModel",                    gl_shademodel                       },
{ "glShaderBinary",                  gl_shaderbinary                     },
{ "glShaderOp1EXT",                  gl_shaderop1ext                     },
{ "glShaderOp2EXT",                  gl_shaderop2ext                     },
{ "glShaderOp3EXT",                  gl_shaderop3ext                     },
{ "glShaderStorageBlockBinding",     gl_shaderstorageblockbinding        },
{ "glSharpenTexFuncSGIS",            gl_sharpentexfuncsgis               },
{ "glSpriteParameterfSGIX",          gl_spriteparameterfsgix             },
{ "glSpriteParameterfvSGIX",         gl_spriteparameterfvsgix            },
{ "glSpriteParameteriSGIX",          gl_spriteparameterisgix             },
{ "glSpriteParameterivSGIX",         gl_spriteparameterivsgix            },
{ "glStencilFillPathInstancedNV",    gl_stencilfillpathinstancednv       },
{ "glStencilFillPathNV",             gl_stencilfillpathnv                },
{ "glStencilFunc",                   gl_stencilfunc                      },
{ "glStencilFuncSeparate",           gl_stencilfuncseparate              },
{ "glStencilFuncSeparateATI",        gl_stencilfuncseparateati           },
{ "glStencilMask",                   gl_stencilmask                      },
{ "glStencilMaskSeparate",           gl_stencilmaskseparate              },
{ "glStencilOp",                     gl_stencilop                        },
{ "glStencilOpSeparate",             gl_stencilopseparate                },
{ "glStencilOpSeparateATI",          gl_stencilopseparateati             },
{ "glStencilOpValueAMD",             gl_stencilopvalueamd                },
{ "glStencilStrokePathInstancedNV",  gl_stencilstrokepathinstancednv     },
{ "glStencilStrokePathNV",           gl_stencilstrokepathnv              },
{ "glStringMarkerGREMEDY",           gl_stringmarkergremedy              },
{ "glSwizzleEXT",                    gl_swizzleext                       },
{ "glSyncTextureINTEL",              gl_synctextureintel                 },
{ "glTagSampleBufferSGIX",           gl_tagsamplebuffersgix              },
{ "glTangentPointerEXT",             gl_tangentpointerext                },
{ "glTbufferMask3DFX",               gl_tbuffermask3dfx                  },
{ "glTessellationFactorAMD",         gl_tessellationfactoramd            },
{ "glTessellationModeAMD",           gl_tessellationmodeamd              },
{ "glTestFenceAPPLE",                gl_testfenceapple                   },
{ "glTestFenceNV",                   gl_testfencenv                      },
{ "glTestObjectAPPLE",               gl_testobjectapple                  },
{ "glTexBuffer",                     gl_texbuffer                        },
{ "glTexBufferARB",                  gl_texbufferarb                     },
{ "glTexBufferEXT",                  gl_texbufferext                     },
{ "glTexBufferRange",                gl_texbufferrange                   },
{ "glTexBumpParameterfvATI",         gl_texbumpparameterfvati            },
{ "glTexBumpParameterivATI",         gl_texbumpparameterivati            },
{ "glTexCoord1d",                    gl_texcoord1d                       },
{ "glTexCoord1dv",                   gl_texcoord1dv                      },
{ "glTexCoord1f",                    gl_texcoord1f                       },
{ "glTexCoord1fv",                   gl_texcoord1fv                      },
{ "glTexCoord1i",                    gl_texcoord1i                       },
{ "glTexCoord1iv",                   gl_texcoord1iv                      },
{ "glTexCoord1s",                    gl_texcoord1s                       },
{ "glTexCoord1sv",                   gl_texcoord1sv                      },
{ "glTexCoord2d",                    gl_texcoord2d                       },
{ "glTexCoord2dv",                   gl_texcoord2dv                      },
{ "glTexCoord2f",                    gl_texcoord2f                       },
{ "glTexCoord2fColor3fVertex3fSUN",  gl_texcoord2fcolor3fvertex3fsun     },
{ "glTexCoord2fColor3fVertex3fvSUN", gl_texcoord2fcolor3fvertex3fvsun    },
{ "glTexCoord2fColor4fNormal3fVertex3fSUN",gl_texcoord2fcolor4fnormal3fvertex3fsun },
{ "glTexCoord2fColor4fNormal3fVertex3fvSUN",gl_texcoord2fcolor4fnormal3fvertex3fvsun },
{ "glTexCoord2fColor4ubVertex3fSUN", gl_texcoord2fcolor4ubvertex3fsun    },
{ "glTexCoord2fColor4ubVertex3fvSUN",gl_texcoord2fcolor4ubvertex3fvsun   },
{ "glTexCoord2fNormal3fVertex3fSUN", gl_texcoord2fnormal3fvertex3fsun    },
{ "glTexCoord2fNormal3fVertex3fvSUN",gl_texcoord2fnormal3fvertex3fvsun   },
{ "glTexCoord2fVertex3fSUN",         gl_texcoord2fvertex3fsun            },
{ "glTexCoord2fVertex3fvSUN",        gl_texcoord2fvertex3fvsun           },
{ "glTexCoord2fv",                   gl_texcoord2fv                      },
{ "glTexCoord2i",                    gl_texcoord2i                       },
{ "glTexCoord2iv",                   gl_texcoord2iv                      },
{ "glTexCoord2s",                    gl_texcoord2s                       },
{ "glTexCoord2sv",                   gl_texcoord2sv                      },
{ "glTexCoord3d",                    gl_texcoord3d                       },
{ "glTexCoord3dv",                   gl_texcoord3dv                      },
{ "glTexCoord3f",                    gl_texcoord3f                       },
{ "glTexCoord3fv",                   gl_texcoord3fv                      },
{ "glTexCoord3i",                    gl_texcoord3i                       },
{ "glTexCoord3iv",                   gl_texcoord3iv                      },
{ "glTexCoord3s",                    gl_texcoord3s                       },
{ "glTexCoord3sv",                   gl_texcoord3sv                      },
{ "glTexCoord4d",                    gl_texcoord4d                       },
{ "glTexCoord4dv",                   gl_texcoord4dv                      },
{ "glTexCoord4f",                    gl_texcoord4f                       },
{ "glTexCoord4fColor4fNormal3fVertex4fSUN",gl_texcoord4fcolor4fnormal3fvertex4fsun },
{ "glTexCoord4fColor4fNormal3fVertex4fvSUN",gl_texcoord4fcolor4fnormal3fvertex4fvsun },
{ "glTexCoord4fVertex4fSUN",         gl_texcoord4fvertex4fsun            },
{ "glTexCoord4fVertex4fvSUN",        gl_texcoord4fvertex4fvsun           },
{ "glTexCoord4fv",                   gl_texcoord4fv                      },
{ "glTexCoord4i",                    gl_texcoord4i                       },
{ "glTexCoord4iv",                   gl_texcoord4iv                      },
{ "glTexCoord4s",                    gl_texcoord4s                       },
{ "glTexCoord4sv",                   gl_texcoord4sv                      },
{ "glTexCoordFormatNV",              gl_texcoordformatnv                 },
{ "glTexCoordP1ui",                  gl_texcoordp1ui                     },
{ "glTexCoordP1uiv",                 gl_texcoordp1uiv                    },
{ "glTexCoordP2ui",                  gl_texcoordp2ui                     },
{ "glTexCoordP2uiv",                 gl_texcoordp2uiv                    },
{ "glTexCoordP3ui",                  gl_texcoordp3ui                     },
{ "glTexCoordP3uiv",                 gl_texcoordp3uiv                    },
{ "glTexCoordP4ui",                  gl_texcoordp4ui                     },
{ "glTexCoordP4uiv",                 gl_texcoordp4uiv                    },
{ "glTexCoordPointerEXT",            gl_texcoordpointerext               },
{ "glTexEnvf",                       gl_texenvf                          },
{ "glTexEnvfv",                      gl_texenvfv                         },
{ "glTexEnvi",                       gl_texenvi                          },
{ "glTexEnviv",                      gl_texenviv                         },
{ "glTexFilterFuncSGIS",             gl_texfilterfuncsgis                },
{ "glTexGend",                       gl_texgend                          },
{ "glTexGendv",                      gl_texgendv                         },
{ "glTexGenf",                       gl_texgenf                          },
{ "glTexGenfv",                      gl_texgenfv                         },
{ "glTexGeni",                       gl_texgeni                          },
{ "glTexGeniv",                      gl_texgeniv                         },
{ "glTexImage1D",                    gl_teximage1d                       },
{ "glTexImage2DMultisample",         gl_teximage2dmultisample            },
{ "glTexImage2DMultisampleCoverageNV",gl_teximage2dmultisamplecoveragenv  },
{ "glTexImage3D",                    gl_teximage3d                       },
{ "glTexImage3DEXT",                 gl_teximage3dext                    },
{ "glTexImage3DMultisample",         gl_teximage3dmultisample            },
{ "glTexImage3DMultisampleCoverageNV",gl_teximage3dmultisamplecoveragenv  },
{ "glTexImage4DSGIS",                gl_teximage4dsgis                   },
{ "glTexPageCommitmentARB",          gl_texpagecommitmentarb             },
{ "glTexParameterIiv",               gl_texparameteriiv                  },
{ "glTexParameterIivEXT",            gl_texparameteriivext               },
{ "glTexParameterIuiv",              gl_texparameteriuiv                 },
{ "glTexParameterIuivEXT",           gl_texparameteriuivext              },
{ "glTexParameterf",                 gl_texparameterf                    },
{ "glTexParameterfv",                gl_texparameterfv                   },
{ "glTexParameteri",                 gl_texparameteri                    },
{ "glTexParameteriv",                gl_texparameteriv                   },
{ "glTexRenderbufferNV",             gl_texrenderbuffernv                },
{ "glTexStorage1D",                  gl_texstorage1d                     },
{ "glTexStorage2D",                  gl_texstorage2d                     },
{ "glTexStorage2DMultisample",       gl_texstorage2dmultisample          },
{ "glTexStorage3D",                  gl_texstorage3d                     },
{ "glTexStorage3DMultisample",       gl_texstorage3dmultisample          },
{ "glTexStorageSparseAMD",           gl_texstoragesparseamd              },
{ "glTexSubImage1D",                 gl_texsubimage1d                    },
{ "glTexSubImage1DEXT",              gl_texsubimage1dext                 },
{ "glTexSubImage2D",                 gl_texsubimage2d                    },
{ "glTexSubImage2DEXT",              gl_texsubimage2dext                 },
{ "glTexSubImage3D",                 gl_texsubimage3d                    },
{ "glTexSubImage3DEXT",              gl_texsubimage3dext                 },
{ "glTexSubImage4DSGIS",             gl_texsubimage4dsgis                },
{ "glTextureBarrier",                gl_texturebarrier                   },
{ "glTextureBarrierNV",              gl_texturebarriernv                 },
{ "glTextureBuffer",                 gl_texturebuffer                    },
{ "glTextureBufferEXT",              gl_texturebufferext                 },
{ "glTextureBufferRange",            gl_texturebufferrange               },
{ "glTextureBufferRangeEXT",         gl_texturebufferrangeext            },
{ "glTextureImage1DEXT",             gl_textureimage1dext                },
{ "glTextureImage2DEXT",             gl_textureimage2dext                },
{ "glTextureImage2DMultisampleCoverageNV",gl_textureimage2dmultisamplecoveragenv },
{ "glTextureImage2DMultisampleNV",   gl_textureimage2dmultisamplenv      },
{ "glTextureImage3DEXT",             gl_textureimage3dext                },
{ "glTextureImage3DMultisampleCoverageNV",gl_textureimage3dmultisamplecoveragenv },
{ "glTextureImage3DMultisampleNV",   gl_textureimage3dmultisamplenv      },
{ "glTextureLightEXT",               gl_texturelightext                  },
{ "glTextureMaterialEXT",            gl_texturematerialext               },
{ "glTextureNormalEXT",              gl_texturenormalext                 },
{ "glTexturePageCommitmentEXT",      gl_texturepagecommitmentext         },
{ "glTextureParameterIiv",           gl_textureparameteriiv              },
{ "glTextureParameterIivEXT",        gl_textureparameteriivext           },
{ "glTextureParameterIuiv",          gl_textureparameteriuiv             },
{ "glTextureParameterIuivEXT",       gl_textureparameteriuivext          },
{ "glTextureParameterf",             gl_textureparameterf                },
{ "glTextureParameterfEXT",          gl_textureparameterfext             },
{ "glTextureParameterfv",            gl_textureparameterfv               },
{ "glTextureParameterfvEXT",         gl_textureparameterfvext            },
{ "glTextureParameteri",             gl_textureparameteri                },
{ "glTextureParameteriEXT",          gl_textureparameteriext             },
{ "glTextureParameteriv",            gl_textureparameteriv               },
{ "glTextureParameterivEXT",         gl_textureparameterivext            },
{ "glTextureRangeAPPLE",             gl_texturerangeapple                },
{ "glTextureRenderbufferEXT",        gl_texturerenderbufferext           },
{ "glTextureStorage1D",              gl_texturestorage1d                 },
{ "glTextureStorage1DEXT",           gl_texturestorage1dext              },
{ "glTextureStorage2D",              gl_texturestorage2d                 },
{ "glTextureStorage2DEXT",           gl_texturestorage2dext              },
{ "glTextureStorage2DMultisample",   gl_texturestorage2dmultisample      },
{ "glTextureStorage2DMultisampleEXT",gl_texturestorage2dmultisampleext   },
{ "glTextureStorage3D",              gl_texturestorage3d                 },
{ "glTextureStorage3DEXT",           gl_texturestorage3dext              },
{ "glTextureStorage3DMultisample",   gl_texturestorage3dmultisample      },
{ "glTextureStorage3DMultisampleEXT",gl_texturestorage3dmultisampleext   },
{ "glTextureStorageSparseAMD",       gl_texturestoragesparseamd          },
{ "glTextureSubImage1D",             gl_texturesubimage1d                },
{ "glTextureSubImage1DEXT",          gl_texturesubimage1dext             },
{ "glTextureSubImage2D",             gl_texturesubimage2d                },
{ "glTextureSubImage2DEXT",          gl_texturesubimage2dext             },
{ "glTextureSubImage3D",             gl_texturesubimage3d                },
{ "glTextureSubImage3DEXT",          gl_texturesubimage3dext             },
{ "glTextureView",                   gl_textureview                      },
{ "glTrackMatrixNV",                 gl_trackmatrixnv                    },
{ "glTransformFeedbackAttribsNV",    gl_transformfeedbackattribsnv       },
{ "glTransformFeedbackBufferBase",   gl_transformfeedbackbufferbase      },
{ "glTransformFeedbackBufferRange",  gl_transformfeedbackbufferrange     },
{ "glTransformPathNV",               gl_transformpathnv                  },
{ "glTranslated",                    gl_translated                       },
{ "glTranslatef",                    gl_translatef                       },
{ "glUniform1d",                     gl_uniform1d                        },
{ "glUniform1dv",                    gl_uniform1dv                       },
{ "glUniform1f",                     gl_uniform1f                        },
{ "glUniform1fARB",                  gl_uniform1farb                     },
{ "glUniform1fv",                    gl_uniform1fv                       },
{ "glUniform1fvARB",                 gl_uniform1fvarb                    },
{ "glUniform1i",                     gl_uniform1i                        },
{ "glUniform1i64NV",                 gl_uniform1i64nv                    },
{ "glUniform1i64vNV",                gl_uniform1i64vnv                   },
{ "glUniform1iARB",                  gl_uniform1iarb                     },
{ "glUniform1iv",                    gl_uniform1iv                       },
{ "glUniform1ivARB",                 gl_uniform1ivarb                    },
{ "glUniform1ui",                    gl_uniform1ui                       },
{ "glUniform1ui64NV",                gl_uniform1ui64nv                   },
{ "glUniform1ui64vNV",               gl_uniform1ui64vnv                  },
{ "glUniform1uiEXT",                 gl_uniform1uiext                    },
{ "glUniform1uiv",                   gl_uniform1uiv                      },
{ "glUniform1uivEXT",                gl_uniform1uivext                   },
{ "glUniform2d",                     gl_uniform2d                        },
{ "glUniform2dv",                    gl_uniform2dv                       },
{ "glUniform2f",                     gl_uniform2f                        },
{ "glUniform2fARB",                  gl_uniform2farb                     },
{ "glUniform2fv",                    gl_uniform2fv                       },
{ "glUniform2fvARB",                 gl_uniform2fvarb                    },
{ "glUniform2i",                     gl_uniform2i                        },
{ "glUniform2i64NV",                 gl_uniform2i64nv                    },
{ "glUniform2i64vNV",                gl_uniform2i64vnv                   },
{ "glUniform2iARB",                  gl_uniform2iarb                     },
{ "glUniform2iv",                    gl_uniform2iv                       },
{ "glUniform2ivARB",                 gl_uniform2ivarb                    },
{ "glUniform2ui",                    gl_uniform2ui                       },
{ "glUniform2ui64NV",                gl_uniform2ui64nv                   },
{ "glUniform2ui64vNV",               gl_uniform2ui64vnv                  },
{ "glUniform2uiEXT",                 gl_uniform2uiext                    },
{ "glUniform2uiv",                   gl_uniform2uiv                      },
{ "glUniform2uivEXT",                gl_uniform2uivext                   },
{ "glUniform3d",                     gl_uniform3d                        },
{ "glUniform3dv",                    gl_uniform3dv                       },
{ "glUniform3f",                     gl_uniform3f                        },
{ "glUniform3fARB",                  gl_uniform3farb                     },
{ "glUniform3fv",                    gl_uniform3fv                       },
{ "glUniform3fvARB",                 gl_uniform3fvarb                    },
{ "glUniform3i",                     gl_uniform3i                        },
{ "glUniform3i64NV",                 gl_uniform3i64nv                    },
{ "glUniform3i64vNV",                gl_uniform3i64vnv                   },
{ "glUniform3iARB",                  gl_uniform3iarb                     },
{ "glUniform3iv",                    gl_uniform3iv                       },
{ "glUniform3ivARB",                 gl_uniform3ivarb                    },
{ "glUniform3ui",                    gl_uniform3ui                       },
{ "glUniform3ui64NV",                gl_uniform3ui64nv                   },
{ "glUniform3ui64vNV",               gl_uniform3ui64vnv                  },
{ "glUniform3uiEXT",                 gl_uniform3uiext                    },
{ "glUniform3uiv",                   gl_uniform3uiv                      },
{ "glUniform3uivEXT",                gl_uniform3uivext                   },
{ "glUniform4d",                     gl_uniform4d                        },
{ "glUniform4dv",                    gl_uniform4dv                       },
{ "glUniform4f",                     gl_uniform4f                        },
{ "glUniform4fARB",                  gl_uniform4farb                     },
{ "glUniform4fv",                    gl_uniform4fv                       },
{ "glUniform4fvARB",                 gl_uniform4fvarb                    },
{ "glUniform4i",                     gl_uniform4i                        },
{ "glUniform4i64NV",                 gl_uniform4i64nv                    },
{ "glUniform4i64vNV",                gl_uniform4i64vnv                   },
{ "glUniform4iARB",                  gl_uniform4iarb                     },
{ "glUniform4iv",                    gl_uniform4iv                       },
{ "glUniform4ivARB",                 gl_uniform4ivarb                    },
{ "glUniform4ui",                    gl_uniform4ui                       },
{ "glUniform4ui64NV",                gl_uniform4ui64nv                   },
{ "glUniform4ui64vNV",               gl_uniform4ui64vnv                  },
{ "glUniform4uiEXT",                 gl_uniform4uiext                    },
{ "glUniform4uiv",                   gl_uniform4uiv                      },
{ "glUniform4uivEXT",                gl_uniform4uivext                   },
{ "glUniformBlockBinding",           gl_uniformblockbinding              },
{ "glUniformBufferEXT",              gl_uniformbufferext                 },
{ "glUniformHandleui64ARB",          gl_uniformhandleui64arb             },
{ "glUniformHandleui64NV",           gl_uniformhandleui64nv              },
{ "glUniformHandleui64vARB",         gl_uniformhandleui64varb            },
{ "glUniformHandleui64vNV",          gl_uniformhandleui64vnv             },
{ "glUniformMatrix2dv",              gl_uniformmatrix2dv                 },
{ "glUniformMatrix2fv",              gl_uniformmatrix2fv                 },
{ "glUniformMatrix2fvARB",           gl_uniformmatrix2fvarb              },
{ "glUniformMatrix2x3dv",            gl_uniformmatrix2x3dv               },
{ "glUniformMatrix2x3fv",            gl_uniformmatrix2x3fv               },
{ "glUniformMatrix2x4dv",            gl_uniformmatrix2x4dv               },
{ "glUniformMatrix2x4fv",            gl_uniformmatrix2x4fv               },
{ "glUniformMatrix3dv",              gl_uniformmatrix3dv                 },
{ "glUniformMatrix3fv",              gl_uniformmatrix3fv                 },
{ "glUniformMatrix3fvARB",           gl_uniformmatrix3fvarb              },
{ "glUniformMatrix3x2dv",            gl_uniformmatrix3x2dv               },
{ "glUniformMatrix3x2fv",            gl_uniformmatrix3x2fv               },
{ "glUniformMatrix3x4dv",            gl_uniformmatrix3x4dv               },
{ "glUniformMatrix3x4fv",            gl_uniformmatrix3x4fv               },
{ "glUniformMatrix4dv",              gl_uniformmatrix4dv                 },
{ "glUniformMatrix4fv",              gl_uniformmatrix4fv                 },
{ "glUniformMatrix4fvARB",           gl_uniformmatrix4fvarb              },
{ "glUniformMatrix4x2dv",            gl_uniformmatrix4x2dv               },
{ "glUniformMatrix4x2fv",            gl_uniformmatrix4x2fv               },
{ "glUniformMatrix4x3dv",            gl_uniformmatrix4x3dv               },
{ "glUniformMatrix4x3fv",            gl_uniformmatrix4x3fv               },
{ "glUniformSubroutinesuiv",         gl_uniformsubroutinesuiv            },
{ "glUniformui64NV",                 gl_uniformui64nv                    },
{ "glUniformui64vNV",                gl_uniformui64vnv                   },
{ "glUnlockArraysEXT",               gl_unlockarraysext                  },
{ "glUnmapBuffer",                   gl_unmapbuffer                      },
{ "glUnmapBufferARB",                gl_unmapbufferarb                   },
{ "glUnmapNamedBuffer",              gl_unmapnamedbuffer                 },
{ "glUnmapNamedBufferEXT",           gl_unmapnamedbufferext              },
{ "glUnmapObjectBufferATI",          gl_unmapobjectbufferati             },
{ "glUnmapTexture2DINTEL",           gl_unmaptexture2dintel              },
{ "glUpdateObjectBufferATI",         gl_updateobjectbufferati            },
{ "glUseProgram",                    gl_useprogram                       },
{ "glUseProgramObjectARB",           gl_useprogramobjectarb              },
{ "glUseProgramStages",              gl_useprogramstages                 },
{ "glUseShaderProgramEXT",           gl_useshaderprogramext              },
{ "glVDPAUFiniNV",                   gl_vdpaufininv                      },
{ "glVDPAUInitNV",                   gl_vdpauinitnv                      },
{ "glValidateProgram",               gl_validateprogram                  },
{ "glValidateProgramARB",            gl_validateprogramarb               },
{ "glValidateProgramPipeline",       gl_validateprogrampipeline          },
{ "glVariantArrayObjectATI",         gl_variantarrayobjectati            },
{ "glVariantPointerEXT",             gl_variantpointerext                },
{ "glVariantbvEXT",                  gl_variantbvext                     },
{ "glVariantdvEXT",                  gl_variantdvext                     },
{ "glVariantfvEXT",                  gl_variantfvext                     },
{ "glVariantivEXT",                  gl_variantivext                     },
{ "glVariantsvEXT",                  gl_variantsvext                     },
{ "glVariantubvEXT",                 gl_variantubvext                    },
{ "glVariantuivEXT",                 gl_variantuivext                    },
{ "glVariantusvEXT",                 gl_variantusvext                    },
{ "glVertex2d",                      gl_vertex2d                         },
{ "glVertex2dv",                     gl_vertex2dv                        },
{ "glVertex2f",                      gl_vertex2f                         },
{ "glVertex2fv",                     gl_vertex2fv                        },
{ "glVertex2i",                      gl_vertex2i                         },
{ "glVertex2iv",                     gl_vertex2iv                        },
{ "glVertex2s",                      gl_vertex2s                         },
{ "glVertex2sv",                     gl_vertex2sv                        },
{ "glVertex3d",                      gl_vertex3d                         },
{ "glVertex3dv",                     gl_vertex3dv                        },
{ "glVertex3f",                      gl_vertex3f                         },
{ "glVertex3fv",                     gl_vertex3fv                        },
{ "glVertex3i",                      gl_vertex3i                         },
{ "glVertex3iv",                     gl_vertex3iv                        },
{ "glVertex3s",                      gl_vertex3s                         },
{ "glVertex3sv",                     gl_vertex3sv                        },
{ "glVertex4d",                      gl_vertex4d                         },
{ "glVertex4dv",                     gl_vertex4dv                        },
{ "glVertex4f",                      gl_vertex4f                         },
{ "glVertex4fv",                     gl_vertex4fv                        },
{ "glVertex4i",                      gl_vertex4i                         },
{ "glVertex4iv",                     gl_vertex4iv                        },
{ "glVertex4s",                      gl_vertex4s                         },
{ "glVertex4sv",                     gl_vertex4sv                        },
{ "glVertexArrayAttribBinding",      gl_vertexarrayattribbinding         },
{ "glVertexArrayAttribFormat",       gl_vertexarrayattribformat          },
{ "glVertexArrayAttribIFormat",      gl_vertexarrayattribiformat         },
{ "glVertexArrayAttribLFormat",      gl_vertexarrayattriblformat         },
{ "glVertexArrayBindVertexBufferEXT",gl_vertexarraybindvertexbufferext   },
{ "glVertexArrayBindingDivisor",     gl_vertexarraybindingdivisor        },
{ "glVertexArrayColorOffsetEXT",     gl_vertexarraycoloroffsetext        },
{ "glVertexArrayEdgeFlagOffsetEXT",  gl_vertexarrayedgeflagoffsetext     },
{ "glVertexArrayElementBuffer",      gl_vertexarrayelementbuffer         },
{ "glVertexArrayFogCoordOffsetEXT",  gl_vertexarrayfogcoordoffsetext     },
{ "glVertexArrayIndexOffsetEXT",     gl_vertexarrayindexoffsetext        },
{ "glVertexArrayMultiTexCoordOffsetEXT",gl_vertexarraymultitexcoordoffsetext },
{ "glVertexArrayNormalOffsetEXT",    gl_vertexarraynormaloffsetext       },
{ "glVertexArrayParameteriAPPLE",    gl_vertexarrayparameteriapple       },
{ "glVertexArrayRangeAPPLE",         gl_vertexarrayrangeapple            },
{ "glVertexArrayRangeNV",            gl_vertexarrayrangenv               },
{ "glVertexArraySecondaryColorOffsetEXT",gl_vertexarraysecondarycoloroffsetext },
{ "glVertexArrayTexCoordOffsetEXT",  gl_vertexarraytexcoordoffsetext     },
{ "glVertexArrayVertexAttribBindingEXT",gl_vertexarrayvertexattribbindingext },
{ "glVertexArrayVertexAttribDivisorEXT",gl_vertexarrayvertexattribdivisorext },
{ "glVertexArrayVertexAttribFormatEXT",gl_vertexarrayvertexattribformatext },
{ "glVertexArrayVertexAttribIFormatEXT",gl_vertexarrayvertexattribiformatext },
{ "glVertexArrayVertexAttribIOffsetEXT",gl_vertexarrayvertexattribioffsetext },
{ "glVertexArrayVertexAttribLFormatEXT",gl_vertexarrayvertexattriblformatext },
{ "glVertexArrayVertexAttribLOffsetEXT",gl_vertexarrayvertexattribloffsetext },
{ "glVertexArrayVertexAttribOffsetEXT",gl_vertexarrayvertexattriboffsetext },
{ "glVertexArrayVertexBindingDivisorEXT",gl_vertexarrayvertexbindingdivisorext },
{ "glVertexArrayVertexBuffer",       gl_vertexarrayvertexbuffer          },
{ "glVertexArrayVertexBuffers",      gl_vertexarrayvertexbuffers         },
{ "glVertexArrayVertexOffsetEXT",    gl_vertexarrayvertexoffsetext       },
{ "glVertexAttrib1d",                gl_vertexattrib1d                   },
{ "glVertexAttrib1dARB",             gl_vertexattrib1darb                },
{ "glVertexAttrib1dNV",              gl_vertexattrib1dnv                 },
{ "glVertexAttrib1dv",               gl_vertexattrib1dv                  },
{ "glVertexAttrib1dvARB",            gl_vertexattrib1dvarb               },
{ "glVertexAttrib1dvNV",             gl_vertexattrib1dvnv                },
{ "glVertexAttrib1f",                gl_vertexattrib1f                   },
{ "glVertexAttrib1fARB",             gl_vertexattrib1farb                },
{ "glVertexAttrib1fNV",              gl_vertexattrib1fnv                 },
{ "glVertexAttrib1fv",               gl_vertexattrib1fv                  },
{ "glVertexAttrib1fvARB",            gl_vertexattrib1fvarb               },
{ "glVertexAttrib1fvNV",             gl_vertexattrib1fvnv                },
{ "glVertexAttrib1s",                gl_vertexattrib1s                   },
{ "glVertexAttrib1sARB",             gl_vertexattrib1sarb                },
{ "glVertexAttrib1sNV",              gl_vertexattrib1snv                 },
{ "glVertexAttrib1sv",               gl_vertexattrib1sv                  },
{ "glVertexAttrib1svARB",            gl_vertexattrib1svarb               },
{ "glVertexAttrib1svNV",             gl_vertexattrib1svnv                },
{ "glVertexAttrib2d",                gl_vertexattrib2d                   },
{ "glVertexAttrib2dARB",             gl_vertexattrib2darb                },
{ "glVertexAttrib2dNV",              gl_vertexattrib2dnv                 },
{ "glVertexAttrib2dv",               gl_vertexattrib2dv                  },
{ "glVertexAttrib2dvARB",            gl_vertexattrib2dvarb               },
{ "glVertexAttrib2dvNV",             gl_vertexattrib2dvnv                },
{ "glVertexAttrib2f",                gl_vertexattrib2f                   },
{ "glVertexAttrib2fARB",             gl_vertexattrib2farb                },
{ "glVertexAttrib2fNV",              gl_vertexattrib2fnv                 },
{ "glVertexAttrib2fv",               gl_vertexattrib2fv                  },
{ "glVertexAttrib2fvARB",            gl_vertexattrib2fvarb               },
{ "glVertexAttrib2fvNV",             gl_vertexattrib2fvnv                },
{ "glVertexAttrib2s",                gl_vertexattrib2s                   },
{ "glVertexAttrib2sARB",             gl_vertexattrib2sarb                },
{ "glVertexAttrib2sNV",              gl_vertexattrib2snv                 },
{ "glVertexAttrib2sv",               gl_vertexattrib2sv                  },
{ "glVertexAttrib2svARB",            gl_vertexattrib2svarb               },
{ "glVertexAttrib2svNV",             gl_vertexattrib2svnv                },
{ "glVertexAttrib3d",                gl_vertexattrib3d                   },
{ "glVertexAttrib3dARB",             gl_vertexattrib3darb                },
{ "glVertexAttrib3dNV",              gl_vertexattrib3dnv                 },
{ "glVertexAttrib3dv",               gl_vertexattrib3dv                  },
{ "glVertexAttrib3dvARB",            gl_vertexattrib3dvarb               },
{ "glVertexAttrib3dvNV",             gl_vertexattrib3dvnv                },
{ "glVertexAttrib3f",                gl_vertexattrib3f                   },
{ "glVertexAttrib3fARB",             gl_vertexattrib3farb                },
{ "glVertexAttrib3fNV",              gl_vertexattrib3fnv                 },
{ "glVertexAttrib3fv",               gl_vertexattrib3fv                  },
{ "glVertexAttrib3fvARB",            gl_vertexattrib3fvarb               },
{ "glVertexAttrib3fvNV",             gl_vertexattrib3fvnv                },
{ "glVertexAttrib3s",                gl_vertexattrib3s                   },
{ "glVertexAttrib3sARB",             gl_vertexattrib3sarb                },
{ "glVertexAttrib3sNV",              gl_vertexattrib3snv                 },
{ "glVertexAttrib3sv",               gl_vertexattrib3sv                  },
{ "glVertexAttrib3svARB",            gl_vertexattrib3svarb               },
{ "glVertexAttrib3svNV",             gl_vertexattrib3svnv                },
{ "glVertexAttrib4Nbv",              gl_vertexattrib4nbv                 },
{ "glVertexAttrib4NbvARB",           gl_vertexattrib4nbvarb              },
{ "glVertexAttrib4Niv",              gl_vertexattrib4niv                 },
{ "glVertexAttrib4NivARB",           gl_vertexattrib4nivarb              },
{ "glVertexAttrib4Nsv",              gl_vertexattrib4nsv                 },
{ "glVertexAttrib4NsvARB",           gl_vertexattrib4nsvarb              },
{ "glVertexAttrib4Nub",              gl_vertexattrib4nub                 },
{ "glVertexAttrib4NubARB",           gl_vertexattrib4nubarb              },
{ "glVertexAttrib4Nubv",             gl_vertexattrib4nubv                },
{ "glVertexAttrib4NubvARB",          gl_vertexattrib4nubvarb             },
{ "glVertexAttrib4Nuiv",             gl_vertexattrib4nuiv                },
{ "glVertexAttrib4NuivARB",          gl_vertexattrib4nuivarb             },
{ "glVertexAttrib4Nusv",             gl_vertexattrib4nusv                },
{ "glVertexAttrib4NusvARB",          gl_vertexattrib4nusvarb             },
{ "glVertexAttrib4bv",               gl_vertexattrib4bv                  },
{ "glVertexAttrib4bvARB",            gl_vertexattrib4bvarb               },
{ "glVertexAttrib4d",                gl_vertexattrib4d                   },
{ "glVertexAttrib4dARB",             gl_vertexattrib4darb                },
{ "glVertexAttrib4dNV",              gl_vertexattrib4dnv                 },
{ "glVertexAttrib4dv",               gl_vertexattrib4dv                  },
{ "glVertexAttrib4dvARB",            gl_vertexattrib4dvarb               },
{ "glVertexAttrib4dvNV",             gl_vertexattrib4dvnv                },
{ "glVertexAttrib4f",                gl_vertexattrib4f                   },
{ "glVertexAttrib4fARB",             gl_vertexattrib4farb                },
{ "glVertexAttrib4fNV",              gl_vertexattrib4fnv                 },
{ "glVertexAttrib4fv",               gl_vertexattrib4fv                  },
{ "glVertexAttrib4fvARB",            gl_vertexattrib4fvarb               },
{ "glVertexAttrib4fvNV",             gl_vertexattrib4fvnv                },
{ "glVertexAttrib4iv",               gl_vertexattrib4iv                  },
{ "glVertexAttrib4ivARB",            gl_vertexattrib4ivarb               },
{ "glVertexAttrib4s",                gl_vertexattrib4s                   },
{ "glVertexAttrib4sARB",             gl_vertexattrib4sarb                },
{ "glVertexAttrib4sNV",              gl_vertexattrib4snv                 },
{ "glVertexAttrib4sv",               gl_vertexattrib4sv                  },
{ "glVertexAttrib4svARB",            gl_vertexattrib4svarb               },
{ "glVertexAttrib4svNV",             gl_vertexattrib4svnv                },
{ "glVertexAttrib4ubNV",             gl_vertexattrib4ubnv                },
{ "glVertexAttrib4ubv",              gl_vertexattrib4ubv                 },
{ "glVertexAttrib4ubvARB",           gl_vertexattrib4ubvarb              },
{ "glVertexAttrib4ubvNV",            gl_vertexattrib4ubvnv               },
{ "glVertexAttrib4uiv",              gl_vertexattrib4uiv                 },
{ "glVertexAttrib4uivARB",           gl_vertexattrib4uivarb              },
{ "glVertexAttrib4usv",              gl_vertexattrib4usv                 },
{ "glVertexAttrib4usvARB",           gl_vertexattrib4usvarb              },
{ "glVertexAttribArrayObjectATI",    gl_vertexattribarrayobjectati       },
{ "glVertexAttribBinding",           gl_vertexattribbinding              },
{ "glVertexAttribDivisor",           gl_vertexattribdivisor              },
{ "glVertexAttribDivisorARB",        gl_vertexattribdivisorarb           },
{ "glVertexAttribFormat",            gl_vertexattribformat               },
{ "glVertexAttribFormatNV",          gl_vertexattribformatnv             },
{ "glVertexAttribI1i",               gl_vertexattribi1i                  },
{ "glVertexAttribI1iEXT",            gl_vertexattribi1iext               },
{ "glVertexAttribI1iv",              gl_vertexattribi1iv                 },
{ "glVertexAttribI1ivEXT",           gl_vertexattribi1ivext              },
{ "glVertexAttribI1ui",              gl_vertexattribi1ui                 },
{ "glVertexAttribI1uiEXT",           gl_vertexattribi1uiext              },
{ "glVertexAttribI1uiv",             gl_vertexattribi1uiv                },
{ "glVertexAttribI1uivEXT",          gl_vertexattribi1uivext             },
{ "glVertexAttribI2i",               gl_vertexattribi2i                  },
{ "glVertexAttribI2iEXT",            gl_vertexattribi2iext               },
{ "glVertexAttribI2iv",              gl_vertexattribi2iv                 },
{ "glVertexAttribI2ivEXT",           gl_vertexattribi2ivext              },
{ "glVertexAttribI2ui",              gl_vertexattribi2ui                 },
{ "glVertexAttribI2uiEXT",           gl_vertexattribi2uiext              },
{ "glVertexAttribI2uiv",             gl_vertexattribi2uiv                },
{ "glVertexAttribI2uivEXT",          gl_vertexattribi2uivext             },
{ "glVertexAttribI3i",               gl_vertexattribi3i                  },
{ "glVertexAttribI3iEXT",            gl_vertexattribi3iext               },
{ "glVertexAttribI3iv",              gl_vertexattribi3iv                 },
{ "glVertexAttribI3ivEXT",           gl_vertexattribi3ivext              },
{ "glVertexAttribI3ui",              gl_vertexattribi3ui                 },
{ "glVertexAttribI3uiEXT",           gl_vertexattribi3uiext              },
{ "glVertexAttribI3uiv",             gl_vertexattribi3uiv                },
{ "glVertexAttribI3uivEXT",          gl_vertexattribi3uivext             },
{ "glVertexAttribI4bv",              gl_vertexattribi4bv                 },
{ "glVertexAttribI4bvEXT",           gl_vertexattribi4bvext              },
{ "glVertexAttribI4i",               gl_vertexattribi4i                  },
{ "glVertexAttribI4iEXT",            gl_vertexattribi4iext               },
{ "glVertexAttribI4iv",              gl_vertexattribi4iv                 },
{ "glVertexAttribI4ivEXT",           gl_vertexattribi4ivext              },
{ "glVertexAttribI4sv",              gl_vertexattribi4sv                 },
{ "glVertexAttribI4svEXT",           gl_vertexattribi4svext              },
{ "glVertexAttribI4ubv",             gl_vertexattribi4ubv                },
{ "glVertexAttribI4ubvEXT",          gl_vertexattribi4ubvext             },
{ "glVertexAttribI4ui",              gl_vertexattribi4ui                 },
{ "glVertexAttribI4uiEXT",           gl_vertexattribi4uiext              },
{ "glVertexAttribI4uiv",             gl_vertexattribi4uiv                },
{ "glVertexAttribI4uivEXT",          gl_vertexattribi4uivext             },
{ "glVertexAttribI4usv",             gl_vertexattribi4usv                },
{ "glVertexAttribI4usvEXT",          gl_vertexattribi4usvext             },
{ "glVertexAttribIFormat",           gl_vertexattribiformat              },
{ "glVertexAttribIFormatNV",         gl_vertexattribiformatnv            },
{ "glVertexAttribIPointer",          gl_vertexattribipointer             },
{ "glVertexAttribIPointerEXT",       gl_vertexattribipointerext          },
{ "glVertexAttribL1d",               gl_vertexattribl1d                  },
{ "glVertexAttribL1dEXT",            gl_vertexattribl1dext               },
{ "glVertexAttribL1dv",              gl_vertexattribl1dv                 },
{ "glVertexAttribL1dvEXT",           gl_vertexattribl1dvext              },
{ "glVertexAttribL1i64NV",           gl_vertexattribl1i64nv              },
{ "glVertexAttribL1i64vNV",          gl_vertexattribl1i64vnv             },
{ "glVertexAttribL1ui64ARB",         gl_vertexattribl1ui64arb            },
{ "glVertexAttribL1ui64NV",          gl_vertexattribl1ui64nv             },
{ "glVertexAttribL1ui64vARB",        gl_vertexattribl1ui64varb           },
{ "glVertexAttribL1ui64vNV",         gl_vertexattribl1ui64vnv            },
{ "glVertexAttribL2d",               gl_vertexattribl2d                  },
{ "glVertexAttribL2dEXT",            gl_vertexattribl2dext               },
{ "glVertexAttribL2dv",              gl_vertexattribl2dv                 },
{ "glVertexAttribL2dvEXT",           gl_vertexattribl2dvext              },
{ "glVertexAttribL2i64NV",           gl_vertexattribl2i64nv              },
{ "glVertexAttribL2i64vNV",          gl_vertexattribl2i64vnv             },
{ "glVertexAttribL2ui64NV",          gl_vertexattribl2ui64nv             },
{ "glVertexAttribL2ui64vNV",         gl_vertexattribl2ui64vnv            },
{ "glVertexAttribL3d",               gl_vertexattribl3d                  },
{ "glVertexAttribL3dEXT",            gl_vertexattribl3dext               },
{ "glVertexAttribL3dv",              gl_vertexattribl3dv                 },
{ "glVertexAttribL3dvEXT",           gl_vertexattribl3dvext              },
{ "glVertexAttribL3i64NV",           gl_vertexattribl3i64nv              },
{ "glVertexAttribL3i64vNV",          gl_vertexattribl3i64vnv             },
{ "glVertexAttribL3ui64NV",          gl_vertexattribl3ui64nv             },
{ "glVertexAttribL3ui64vNV",         gl_vertexattribl3ui64vnv            },
{ "glVertexAttribL4d",               gl_vertexattribl4d                  },
{ "glVertexAttribL4dEXT",            gl_vertexattribl4dext               },
{ "glVertexAttribL4dv",              gl_vertexattribl4dv                 },
{ "glVertexAttribL4dvEXT",           gl_vertexattribl4dvext              },
{ "glVertexAttribL4i64NV",           gl_vertexattribl4i64nv              },
{ "glVertexAttribL4i64vNV",          gl_vertexattribl4i64vnv             },
{ "glVertexAttribL4ui64NV",          gl_vertexattribl4ui64nv             },
{ "glVertexAttribL4ui64vNV",         gl_vertexattribl4ui64vnv            },
{ "glVertexAttribLFormat",           gl_vertexattriblformat              },
{ "glVertexAttribLFormatNV",         gl_vertexattriblformatnv            },
{ "glVertexAttribLPointer",          gl_vertexattriblpointer             },
{ "glVertexAttribLPointerEXT",       gl_vertexattriblpointerext          },
{ "glVertexAttribP1ui",              gl_vertexattribp1ui                 },
{ "glVertexAttribP1uiv",             gl_vertexattribp1uiv                },
{ "glVertexAttribP2ui",              gl_vertexattribp2ui                 },
{ "glVertexAttribP2uiv",             gl_vertexattribp2uiv                },
{ "glVertexAttribP3ui",              gl_vertexattribp3ui                 },
{ "glVertexAttribP3uiv",             gl_vertexattribp3uiv                },
{ "glVertexAttribP4ui",              gl_vertexattribp4ui                 },
{ "glVertexAttribP4uiv",             gl_vertexattribp4uiv                },
{ "glVertexAttribParameteriAMD",     gl_vertexattribparameteriamd        },
{ "glVertexAttribPointerARB",        gl_vertexattribpointerarb           },
{ "glVertexAttribPointerNV",         gl_vertexattribpointernv            },
{ "glVertexAttribs1dvNV",            gl_vertexattribs1dvnv               },
{ "glVertexAttribs1fvNV",            gl_vertexattribs1fvnv               },
{ "glVertexAttribs1svNV",            gl_vertexattribs1svnv               },
{ "glVertexAttribs2dvNV",            gl_vertexattribs2dvnv               },
{ "glVertexAttribs2fvNV",            gl_vertexattribs2fvnv               },
{ "glVertexAttribs2svNV",            gl_vertexattribs2svnv               },
{ "glVertexAttribs3dvNV",            gl_vertexattribs3dvnv               },
{ "glVertexAttribs3fvNV",            gl_vertexattribs3fvnv               },
{ "glVertexAttribs3svNV",            gl_vertexattribs3svnv               },
{ "glVertexAttribs4dvNV",            gl_vertexattribs4dvnv               },
{ "glVertexAttribs4fvNV",            gl_vertexattribs4fvnv               },
{ "glVertexAttribs4svNV",            gl_vertexattribs4svnv               },
{ "glVertexAttribs4ubvNV",           gl_vertexattribs4ubvnv              },
{ "glVertexBindingDivisor",          gl_vertexbindingdivisor             },
{ "glVertexBlendARB",                gl_vertexblendarb                   },
{ "glVertexBlendEnvfATI",            gl_vertexblendenvfati               },
{ "glVertexBlendEnviATI",            gl_vertexblendenviati               },
{ "glVertexFormatNV",                gl_vertexformatnv                   },
{ "glVertexP2ui",                    gl_vertexp2ui                       },
{ "glVertexP2uiv",                   gl_vertexp2uiv                      },
{ "glVertexP3ui",                    gl_vertexp3ui                       },
{ "glVertexP3uiv",                   gl_vertexp3uiv                      },
{ "glVertexP4ui",                    gl_vertexp4ui                       },
{ "glVertexP4uiv",                   gl_vertexp4uiv                      },
{ "glVertexPointerEXT",              gl_vertexpointerext                 },
{ "glVertexStream1dATI",             gl_vertexstream1dati                },
{ "glVertexStream1dvATI",            gl_vertexstream1dvati               },
{ "glVertexStream1fATI",             gl_vertexstream1fati                },
{ "glVertexStream1fvATI",            gl_vertexstream1fvati               },
{ "glVertexStream1iATI",             gl_vertexstream1iati                },
{ "glVertexStream1ivATI",            gl_vertexstream1ivati               },
{ "glVertexStream1sATI",             gl_vertexstream1sati                },
{ "glVertexStream1svATI",            gl_vertexstream1svati               },
{ "glVertexStream2dATI",             gl_vertexstream2dati                },
{ "glVertexStream2dvATI",            gl_vertexstream2dvati               },
{ "glVertexStream2fATI",             gl_vertexstream2fati                },
{ "glVertexStream2fvATI",            gl_vertexstream2fvati               },
{ "glVertexStream2iATI",             gl_vertexstream2iati                },
{ "glVertexStream2ivATI",            gl_vertexstream2ivati               },
{ "glVertexStream2sATI",             gl_vertexstream2sati                },
{ "glVertexStream2svATI",            gl_vertexstream2svati               },
{ "glVertexStream3dATI",             gl_vertexstream3dati                },
{ "glVertexStream3dvATI",            gl_vertexstream3dvati               },
{ "glVertexStream3fATI",             gl_vertexstream3fati                },
{ "glVertexStream3fvATI",            gl_vertexstream3fvati               },
{ "glVertexStream3iATI",             gl_vertexstream3iati                },
{ "glVertexStream3ivATI",            gl_vertexstream3ivati               },
{ "glVertexStream3sATI",             gl_vertexstream3sati                },
{ "glVertexStream3svATI",            gl_vertexstream3svati               },
{ "glVertexStream4dATI",             gl_vertexstream4dati                },
{ "glVertexStream4dvATI",            gl_vertexstream4dvati               },
{ "glVertexStream4fATI",             gl_vertexstream4fati                },
{ "glVertexStream4fvATI",            gl_vertexstream4fvati               },
{ "glVertexStream4iATI",             gl_vertexstream4iati                },
{ "glVertexStream4ivATI",            gl_vertexstream4ivati               },
{ "glVertexStream4sATI",             gl_vertexstream4sati                },
{ "glVertexStream4svATI",            gl_vertexstream4svati               },
{ "glVertexWeightPointerEXT",        gl_vertexweightpointerext           },
{ "glVertexWeightfEXT",              gl_vertexweightfext                 },
{ "glVertexWeightfvEXT",             gl_vertexweightfvext                },
{ "glVideoCaptureNV",                gl_videocapturenv                   },
{ "glVideoCaptureStreamParameterdvNV",gl_videocapturestreamparameterdvnv  },
{ "glVideoCaptureStreamParameterfvNV",gl_videocapturestreamparameterfvnv  },
{ "glVideoCaptureStreamParameterivNV",gl_videocapturestreamparameterivnv  },
{ "glViewport",                      gl_viewport                         },
{ "glViewportArrayv",                gl_viewportarrayv                   },
{ "glViewportIndexedf",              gl_viewportindexedf                 },
{ "glViewportIndexedfv",             gl_viewportindexedfv                },
{ "glWaitSync",                      gl_waitsync                         },
{ "glWeightPathsNV",                 gl_weightpathsnv                    },
{ "glWeightPointerARB",              gl_weightpointerarb                 },
{ "glWeightbvARB",                   gl_weightbvarb                      },
{ "glWeightdvARB",                   gl_weightdvarb                      },
{ "glWeightfvARB",                   gl_weightfvarb                      },
{ "glWeightivARB",                   gl_weightivarb                      },
{ "glWeightsvARB",                   gl_weightsvarb                      },
{ "glWeightubvARB",                  gl_weightubvarb                     },
{ "glWeightuivARB",                  gl_weightuivarb                     },
{ "glWeightusvARB",                  gl_weightusvarb                     },
{ "glWindowPos2d",                   gl_windowpos2d                      },
{ "glWindowPos2dARB",                gl_windowpos2darb                   },
{ "glWindowPos2dMESA",               gl_windowpos2dmesa                  },
{ "glWindowPos2dv",                  gl_windowpos2dv                     },
{ "glWindowPos2dvARB",               gl_windowpos2dvarb                  },
{ "glWindowPos2dvMESA",              gl_windowpos2dvmesa                 },
{ "glWindowPos2f",                   gl_windowpos2f                      },
{ "glWindowPos2fARB",                gl_windowpos2farb                   },
{ "glWindowPos2fMESA",               gl_windowpos2fmesa                  },
{ "glWindowPos2fv",                  gl_windowpos2fv                     },
{ "glWindowPos2fvARB",               gl_windowpos2fvarb                  },
{ "glWindowPos2fvMESA",              gl_windowpos2fvmesa                 },
{ "glWindowPos2i",                   gl_windowpos2i                      },
{ "glWindowPos2iARB",                gl_windowpos2iarb                   },
{ "glWindowPos2iMESA",               gl_windowpos2imesa                  },
{ "glWindowPos2iv",                  gl_windowpos2iv                     },
{ "glWindowPos2ivARB",               gl_windowpos2ivarb                  },
{ "glWindowPos2ivMESA",              gl_windowpos2ivmesa                 },
{ "glWindowPos2s",                   gl_windowpos2s                      },
{ "glWindowPos2sARB",                gl_windowpos2sarb                   },
{ "glWindowPos2sMESA",               gl_windowpos2smesa                  },
{ "glWindowPos2sv",                  gl_windowpos2sv                     },
{ "glWindowPos2svARB",               gl_windowpos2svarb                  },
{ "glWindowPos2svMESA",              gl_windowpos2svmesa                 },
{ "glWindowPos3d",                   gl_windowpos3d                      },
{ "glWindowPos3dARB",                gl_windowpos3darb                   },
{ "glWindowPos3dMESA",               gl_windowpos3dmesa                  },
{ "glWindowPos3dv",                  gl_windowpos3dv                     },
{ "glWindowPos3dvARB",               gl_windowpos3dvarb                  },
{ "glWindowPos3dvMESA",              gl_windowpos3dvmesa                 },
{ "glWindowPos3f",                   gl_windowpos3f                      },
{ "glWindowPos3fARB",                gl_windowpos3farb                   },
{ "glWindowPos3fMESA",               gl_windowpos3fmesa                  },
{ "glWindowPos3fv",                  gl_windowpos3fv                     },
{ "glWindowPos3fvARB",               gl_windowpos3fvarb                  },
{ "glWindowPos3fvMESA",              gl_windowpos3fvmesa                 },
{ "glWindowPos3i",                   gl_windowpos3i                      },
{ "glWindowPos3iARB",                gl_windowpos3iarb                   },
{ "glWindowPos3iMESA",               gl_windowpos3imesa                  },
{ "glWindowPos3iv",                  gl_windowpos3iv                     },
{ "glWindowPos3ivARB",               gl_windowpos3ivarb                  },
{ "glWindowPos3ivMESA",              gl_windowpos3ivmesa                 },
{ "glWindowPos3s",                   gl_windowpos3s                      },
{ "glWindowPos3sARB",                gl_windowpos3sarb                   },
{ "glWindowPos3sMESA",               gl_windowpos3smesa                  },
{ "glWindowPos3sv",                  gl_windowpos3sv                     },
{ "glWindowPos3svARB",               gl_windowpos3svarb                  },
{ "glWindowPos3svMESA",              gl_windowpos3svmesa                 },
{ "glWindowPos4dMESA",               gl_windowpos4dmesa                  },
{ "glWindowPos4dvMESA",              gl_windowpos4dvmesa                 },
{ "glWindowPos4fMESA",               gl_windowpos4fmesa                  },
{ "glWindowPos4fvMESA",              gl_windowpos4fvmesa                 },
{ "glWindowPos4iMESA",               gl_windowpos4imesa                  },
{ "glWindowPos4ivMESA",              gl_windowpos4ivmesa                 },
{ "glWindowPos4sMESA",               gl_windowpos4smesa                  },
{ "glWindowPos4svMESA",              gl_windowpos4svmesa                 },
{ "glWriteMaskEXT",                  gl_writemaskext                     },
{ "gluBeginCurve",                   glu_begincurve                      },
{ "gluBeginSurface",                 glu_beginsurface                    },
{ "gluBeginTrim",                    glu_begintrim                       },
{ "gluBuild1DMipmapLevels",          glu_build1dmipmaplevels             },
{ "gluBuild1DMipmaps",               glu_build1dmipmaps                  },
{ "gluBuild2DMipmapLevels",          glu_build2dmipmaplevels             },
{ "gluBuild2DMipmaps",               glu_build2dmipmaps                  },
{ "gluBuild3DMipmapLevels",          glu_build3dmipmaplevels             },
{ "gluBuild3DMipmaps",               glu_build3dmipmaps                  },
{ "gluCheckExtension",               glu_checkextension                  },
{ "gluCylinder",                     glu_cylinder                        },
{ "gluDeleteNurbsRenderer",          glu_deletenurbsrenderer             },
{ "gluDeleteQuadric",                glu_deletequadric                   },
{ "gluDisk",                         glu_disk                            },
{ "gluEndCurve",                     glu_endcurve                        },
{ "gluEndSurface",                   glu_endsurface                      },
{ "gluEndTrim",                      glu_endtrim                         },
{ "gluGetNurbsProperty",             glu_getnurbsproperty                },
{ "gluGetTessProperty",              glu_gettessproperty                 },
{ "gluLoadSamplingMatrices",         glu_loadsamplingmatrices            },
{ "gluLookAt",                       glu_lookat                          },
{ "gluNewNurbsRenderer",             glu_newnurbsrenderer                },
{ "gluNewQuadric",                   glu_newquadric                      },
{ "gluNurbsCurve",                   glu_nurbscurve                      },
{ "gluNurbsProperty",                glu_nurbsproperty                   },
{ "gluNurbsSurface",                 glu_nurbssurface                    },
{ "gluOrtho2D",                      glu_ortho2d                         },
{ "gluPartialDisk",                  glu_partialdisk                     },
{ "gluPerspective",                  glu_perspective                     },
{ "gluPickMatrix",                   glu_pickmatrix                      },
{ "gluProject",                      glu_project                         },
{ "gluPwlCurve",                     glu_pwlcurve                        },
{ "gluQuadricDrawStyle",             glu_quadricdrawstyle                },
{ "gluQuadricNormals",               glu_quadricnormals                  },
{ "gluQuadricOrientation",           glu_quadricorientation              },
{ "gluQuadricTexture",               glu_quadrictexture                  },
{ "gluScaleImage",                   glu_scaleimage                      },
{ "gluSphere",                       glu_sphere                          },
{ "gluUnProject",                    glu_unproject                       },
{ "gluUnProject4",                   glu_unproject4                      },
{ "glutSolidCone",                   glut_solidcone                      },
{ "glutSolidCube",                   glut_solidcube                      },
{ "glutSolidDodecahedron",           glut_soliddodecahedron              },
{ "glutSolidIcosahedron",            glut_solidicosahedron               },
{ "glutSolidOctahedron",             glut_solidoctahedron                },
{ "glutSolidSphere",                 glut_solidsphere                    },
{ "glutSolidTeapot",                 glut_solidteapot                    },
{ "glutSolidTetrahedron",            glut_solidtetrahedron               },
{ "glutSolidTorus",                  glut_solidtorus                     },
{ "glutWireCone",                    glut_wirecone                       },
{ "glutWireCube",                    glut_wirecube                       },
{ "glutWireDodecahedron",            glut_wiredodecahedron               },
{ "glutWireIcosahedron",             glut_wireicosahedron                },
{ "glutWireOctahedron",              glut_wireoctahedron                 },
{ "glutWireSphere",                  glut_wiresphere                     },
{ "glutWireTeapot",                  glut_wireteapot                     },
{ "glutWireTetrahedron",             glut_wiretetrahedron                },
{ "glutWireTorus",                   glut_wiretorus                      }};
