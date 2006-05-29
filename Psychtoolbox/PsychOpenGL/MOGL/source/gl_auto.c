
/*
 * gl_auto.c
 *
 * 30-May-2006 -- created (moglgen)
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

void gl_colorpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glColorPointer) mogl_glunsupported("glColorPointer");
	glColorPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

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

void gl_drawelements( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawElements) mogl_glunsupported("glDrawElements");
	glDrawElements((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_drawpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawPixels) mogl_glunsupported("glDrawPixels");
	glDrawPixels((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const GLvoid*)mxGetData(prhs[4]));

}

void gl_drawrangeelements( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glDrawRangeElements) mogl_glunsupported("glDrawRangeElements");
	glDrawRangeElements((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

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

void gl_getcolortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTable) mogl_glunsupported("glGetColorTable");
	glGetColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_getcolortableparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameterfv) mogl_glunsupported("glGetColorTableParameterfv");
	glGetColorTableParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getcolortableparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetColorTableParameteriv) mogl_glunsupported("glGetColorTableParameteriv");
	glGetColorTableParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getconvolutionfilter( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionFilter) mogl_glunsupported("glGetConvolutionFilter");
	glGetConvolutionFilter((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_getconvolutionparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionParameterfv) mogl_glunsupported("glGetConvolutionParameterfv");
	glGetConvolutionParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getconvolutionparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetConvolutionParameteriv) mogl_glunsupported("glGetConvolutionParameteriv");
	glGetConvolutionParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

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

void gl_gethistogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogram) mogl_glunsupported("glGetHistogram");
	glGetHistogram((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

}

void gl_gethistogramparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogramParameterfv) mogl_glunsupported("glGetHistogramParameterfv");
	glGetHistogramParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gethistogramparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetHistogramParameteriv) mogl_glunsupported("glGetHistogramParameteriv");
	glGetHistogramParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

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

void gl_getminmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetMinmax) mogl_glunsupported("glGetMinmax");
	glGetMinmax((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

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

void gl_getseparablefilter( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetSeparableFilter) mogl_glunsupported("glGetSeparableFilter");
	glGetSeparableFilter((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]),
		(GLvoid*)mxGetData(prhs[5]));

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

void gl_normalpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glNormalPointer) mogl_glunsupported("glNormalPointer");
	glNormalPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

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

void gl_readpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glReadPixels) mogl_glunsupported("glReadPixels");
	glReadPixels((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLvoid*)mxGetData(prhs[6]));

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

void gl_texcoordpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexCoordPointer) mogl_glunsupported("glTexCoordPointer");
	glTexCoordPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

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
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(const GLvoid*)mxGetData(prhs[7]));

}

void gl_teximage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage2D) mogl_glunsupported("glTexImage2D");
	glTexImage2D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]),
		(GLenum)mxGetScalar(prhs[7]),
		(const GLvoid*)mxGetData(prhs[8]));

}

void gl_teximage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glTexImage3D) mogl_glunsupported("glTexImage3D");
	glTexImage3D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
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

void gl_vertexpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexPointer) mogl_glunsupported("glVertexPointer");
	glVertexPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

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

void gl_bufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferData) mogl_glunsupported("glBufferData");
	glBufferData((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_buffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glBufferSubData) mogl_glunsupported("glBufferSubData");
	glBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_getbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetBufferSubData) mogl_glunsupported("glGetBufferSubData");
	glGetBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_mapbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glMapBuffer) mogl_glunsupported("glMapBuffer");
	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)glMapBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

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

void gl_vertexattribpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribPointer) mogl_glunsupported("glVertexAttribPointer");
	glVertexAttribPointer((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

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
		(GLsizei*)mxGetData(prhs[4]),
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
		(GLsizei*)mxGetData(prhs[4]),
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

void glu_begincurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginCurve) mogl_glunsupported("gluBeginCurve");
	gluBeginCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_beginpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginPolygon) mogl_glunsupported("gluBeginPolygon");
	gluBeginPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_beginsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginSurface) mogl_glunsupported("gluBeginSurface");
	gluBeginSurface((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_begintrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluBeginTrim) mogl_glunsupported("gluBeginTrim");
	gluBeginTrim((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

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
	gluCylinder((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void glu_deletenurbsrenderer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDeleteNurbsRenderer) mogl_glunsupported("gluDeleteNurbsRenderer");
	gluDeleteNurbsRenderer((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_deletequadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDeleteQuadric) mogl_glunsupported("gluDeleteQuadric");
	gluDeleteQuadric((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_deletetess( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDeleteTess) mogl_glunsupported("gluDeleteTess");
	gluDeleteTess((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_disk( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluDisk) mogl_glunsupported("gluDisk");
	gluDisk((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void glu_endcurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndCurve) mogl_glunsupported("gluEndCurve");
	gluEndCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndPolygon) mogl_glunsupported("gluEndPolygon");
	gluEndPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndSurface) mogl_glunsupported("gluEndSurface");
	gluEndSurface((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endtrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluEndTrim) mogl_glunsupported("gluEndTrim");
	gluEndTrim((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_getnurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluGetNurbsProperty) mogl_glunsupported("gluGetNurbsProperty");
	gluGetNurbsProperty((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void glu_gettessproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluGetTessProperty) mogl_glunsupported("gluGetTessProperty");
	gluGetTessProperty((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void glu_loadsamplingmatrices( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluLoadSamplingMatrices) mogl_glunsupported("gluLoadSamplingMatrices");
	gluLoadSamplingMatrices((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
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
	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewNurbsRenderer();

}

void glu_newquadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNewQuadric) mogl_glunsupported("gluNewQuadric");
	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewQuadric();

}

void glu_newtess( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNewTess) mogl_glunsupported("gluNewTess");
	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewTess();

}

void glu_nextcontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNextContour) mogl_glunsupported("gluNextContour");
	gluNextContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_nurbscurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsCurve) mogl_glunsupported("gluNurbsCurve");
	gluNurbsCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]));

}

void glu_nurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsProperty) mogl_glunsupported("gluNurbsProperty");
	gluNurbsProperty((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void glu_nurbssurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluNurbsSurface) mogl_glunsupported("gluNurbsSurface");
	gluNurbsSurface((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
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
	gluPartialDisk((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
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
	gluPwlCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void glu_quadricdrawstyle( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricDrawStyle) mogl_glunsupported("gluQuadricDrawStyle");
	gluQuadricDrawStyle((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricnormals( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricNormals) mogl_glunsupported("gluQuadricNormals");
	gluQuadricNormals((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricorientation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricOrientation) mogl_glunsupported("gluQuadricOrientation");
	gluQuadricOrientation((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadrictexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluQuadricTexture) mogl_glunsupported("gluQuadricTexture");
	gluQuadricTexture((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
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
	gluSphere((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glu_tessbegincontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessBeginContour) mogl_glunsupported("gluTessBeginContour");
	gluTessBeginContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessbeginpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessBeginPolygon) mogl_glunsupported("gluTessBeginPolygon");
	gluTessBeginPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLvoid*)mxGetData(prhs[1]));

}

void glu_tessendcontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessEndContour) mogl_glunsupported("gluTessEndContour");
	gluTessEndContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessendpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessEndPolygon) mogl_glunsupported("gluTessEndPolygon");
	gluTessEndPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessnormal( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessNormal) mogl_glunsupported("gluTessNormal");
	gluTessNormal((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_tessproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessProperty) mogl_glunsupported("gluTessProperty");
	gluTessProperty((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void glu_tessvertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == gluTessVertex) mogl_glunsupported("gluTessVertex");
	gluTessVertex((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]),
		(GLvoid*)mxGetData(prhs[2]));

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

void gl_areprogramsresidentnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glAreProgramsResidentNV) mogl_glunsupported("glAreProgramsResidentNV");
	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glAreProgramsResidentNV((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_requestresidentprogramsnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glRequestResidentProgramsNV) mogl_glunsupported("glRequestResidentProgramsNV");
	glRequestResidentProgramsNV((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getprogramparameterfvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramParameterfvNV) mogl_glunsupported("glGetProgramParameterfvNV");
	glGetProgramParameterfvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_getprogramparameterdvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glGetProgramParameterdvNV) mogl_glunsupported("glGetProgramParameterdvNV");
	glGetProgramParameterdvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLdouble*)mxGetData(prhs[3]));

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

void gl_programparameter4fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameter4fNV) mogl_glunsupported("glProgramParameter4fNV");
	glProgramParameter4fNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

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
		(GLuint)mxGetScalar(prhs[2]),
		(const GLdouble*)mxGetData(prhs[3]));

}

void gl_programparameters4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glProgramParameters4fvNV) mogl_glunsupported("glProgramParameters4fvNV");
	glProgramParameters4fvNV((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

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
		(const GLvoid*)mxGetData(prhs[4]));

}

void gl_vertexattrib1snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1sNV) mogl_glunsupported("glVertexAttrib1sNV");
	glVertexAttrib1sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fNV) mogl_glunsupported("glVertexAttrib1fNV");
	glVertexAttrib1fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dNV) mogl_glunsupported("glVertexAttrib1dNV");
	glVertexAttrib1dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattrib2snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2sNV) mogl_glunsupported("glVertexAttrib2sNV");
	glVertexAttrib2sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fNV) mogl_glunsupported("glVertexAttrib2fNV");
	glVertexAttrib2fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dNV) mogl_glunsupported("glVertexAttrib2dNV");
	glVertexAttrib2dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattrib3snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3sNV) mogl_glunsupported("glVertexAttrib3sNV");
	glVertexAttrib3sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fNV) mogl_glunsupported("glVertexAttrib3fNV");
	glVertexAttrib3fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dNV) mogl_glunsupported("glVertexAttrib3dNV");
	glVertexAttrib3dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattrib4snv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4sNV) mogl_glunsupported("glVertexAttrib4sNV");
	glVertexAttrib4sNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4fnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fNV) mogl_glunsupported("glVertexAttrib4fNV");
	glVertexAttrib4fNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4dnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dNV) mogl_glunsupported("glVertexAttrib4dNV");
	glVertexAttrib4dNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4ubnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubNV) mogl_glunsupported("glVertexAttrib4ubNV");
	glVertexAttrib4ubNV((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]));

}

void gl_vertexattrib1svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1svNV) mogl_glunsupported("glVertexAttrib1svNV");
	glVertexAttrib1svNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib1fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1fvNV) mogl_glunsupported("glVertexAttrib1fvNV");
	glVertexAttrib1fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib1dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib1dvNV) mogl_glunsupported("glVertexAttrib1dvNV");
	glVertexAttrib1dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib2svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2svNV) mogl_glunsupported("glVertexAttrib2svNV");
	glVertexAttrib2svNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib2fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2fvNV) mogl_glunsupported("glVertexAttrib2fvNV");
	glVertexAttrib2fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib2dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib2dvNV) mogl_glunsupported("glVertexAttrib2dvNV");
	glVertexAttrib2dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib3svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3svNV) mogl_glunsupported("glVertexAttrib3svNV");
	glVertexAttrib3svNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib3fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3fvNV) mogl_glunsupported("glVertexAttrib3fvNV");
	glVertexAttrib3fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib3dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib3dvNV) mogl_glunsupported("glVertexAttrib3dvNV");
	glVertexAttrib3dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4svNV) mogl_glunsupported("glVertexAttrib4svNV");
	glVertexAttrib4svNV((GLuint)mxGetScalar(prhs[0]),
		(GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4fvNV) mogl_glunsupported("glVertexAttrib4fvNV");
	glVertexAttrib4fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4dvNV) mogl_glunsupported("glVertexAttrib4dvNV");
	glVertexAttrib4dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ubvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttrib4ubvNV) mogl_glunsupported("glVertexAttrib4ubvNV");
	glVertexAttrib4ubvNV((GLuint)mxGetScalar(prhs[0]),
		(GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattribs1svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1svNV) mogl_glunsupported("glVertexAttribs1svNV");
	glVertexAttribs1svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs1fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1fvNV) mogl_glunsupported("glVertexAttribs1fvNV");
	glVertexAttribs1fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs1dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs1dvNV) mogl_glunsupported("glVertexAttribs1dvNV");
	glVertexAttribs1dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs2svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2svNV) mogl_glunsupported("glVertexAttribs2svNV");
	glVertexAttribs2svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs2fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2fvNV) mogl_glunsupported("glVertexAttribs2fvNV");
	glVertexAttribs2fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs2dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs2dvNV) mogl_glunsupported("glVertexAttribs2dvNV");
	glVertexAttribs2dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs3svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3svNV) mogl_glunsupported("glVertexAttribs3svNV");
	glVertexAttribs3svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs3fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3fvNV) mogl_glunsupported("glVertexAttribs3fvNV");
	glVertexAttribs3fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs3dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs3dvNV) mogl_glunsupported("glVertexAttribs3dvNV");
	glVertexAttribs3dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs4svnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4svNV) mogl_glunsupported("glVertexAttribs4svNV");
	glVertexAttribs4svNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLshort*)mxGetData(prhs[2]));

}

void gl_vertexattribs4fvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4fvNV) mogl_glunsupported("glVertexAttribs4fvNV");
	glVertexAttribs4fvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_vertexattribs4dvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4dvNV) mogl_glunsupported("glVertexAttribs4dvNV");
	glVertexAttribs4dvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_vertexattribs4ubvnv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glVertexAttribs4ubvNV) mogl_glunsupported("glVertexAttribs4ubvNV");
	glVertexAttribs4ubvNV((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLubyte*)mxGetData(prhs[2]));

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

void gl_samplemasksgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSampleMaskSGIS) mogl_glunsupported("glSampleMaskSGIS");
	glSampleMaskSGIS((GLclampf)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_samplepatternsgis( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	if (NULL == glSamplePatternSGIS) mogl_glunsupported("glSamplePatternSGIS");
	glSamplePatternSGIS((GLenum)mxGetScalar(prhs[0]));

}

int gl_auto_map_count=738;
cmdhandler gl_auto_map[] = {
{ "glAccum",                         gl_accum                            },
{ "glActiveTexture",                 gl_activetexture                    },
{ "glAlphaFunc",                     gl_alphafunc                        },
{ "glAreProgramsResidentNV",         gl_areprogramsresidentnv            },
{ "glAreTexturesResident",           gl_aretexturesresident              },
{ "glArrayElement",                  gl_arrayelement                     },
{ "glAttachShader",                  gl_attachshader                     },
{ "glBegin",                         gl_begin                            },
{ "glBeginQuery",                    gl_beginquery                       },
{ "glBindAttribLocation",            gl_bindattriblocation               },
{ "glBindBuffer",                    gl_bindbuffer                       },
{ "glBindFramebufferEXT",            gl_bindframebufferext               },
{ "glBindProgramNV",                 gl_bindprogramnv                    },
{ "glBindRenderbufferEXT",           gl_bindrenderbufferext              },
{ "glBindTexture",                   gl_bindtexture                      },
{ "glBitmap",                        gl_bitmap                           },
{ "glBlendColor",                    gl_blendcolor                       },
{ "glBlendEquation",                 gl_blendequation                    },
{ "glBlendEquationSeparate",         gl_blendequationseparate            },
{ "glBlendFunc",                     gl_blendfunc                        },
{ "glBlendFuncSeparate",             gl_blendfuncseparate                },
{ "glBufferData",                    gl_bufferdata                       },
{ "glBufferSubData",                 gl_buffersubdata                    },
{ "glCallList",                      gl_calllist                         },
{ "glCallLists",                     gl_calllists                        },
{ "glCheckFramebufferStatusEXT",     gl_checkframebufferstatusext        },
{ "glClear",                         gl_clear                            },
{ "glClearAccum",                    gl_clearaccum                       },
{ "glClearColor",                    gl_clearcolor                       },
{ "glClearDepth",                    gl_cleardepth                       },
{ "glClearIndex",                    gl_clearindex                       },
{ "glClearStencil",                  gl_clearstencil                     },
{ "glClientActiveTexture",           gl_clientactivetexture              },
{ "glClipPlane",                     gl_clipplane                        },
{ "glColor3b",                       gl_color3b                          },
{ "glColor3bv",                      gl_color3bv                         },
{ "glColor3d",                       gl_color3d                          },
{ "glColor3dv",                      gl_color3dv                         },
{ "glColor3f",                       gl_color3f                          },
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
{ "glColor4fv",                      gl_color4fv                         },
{ "glColor4i",                       gl_color4i                          },
{ "glColor4iv",                      gl_color4iv                         },
{ "glColor4s",                       gl_color4s                          },
{ "glColor4sv",                      gl_color4sv                         },
{ "glColor4ub",                      gl_color4ub                         },
{ "glColor4ubv",                     gl_color4ubv                        },
{ "glColor4ui",                      gl_color4ui                         },
{ "glColor4uiv",                     gl_color4uiv                        },
{ "glColor4us",                      gl_color4us                         },
{ "glColor4usv",                     gl_color4usv                        },
{ "glColorMask",                     gl_colormask                        },
{ "glColorMaterial",                 gl_colormaterial                    },
{ "glColorPointer",                  gl_colorpointer                     },
{ "glColorSubTable",                 gl_colorsubtable                    },
{ "glColorTable",                    gl_colortable                       },
{ "glColorTableParameterfv",         gl_colortableparameterfv            },
{ "glColorTableParameteriv",         gl_colortableparameteriv            },
{ "glCombinerInputNV",               gl_combinerinputnv                  },
{ "glCombinerOutputNV",              gl_combineroutputnv                 },
{ "glCombinerParameterfNV",          gl_combinerparameterfnv             },
{ "glCombinerParameterfvNV",         gl_combinerparameterfvnv            },
{ "glCombinerParameteriNV",          gl_combinerparameterinv             },
{ "glCombinerParameterivNV",         gl_combinerparameterivnv            },
{ "glCombinerStageParameterfvNV",    gl_combinerstageparameterfvnv       },
{ "glCompileShader",                 gl_compileshader                    },
{ "glCompressedTexImage1D",          gl_compressedteximage1d             },
{ "glCompressedTexImage2D",          gl_compressedteximage2d             },
{ "glCompressedTexImage3D",          gl_compressedteximage3d             },
{ "glCompressedTexSubImage1D",       gl_compressedtexsubimage1d          },
{ "glCompressedTexSubImage2D",       gl_compressedtexsubimage2d          },
{ "glCompressedTexSubImage3D",       gl_compressedtexsubimage3d          },
{ "glConvolutionFilter1D",           gl_convolutionfilter1d              },
{ "glConvolutionFilter2D",           gl_convolutionfilter2d              },
{ "glConvolutionParameterf",         gl_convolutionparameterf            },
{ "glConvolutionParameterfv",        gl_convolutionparameterfv           },
{ "glConvolutionParameteri",         gl_convolutionparameteri            },
{ "glConvolutionParameteriv",        gl_convolutionparameteriv           },
{ "glCopyColorSubTable",             gl_copycolorsubtable                },
{ "glCopyColorTable",                gl_copycolortable                   },
{ "glCopyConvolutionFilter1D",       gl_copyconvolutionfilter1d          },
{ "glCopyConvolutionFilter2D",       gl_copyconvolutionfilter2d          },
{ "glCopyPixels",                    gl_copypixels                       },
{ "glCopyTexImage1D",                gl_copyteximage1d                   },
{ "glCopyTexImage2D",                gl_copyteximage2d                   },
{ "glCopyTexSubImage1D",             gl_copytexsubimage1d                },
{ "glCopyTexSubImage2D",             gl_copytexsubimage2d                },
{ "glCopyTexSubImage3D",             gl_copytexsubimage3d                },
{ "glCreateProgram",                 gl_createprogram                    },
{ "glCreateShader",                  gl_createshader                     },
{ "glCullFace",                      gl_cullface                         },
{ "glDeleteBuffers",                 gl_deletebuffers                    },
{ "glDeleteFramebuffersEXT",         gl_deleteframebuffersext            },
{ "glDeleteLists",                   gl_deletelists                      },
{ "glDeleteProgram",                 gl_deleteprogram                    },
{ "glDeleteProgramsNV",              gl_deleteprogramsnv                 },
{ "glDeleteQueries",                 gl_deletequeries                    },
{ "glDeleteRenderbuffersEXT",        gl_deleterenderbuffersext           },
{ "glDeleteShader",                  gl_deleteshader                     },
{ "glDeleteTextures",                gl_deletetextures                   },
{ "glDepthFunc",                     gl_depthfunc                        },
{ "glDepthMask",                     gl_depthmask                        },
{ "glDepthRange",                    gl_depthrange                       },
{ "glDetachShader",                  gl_detachshader                     },
{ "glDisable",                       gl_disable                          },
{ "glDisableClientState",            gl_disableclientstate               },
{ "glDisableVertexAttribArray",      gl_disablevertexattribarray         },
{ "glDrawArrays",                    gl_drawarrays                       },
{ "glDrawBuffer",                    gl_drawbuffer                       },
{ "glDrawBuffers",                   gl_drawbuffers                      },
{ "glDrawElements",                  gl_drawelements                     },
{ "glDrawPixels",                    gl_drawpixels                       },
{ "glDrawRangeElements",             gl_drawrangeelements                },
{ "glEdgeFlag",                      gl_edgeflag                         },
{ "glEdgeFlagPointer",               gl_edgeflagpointer                  },
{ "glEdgeFlagv",                     gl_edgeflagv                        },
{ "glEnable",                        gl_enable                           },
{ "glEnableClientState",             gl_enableclientstate                },
{ "glEnableVertexAttribArray",       gl_enablevertexattribarray          },
{ "glEnd",                           gl_end                              },
{ "glEndList",                       gl_endlist                          },
{ "glEndQuery",                      gl_endquery                         },
{ "glEvalCoord1d",                   gl_evalcoord1d                      },
{ "glEvalCoord1dv",                  gl_evalcoord1dv                     },
{ "glEvalCoord1f",                   gl_evalcoord1f                      },
{ "glEvalCoord1fv",                  gl_evalcoord1fv                     },
{ "glEvalCoord2d",                   gl_evalcoord2d                      },
{ "glEvalCoord2dv",                  gl_evalcoord2dv                     },
{ "glEvalCoord2f",                   gl_evalcoord2f                      },
{ "glEvalCoord2fv",                  gl_evalcoord2fv                     },
{ "glEvalMesh1",                     gl_evalmesh1                        },
{ "glEvalMesh2",                     gl_evalmesh2                        },
{ "glEvalPoint1",                    gl_evalpoint1                       },
{ "glEvalPoint2",                    gl_evalpoint2                       },
{ "glExecuteProgramNV",              gl_executeprogramnv                 },
{ "glFinalCombinerInputNV",          gl_finalcombinerinputnv             },
{ "glFinish",                        gl_finish                           },
{ "glFlush",                         gl_flush                            },
{ "glFogCoordPointer",               gl_fogcoordpointer                  },
{ "glFogCoordd",                     gl_fogcoordd                        },
{ "glFogCoorddv",                    gl_fogcoorddv                       },
{ "glFogCoordf",                     gl_fogcoordf                        },
{ "glFogCoordfv",                    gl_fogcoordfv                       },
{ "glFogf",                          gl_fogf                             },
{ "glFogfv",                         gl_fogfv                            },
{ "glFogi",                          gl_fogi                             },
{ "glFogiv",                         gl_fogiv                            },
{ "glFramebufferRenderbufferEXT",    gl_framebufferrenderbufferext       },
{ "glFramebufferTexture1DEXT",       gl_framebuffertexture1dext          },
{ "glFramebufferTexture2DEXT",       gl_framebuffertexture2dext          },
{ "glFramebufferTexture3DEXT",       gl_framebuffertexture3dext          },
{ "glFrontFace",                     gl_frontface                        },
{ "glFrustum",                       gl_frustum                          },
{ "glGenBuffers",                    gl_genbuffers                       },
{ "glGenFramebuffersEXT",            gl_genframebuffersext               },
{ "glGenLists",                      gl_genlists                         },
{ "glGenProgramsNV",                 gl_genprogramsnv                    },
{ "glGenQueries",                    gl_genqueries                       },
{ "glGenRenderbuffersEXT",           gl_genrenderbuffersext              },
{ "glGenTextures",                   gl_gentextures                      },
{ "glGenerateMipmapEXT",             gl_generatemipmapext                },
{ "glGetActiveAttrib",               gl_getactiveattrib                  },
{ "glGetActiveUniform",              gl_getactiveuniform                 },
{ "glGetAttachedShaders",            gl_getattachedshaders               },
{ "glGetAttribLocation",             gl_getattriblocation                },
{ "glGetBooleanv",                   gl_getbooleanv                      },
{ "glGetBufferParameteriv",          gl_getbufferparameteriv             },
{ "glGetBufferSubData",              gl_getbuffersubdata                 },
{ "glGetClipPlane",                  gl_getclipplane                     },
{ "glGetColorTable",                 gl_getcolortable                    },
{ "glGetColorTableParameterfv",      gl_getcolortableparameterfv         },
{ "glGetColorTableParameteriv",      gl_getcolortableparameteriv         },
{ "glGetCombinerInputParameterfvNV", gl_getcombinerinputparameterfvnv    },
{ "glGetCombinerInputParameterivNV", gl_getcombinerinputparameterivnv    },
{ "glGetCombinerOutputParameterfvNV",gl_getcombineroutputparameterfvnv   },
{ "glGetCombinerOutputParameterivNV",gl_getcombineroutputparameterivnv   },
{ "glGetCombinerStageParameterfvNV", gl_getcombinerstageparameterfvnv    },
{ "glGetCompressedTexImage",         gl_getcompressedteximage            },
{ "glGetConvolutionFilter",          gl_getconvolutionfilter             },
{ "glGetConvolutionParameterfv",     gl_getconvolutionparameterfv        },
{ "glGetConvolutionParameteriv",     gl_getconvolutionparameteriv        },
{ "glGetDoublev",                    gl_getdoublev                       },
{ "glGetError",                      gl_geterror                         },
{ "glGetFinalCombinerInputParameterfvNV",gl_getfinalcombinerinputparameterfvnv },
{ "glGetFinalCombinerInputParameterivNV",gl_getfinalcombinerinputparameterivnv },
{ "glGetFloatv",                     gl_getfloatv                        },
{ "glGetFramebufferAttachmentParameterivEXT",gl_getframebufferattachmentparameterivext },
{ "glGetHistogram",                  gl_gethistogram                     },
{ "glGetHistogramParameterfv",       gl_gethistogramparameterfv          },
{ "glGetHistogramParameteriv",       gl_gethistogramparameteriv          },
{ "glGetIntegerv",                   gl_getintegerv                      },
{ "glGetLightfv",                    gl_getlightfv                       },
{ "glGetLightiv",                    gl_getlightiv                       },
{ "glGetMapdv",                      gl_getmapdv                         },
{ "glGetMapfv",                      gl_getmapfv                         },
{ "glGetMapiv",                      gl_getmapiv                         },
{ "glGetMaterialfv",                 gl_getmaterialfv                    },
{ "glGetMaterialiv",                 gl_getmaterialiv                    },
{ "glGetMinmax",                     gl_getminmax                        },
{ "glGetMinmaxParameterfv",          gl_getminmaxparameterfv             },
{ "glGetMinmaxParameteriv",          gl_getminmaxparameteriv             },
{ "glGetPixelMapfv",                 gl_getpixelmapfv                    },
{ "glGetPixelMapuiv",                gl_getpixelmapuiv                   },
{ "glGetPixelMapusv",                gl_getpixelmapusv                   },
{ "glGetPolygonStipple",             gl_getpolygonstipple                },
{ "glGetProgramInfoLog",             gl_getprograminfolog                },
{ "glGetProgramParameterdvNV",       gl_getprogramparameterdvnv          },
{ "glGetProgramParameterfvNV",       gl_getprogramparameterfvnv          },
{ "glGetProgramStringNV",            gl_getprogramstringnv               },
{ "glGetProgramiv",                  gl_getprogramiv                     },
{ "glGetProgramivNV",                gl_getprogramivnv                   },
{ "glGetQueryObjectiv",              gl_getqueryobjectiv                 },
{ "glGetQueryObjectuiv",             gl_getqueryobjectuiv                },
{ "glGetQueryiv",                    gl_getqueryiv                       },
{ "glGetRenderbufferParameterivEXT", gl_getrenderbufferparameterivext    },
{ "glGetSeparableFilter",            gl_getseparablefilter               },
{ "glGetShaderInfoLog",              gl_getshaderinfolog                 },
{ "glGetShaderSource",               gl_getshadersource                  },
{ "glGetShaderiv",                   gl_getshaderiv                      },
{ "glGetTexEnvfv",                   gl_gettexenvfv                      },
{ "glGetTexEnviv",                   gl_gettexenviv                      },
{ "glGetTexGendv",                   gl_gettexgendv                      },
{ "glGetTexGenfv",                   gl_gettexgenfv                      },
{ "glGetTexGeniv",                   gl_gettexgeniv                      },
{ "glGetTexImage",                   gl_getteximage                      },
{ "glGetTexLevelParameterfv",        gl_gettexlevelparameterfv           },
{ "glGetTexLevelParameteriv",        gl_gettexlevelparameteriv           },
{ "glGetTexParameterfv",             gl_gettexparameterfv                },
{ "glGetTexParameteriv",             gl_gettexparameteriv                },
{ "glGetTrackMatrixivNV",            gl_gettrackmatrixivnv               },
{ "glGetUniformLocation",            gl_getuniformlocation               },
{ "glGetUniformfv",                  gl_getuniformfv                     },
{ "glGetUniformiv",                  gl_getuniformiv                     },
{ "glGetVertexAttribdv",             gl_getvertexattribdv                },
{ "glGetVertexAttribdvNV",           gl_getvertexattribdvnv              },
{ "glGetVertexAttribfv",             gl_getvertexattribfv                },
{ "glGetVertexAttribfvNV",           gl_getvertexattribfvnv              },
{ "glGetVertexAttribiv",             gl_getvertexattribiv                },
{ "glGetVertexAttribivNV",           gl_getvertexattribivnv              },
{ "glHint",                          gl_hint                             },
{ "glHistogram",                     gl_histogram                        },
{ "glIndexMask",                     gl_indexmask                        },
{ "glIndexPointer",                  gl_indexpointer                     },
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
{ "glInterleavedArrays",             gl_interleavedarrays                },
{ "glIsBuffer",                      gl_isbuffer                         },
{ "glIsEnabled",                     gl_isenabled                        },
{ "glIsFramebufferEXT",              gl_isframebufferext                 },
{ "glIsList",                        gl_islist                           },
{ "glIsProgram",                     gl_isprogram                        },
{ "glIsProgramNV",                   gl_isprogramnv                      },
{ "glIsQuery",                       gl_isquery                          },
{ "glIsRenderbufferEXT",             gl_isrenderbufferext                },
{ "glIsShader",                      gl_isshader                         },
{ "glIsTexture",                     gl_istexture                        },
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
{ "glListBase",                      gl_listbase                         },
{ "glLoadIdentity",                  gl_loadidentity                     },
{ "glLoadMatrixd",                   gl_loadmatrixd                      },
{ "glLoadMatrixf",                   gl_loadmatrixf                      },
{ "glLoadName",                      gl_loadname                         },
{ "glLoadProgramNV",                 gl_loadprogramnv                    },
{ "glLoadTransposeMatrixd",          gl_loadtransposematrixd             },
{ "glLoadTransposeMatrixf",          gl_loadtransposematrixf             },
{ "glLogicOp",                       gl_logicop                          },
{ "glMap1d",                         gl_map1d                            },
{ "glMap1f",                         gl_map1f                            },
{ "glMap2d",                         gl_map2d                            },
{ "glMap2f",                         gl_map2f                            },
{ "glMapBuffer",                     gl_mapbuffer                        },
{ "glMapGrid1d",                     gl_mapgrid1d                        },
{ "glMapGrid1f",                     gl_mapgrid1f                        },
{ "glMapGrid2d",                     gl_mapgrid2d                        },
{ "glMapGrid2f",                     gl_mapgrid2f                        },
{ "glMaterialf",                     gl_materialf                        },
{ "glMaterialfv",                    gl_materialfv                       },
{ "glMateriali",                     gl_materiali                        },
{ "glMaterialiv",                    gl_materialiv                       },
{ "glMatrixMode",                    gl_matrixmode                       },
{ "glMinmax",                        gl_minmax                           },
{ "glMultMatrixd",                   gl_multmatrixd                      },
{ "glMultMatrixf",                   gl_multmatrixf                      },
{ "glMultTransposeMatrixd",          gl_multtransposematrixd             },
{ "glMultTransposeMatrixf",          gl_multtransposematrixf             },
{ "glMultiDrawArrays",               gl_multidrawarrays                  },
{ "glMultiTexCoord1d",               gl_multitexcoord1d                  },
{ "glMultiTexCoord1dv",              gl_multitexcoord1dv                 },
{ "glMultiTexCoord1f",               gl_multitexcoord1f                  },
{ "glMultiTexCoord1fv",              gl_multitexcoord1fv                 },
{ "glMultiTexCoord1i",               gl_multitexcoord1i                  },
{ "glMultiTexCoord1iv",              gl_multitexcoord1iv                 },
{ "glMultiTexCoord1s",               gl_multitexcoord1s                  },
{ "glMultiTexCoord1sv",              gl_multitexcoord1sv                 },
{ "glMultiTexCoord2d",               gl_multitexcoord2d                  },
{ "glMultiTexCoord2dv",              gl_multitexcoord2dv                 },
{ "glMultiTexCoord2f",               gl_multitexcoord2f                  },
{ "glMultiTexCoord2fv",              gl_multitexcoord2fv                 },
{ "glMultiTexCoord2i",               gl_multitexcoord2i                  },
{ "glMultiTexCoord2iv",              gl_multitexcoord2iv                 },
{ "glMultiTexCoord2s",               gl_multitexcoord2s                  },
{ "glMultiTexCoord2sv",              gl_multitexcoord2sv                 },
{ "glMultiTexCoord3d",               gl_multitexcoord3d                  },
{ "glMultiTexCoord3dv",              gl_multitexcoord3dv                 },
{ "glMultiTexCoord3f",               gl_multitexcoord3f                  },
{ "glMultiTexCoord3fv",              gl_multitexcoord3fv                 },
{ "glMultiTexCoord3i",               gl_multitexcoord3i                  },
{ "glMultiTexCoord3iv",              gl_multitexcoord3iv                 },
{ "glMultiTexCoord3s",               gl_multitexcoord3s                  },
{ "glMultiTexCoord3sv",              gl_multitexcoord3sv                 },
{ "glMultiTexCoord4d",               gl_multitexcoord4d                  },
{ "glMultiTexCoord4dv",              gl_multitexcoord4dv                 },
{ "glMultiTexCoord4f",               gl_multitexcoord4f                  },
{ "glMultiTexCoord4fv",              gl_multitexcoord4fv                 },
{ "glMultiTexCoord4i",               gl_multitexcoord4i                  },
{ "glMultiTexCoord4iv",              gl_multitexcoord4iv                 },
{ "glMultiTexCoord4s",               gl_multitexcoord4s                  },
{ "glMultiTexCoord4sv",              gl_multitexcoord4sv                 },
{ "glNewList",                       gl_newlist                          },
{ "glNormal3b",                      gl_normal3b                         },
{ "glNormal3bv",                     gl_normal3bv                        },
{ "glNormal3d",                      gl_normal3d                         },
{ "glNormal3dv",                     gl_normal3dv                        },
{ "glNormal3f",                      gl_normal3f                         },
{ "glNormal3fv",                     gl_normal3fv                        },
{ "glNormal3i",                      gl_normal3i                         },
{ "glNormal3iv",                     gl_normal3iv                        },
{ "glNormal3s",                      gl_normal3s                         },
{ "glNormal3sv",                     gl_normal3sv                        },
{ "glNormalPointer",                 gl_normalpointer                    },
{ "glOrtho",                         gl_ortho                            },
{ "glPassThrough",                   gl_passthrough                      },
{ "glPixelMapfv",                    gl_pixelmapfv                       },
{ "glPixelMapuiv",                   gl_pixelmapuiv                      },
{ "glPixelMapusv",                   gl_pixelmapusv                      },
{ "glPixelStoref",                   gl_pixelstoref                      },
{ "glPixelStorei",                   gl_pixelstorei                      },
{ "glPixelTransferf",                gl_pixeltransferf                   },
{ "glPixelTransferi",                gl_pixeltransferi                   },
{ "glPixelZoom",                     gl_pixelzoom                        },
{ "glPointParameterf",               gl_pointparameterf                  },
{ "glPointParameterfv",              gl_pointparameterfv                 },
{ "glPointParameteri",               gl_pointparameteri                  },
{ "glPointParameteriNV",             gl_pointparameterinv                },
{ "glPointParameteriv",              gl_pointparameteriv                 },
{ "glPointParameterivNV",            gl_pointparameterivnv               },
{ "glPointSize",                     gl_pointsize                        },
{ "glPolygonMode",                   gl_polygonmode                      },
{ "glPolygonOffset",                 gl_polygonoffset                    },
{ "glPolygonStipple",                gl_polygonstipple                   },
{ "glPopAttrib",                     gl_popattrib                        },
{ "glPopClientAttrib",               gl_popclientattrib                  },
{ "glPopMatrix",                     gl_popmatrix                        },
{ "glPopName",                       gl_popname                          },
{ "glPrioritizeTextures",            gl_prioritizetextures               },
{ "glProgramParameter4dNV",          gl_programparameter4dnv             },
{ "glProgramParameter4dvNV",         gl_programparameter4dvnv            },
{ "glProgramParameter4fNV",          gl_programparameter4fnv             },
{ "glProgramParameter4fvNV",         gl_programparameter4fvnv            },
{ "glProgramParameters4dvNV",        gl_programparameters4dvnv           },
{ "glProgramParameters4fvNV",        gl_programparameters4fvnv           },
{ "glPushAttrib",                    gl_pushattrib                       },
{ "glPushClientAttrib",              gl_pushclientattrib                 },
{ "glPushMatrix",                    gl_pushmatrix                       },
{ "glPushName",                      gl_pushname                         },
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
{ "glReadPixels",                    gl_readpixels                       },
{ "glRectd",                         gl_rectd                            },
{ "glRectdv",                        gl_rectdv                           },
{ "glRectf",                         gl_rectf                            },
{ "glRectfv",                        gl_rectfv                           },
{ "glRecti",                         gl_recti                            },
{ "glRectiv",                        gl_rectiv                           },
{ "glRects",                         gl_rects                            },
{ "glRectsv",                        gl_rectsv                           },
{ "glRenderMode",                    gl_rendermode                       },
{ "glRenderbufferStorageEXT",        gl_renderbufferstorageext           },
{ "glRequestResidentProgramsNV",     gl_requestresidentprogramsnv        },
{ "glResetHistogram",                gl_resethistogram                   },
{ "glResetMinmax",                   gl_resetminmax                      },
{ "glRotated",                       gl_rotated                          },
{ "glRotatef",                       gl_rotatef                          },
{ "glSampleCoverage",                gl_samplecoverage                   },
{ "glSampleMaskSGIS",                gl_samplemasksgis                   },
{ "glSamplePatternSGIS",             gl_samplepatternsgis                },
{ "glScaled",                        gl_scaled                           },
{ "glScalef",                        gl_scalef                           },
{ "glScissor",                       gl_scissor                          },
{ "glSecondaryColor3b",              gl_secondarycolor3b                 },
{ "glSecondaryColor3bv",             gl_secondarycolor3bv                },
{ "glSecondaryColor3d",              gl_secondarycolor3d                 },
{ "glSecondaryColor3dv",             gl_secondarycolor3dv                },
{ "glSecondaryColor3f",              gl_secondarycolor3f                 },
{ "glSecondaryColor3fv",             gl_secondarycolor3fv                },
{ "glSecondaryColor3i",              gl_secondarycolor3i                 },
{ "glSecondaryColor3iv",             gl_secondarycolor3iv                },
{ "glSecondaryColor3s",              gl_secondarycolor3s                 },
{ "glSecondaryColor3sv",             gl_secondarycolor3sv                },
{ "glSecondaryColor3ub",             gl_secondarycolor3ub                },
{ "glSecondaryColor3ubv",            gl_secondarycolor3ubv               },
{ "glSecondaryColor3ui",             gl_secondarycolor3ui                },
{ "glSecondaryColor3uiv",            gl_secondarycolor3uiv               },
{ "glSecondaryColor3us",             gl_secondarycolor3us                },
{ "glSecondaryColor3usv",            gl_secondarycolor3usv               },
{ "glSecondaryColorPointer",         gl_secondarycolorpointer            },
{ "glSeparableFilter2D",             gl_separablefilter2d                },
{ "glShadeModel",                    gl_shademodel                       },
{ "glStencilFunc",                   gl_stencilfunc                      },
{ "glStencilFuncSeparate",           gl_stencilfuncseparate              },
{ "glStencilMask",                   gl_stencilmask                      },
{ "glStencilMaskSeparate",           gl_stencilmaskseparate              },
{ "glStencilOp",                     gl_stencilop                        },
{ "glStencilOpSeparate",             gl_stencilopseparate                },
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
{ "glTexCoord4fv",                   gl_texcoord4fv                      },
{ "glTexCoord4i",                    gl_texcoord4i                       },
{ "glTexCoord4iv",                   gl_texcoord4iv                      },
{ "glTexCoord4s",                    gl_texcoord4s                       },
{ "glTexCoord4sv",                   gl_texcoord4sv                      },
{ "glTexCoordPointer",               gl_texcoordpointer                  },
{ "glTexEnvf",                       gl_texenvf                          },
{ "glTexEnvfv",                      gl_texenvfv                         },
{ "glTexEnvi",                       gl_texenvi                          },
{ "glTexEnviv",                      gl_texenviv                         },
{ "glTexGend",                       gl_texgend                          },
{ "glTexGendv",                      gl_texgendv                         },
{ "glTexGenf",                       gl_texgenf                          },
{ "glTexGenfv",                      gl_texgenfv                         },
{ "glTexGeni",                       gl_texgeni                          },
{ "glTexGeniv",                      gl_texgeniv                         },
{ "glTexImage1D",                    gl_teximage1d                       },
{ "glTexImage2D",                    gl_teximage2d                       },
{ "glTexImage3D",                    gl_teximage3d                       },
{ "glTexParameterf",                 gl_texparameterf                    },
{ "glTexParameterfv",                gl_texparameterfv                   },
{ "glTexParameteri",                 gl_texparameteri                    },
{ "glTexParameteriv",                gl_texparameteriv                   },
{ "glTexSubImage1D",                 gl_texsubimage1d                    },
{ "glTexSubImage2D",                 gl_texsubimage2d                    },
{ "glTexSubImage3D",                 gl_texsubimage3d                    },
{ "glTrackMatrixNV",                 gl_trackmatrixnv                    },
{ "glTranslated",                    gl_translated                       },
{ "glTranslatef",                    gl_translatef                       },
{ "glUniform1f",                     gl_uniform1f                        },
{ "glUniform1fv",                    gl_uniform1fv                       },
{ "glUniform1i",                     gl_uniform1i                        },
{ "glUniform1iv",                    gl_uniform1iv                       },
{ "glUniform2f",                     gl_uniform2f                        },
{ "glUniform2fv",                    gl_uniform2fv                       },
{ "glUniform2i",                     gl_uniform2i                        },
{ "glUniform2iv",                    gl_uniform2iv                       },
{ "glUniform3f",                     gl_uniform3f                        },
{ "glUniform3fv",                    gl_uniform3fv                       },
{ "glUniform3i",                     gl_uniform3i                        },
{ "glUniform3iv",                    gl_uniform3iv                       },
{ "glUniform4f",                     gl_uniform4f                        },
{ "glUniform4fv",                    gl_uniform4fv                       },
{ "glUniform4i",                     gl_uniform4i                        },
{ "glUniform4iv",                    gl_uniform4iv                       },
{ "glUniformMatrix2fv",              gl_uniformmatrix2fv                 },
{ "glUniformMatrix3fv",              gl_uniformmatrix3fv                 },
{ "glUniformMatrix4fv",              gl_uniformmatrix4fv                 },
{ "glUnmapBuffer",                   gl_unmapbuffer                      },
{ "glUseProgram",                    gl_useprogram                       },
{ "glValidateProgram",               gl_validateprogram                  },
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
{ "glVertexAttrib1d",                gl_vertexattrib1d                   },
{ "glVertexAttrib1dNV",              gl_vertexattrib1dnv                 },
{ "glVertexAttrib1dv",               gl_vertexattrib1dv                  },
{ "glVertexAttrib1dvNV",             gl_vertexattrib1dvnv                },
{ "glVertexAttrib1f",                gl_vertexattrib1f                   },
{ "glVertexAttrib1fNV",              gl_vertexattrib1fnv                 },
{ "glVertexAttrib1fv",               gl_vertexattrib1fv                  },
{ "glVertexAttrib1fvNV",             gl_vertexattrib1fvnv                },
{ "glVertexAttrib1s",                gl_vertexattrib1s                   },
{ "glVertexAttrib1sNV",              gl_vertexattrib1snv                 },
{ "glVertexAttrib1sv",               gl_vertexattrib1sv                  },
{ "glVertexAttrib1svNV",             gl_vertexattrib1svnv                },
{ "glVertexAttrib2d",                gl_vertexattrib2d                   },
{ "glVertexAttrib2dNV",              gl_vertexattrib2dnv                 },
{ "glVertexAttrib2dv",               gl_vertexattrib2dv                  },
{ "glVertexAttrib2dvNV",             gl_vertexattrib2dvnv                },
{ "glVertexAttrib2f",                gl_vertexattrib2f                   },
{ "glVertexAttrib2fNV",              gl_vertexattrib2fnv                 },
{ "glVertexAttrib2fv",               gl_vertexattrib2fv                  },
{ "glVertexAttrib2fvNV",             gl_vertexattrib2fvnv                },
{ "glVertexAttrib2s",                gl_vertexattrib2s                   },
{ "glVertexAttrib2sNV",              gl_vertexattrib2snv                 },
{ "glVertexAttrib2sv",               gl_vertexattrib2sv                  },
{ "glVertexAttrib2svNV",             gl_vertexattrib2svnv                },
{ "glVertexAttrib3d",                gl_vertexattrib3d                   },
{ "glVertexAttrib3dNV",              gl_vertexattrib3dnv                 },
{ "glVertexAttrib3dv",               gl_vertexattrib3dv                  },
{ "glVertexAttrib3dvNV",             gl_vertexattrib3dvnv                },
{ "glVertexAttrib3f",                gl_vertexattrib3f                   },
{ "glVertexAttrib3fNV",              gl_vertexattrib3fnv                 },
{ "glVertexAttrib3fv",               gl_vertexattrib3fv                  },
{ "glVertexAttrib3fvNV",             gl_vertexattrib3fvnv                },
{ "glVertexAttrib3s",                gl_vertexattrib3s                   },
{ "glVertexAttrib3sNV",              gl_vertexattrib3snv                 },
{ "glVertexAttrib3sv",               gl_vertexattrib3sv                  },
{ "glVertexAttrib3svNV",             gl_vertexattrib3svnv                },
{ "glVertexAttrib4Nbv",              gl_vertexattrib4nbv                 },
{ "glVertexAttrib4Niv",              gl_vertexattrib4niv                 },
{ "glVertexAttrib4Nsv",              gl_vertexattrib4nsv                 },
{ "glVertexAttrib4Nub",              gl_vertexattrib4nub                 },
{ "glVertexAttrib4Nubv",             gl_vertexattrib4nubv                },
{ "glVertexAttrib4Nuiv",             gl_vertexattrib4nuiv                },
{ "glVertexAttrib4Nusv",             gl_vertexattrib4nusv                },
{ "glVertexAttrib4bv",               gl_vertexattrib4bv                  },
{ "glVertexAttrib4d",                gl_vertexattrib4d                   },
{ "glVertexAttrib4dNV",              gl_vertexattrib4dnv                 },
{ "glVertexAttrib4dv",               gl_vertexattrib4dv                  },
{ "glVertexAttrib4dvNV",             gl_vertexattrib4dvnv                },
{ "glVertexAttrib4f",                gl_vertexattrib4f                   },
{ "glVertexAttrib4fNV",              gl_vertexattrib4fnv                 },
{ "glVertexAttrib4fv",               gl_vertexattrib4fv                  },
{ "glVertexAttrib4fvNV",             gl_vertexattrib4fvnv                },
{ "glVertexAttrib4iv",               gl_vertexattrib4iv                  },
{ "glVertexAttrib4s",                gl_vertexattrib4s                   },
{ "glVertexAttrib4sNV",              gl_vertexattrib4snv                 },
{ "glVertexAttrib4sv",               gl_vertexattrib4sv                  },
{ "glVertexAttrib4svNV",             gl_vertexattrib4svnv                },
{ "glVertexAttrib4ubNV",             gl_vertexattrib4ubnv                },
{ "glVertexAttrib4ubv",              gl_vertexattrib4ubv                 },
{ "glVertexAttrib4ubvNV",            gl_vertexattrib4ubvnv               },
{ "glVertexAttrib4uiv",              gl_vertexattrib4uiv                 },
{ "glVertexAttrib4usv",              gl_vertexattrib4usv                 },
{ "glVertexAttribPointer",           gl_vertexattribpointer              },
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
{ "glVertexPointer",                 gl_vertexpointer                    },
{ "glViewport",                      gl_viewport                         },
{ "glWindowPos2d",                   gl_windowpos2d                      },
{ "glWindowPos2dv",                  gl_windowpos2dv                     },
{ "glWindowPos2f",                   gl_windowpos2f                      },
{ "glWindowPos2fv",                  gl_windowpos2fv                     },
{ "glWindowPos2i",                   gl_windowpos2i                      },
{ "glWindowPos2iv",                  gl_windowpos2iv                     },
{ "glWindowPos2s",                   gl_windowpos2s                      },
{ "glWindowPos2sv",                  gl_windowpos2sv                     },
{ "glWindowPos3d",                   gl_windowpos3d                      },
{ "glWindowPos3dv",                  gl_windowpos3dv                     },
{ "glWindowPos3f",                   gl_windowpos3f                      },
{ "glWindowPos3fv",                  gl_windowpos3fv                     },
{ "glWindowPos3i",                   gl_windowpos3i                      },
{ "glWindowPos3iv",                  gl_windowpos3iv                     },
{ "glWindowPos3s",                   gl_windowpos3s                      },
{ "glWindowPos3sv",                  gl_windowpos3sv                     },
{ "gluBeginCurve",                   glu_begincurve                      },
{ "gluBeginPolygon",                 glu_beginpolygon                    },
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
{ "gluDeleteTess",                   glu_deletetess                      },
{ "gluDisk",                         glu_disk                            },
{ "gluEndCurve",                     glu_endcurve                        },
{ "gluEndPolygon",                   glu_endpolygon                      },
{ "gluEndSurface",                   glu_endsurface                      },
{ "gluEndTrim",                      glu_endtrim                         },
{ "gluGetNurbsProperty",             glu_getnurbsproperty                },
{ "gluGetTessProperty",              glu_gettessproperty                 },
{ "gluLoadSamplingMatrices",         glu_loadsamplingmatrices            },
{ "gluLookAt",                       glu_lookat                          },
{ "gluNewNurbsRenderer",             glu_newnurbsrenderer                },
{ "gluNewQuadric",                   glu_newquadric                      },
{ "gluNewTess",                      glu_newtess                         },
{ "gluNextContour",                  glu_nextcontour                     },
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
{ "gluTessBeginContour",             glu_tessbegincontour                },
{ "gluTessBeginPolygon",             glu_tessbeginpolygon                },
{ "gluTessEndContour",               glu_tessendcontour                  },
{ "gluTessEndPolygon",               glu_tessendpolygon                  },
{ "gluTessNormal",                   glu_tessnormal                      },
{ "gluTessProperty",                 glu_tessproperty                    },
{ "gluTessVertex",                   glu_tessvertex                      },
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
