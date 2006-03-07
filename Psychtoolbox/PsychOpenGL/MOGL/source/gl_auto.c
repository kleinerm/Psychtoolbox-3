
/*
 * gl_auto.c
 *
 * 05-Mar-2006 -- created (moglgen)
 *
 */

#include "mogltypes.h"

void gl_accum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glAccum((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_alphafunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glAlphaFunc((GLenum)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]));

}

void gl_aretexturesresident( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glAreTexturesResident((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(GLboolean*)mxGetData(prhs[2]));

}

void gl_arrayelement( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glArrayElement((GLint)mxGetScalar(prhs[0]));

}

void gl_begin( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBegin((GLenum)mxGetScalar(prhs[0]));

}

void gl_bindtexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBindTexture((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_bitmap( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBitmap((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]),
		(const GLubyte*)mxGetData(prhs[6]));

}

void gl_blendcolor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBlendColor((GLclampf)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]),
		(GLclampf)mxGetScalar(prhs[2]),
		(GLclampf)mxGetScalar(prhs[3]));

}

void gl_blendequation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBlendEquation((GLenum)mxGetScalar(prhs[0]));

}

void gl_blendequationseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBlendEquationSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_blendfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBlendFunc((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_calllist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCallList((GLuint)mxGetScalar(prhs[0]));

}

void gl_calllists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCallLists((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_clear( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClear((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_clearaccum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClearAccum((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_clearcolor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClearColor((GLclampf)mxGetScalar(prhs[0]),
		(GLclampf)mxGetScalar(prhs[1]),
		(GLclampf)mxGetScalar(prhs[2]),
		(GLclampf)mxGetScalar(prhs[3]));

}

void gl_cleardepth( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClearDepth((GLclampd)mxGetScalar(prhs[0]));

}

void gl_clearindex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClearIndex((GLfloat)mxGetScalar(prhs[0]));

}

void gl_clearstencil( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClearStencil((GLint)mxGetScalar(prhs[0]));

}

void gl_clipplane( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClipPlane((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_color3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_color3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_color3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_color3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_color3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_color3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_color3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_color3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_color3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_color3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_color3ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]));

}

void gl_color3ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_color3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_color3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_color3us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]));

}

void gl_color3usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor3usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_color4b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]),
		(GLbyte)mxGetScalar(prhs[3]));

}

void gl_color4bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_color4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_color4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_color4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_color4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_color4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_color4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_color4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_color4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_color4ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]));

}

void gl_color4ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_color4ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_color4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_color4us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]),
		(GLushort)mxGetScalar(prhs[3]));

}

void gl_color4usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColor4usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_colormask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorMask((GLboolean)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_colormaterial( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorMaterial((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_colorpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_colorsubtable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorSubTable((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_colortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_colortableparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorTableParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_colortableparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glColorTableParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_convolutionfilter1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionFilter1D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_convolutionfilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionFilter2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_convolutionparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_convolutionparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_convolutionparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_convolutionparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glConvolutionParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_copycolorsubtable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyColorSubTable((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copycolortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copyconvolutionfilter1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyConvolutionFilter1D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]));

}

void gl_copyconvolutionfilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyConvolutionFilter2D((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copypixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyPixels((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void gl_copyteximage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCopyTexImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(GLint)mxGetScalar(prhs[6]));

}

void gl_copyteximage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glCopyTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]));

}

void gl_copytexsubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glCullFace((GLenum)mxGetScalar(prhs[0]));

}

void gl_deletelists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteLists((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]));

}

void gl_deletetextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteTextures((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_depthfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDepthFunc((GLenum)mxGetScalar(prhs[0]));

}

void gl_depthmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDepthMask((GLboolean)mxGetScalar(prhs[0]));

}

void gl_depthrange( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDepthRange((GLclampd)mxGetScalar(prhs[0]),
		(GLclampd)mxGetScalar(prhs[1]));

}

void gl_disable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDisable((GLenum)mxGetScalar(prhs[0]));

}

void gl_disableclientstate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDisableClientState((GLenum)mxGetScalar(prhs[0]));

}

void gl_drawarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawArrays((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]));

}

void gl_drawbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_drawelements( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawElements((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_drawpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawPixels((GLsizei)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(const GLvoid*)mxGetData(prhs[4]));

}

void gl_drawrangeelements( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawRangeElements((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_edgeflag( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEdgeFlag((GLboolean)mxGetScalar(prhs[0]));

}

void gl_edgeflagpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEdgeFlagPointer((GLsizei)mxGetScalar(prhs[0]),
		(const GLvoid*)mxGetData(prhs[1]));

}

void gl_edgeflagv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEdgeFlagv((const GLboolean*)mxGetData(prhs[0]));

}

void gl_enable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEnable((GLenum)mxGetScalar(prhs[0]));

}

void gl_enableclientstate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEnableClientState((GLenum)mxGetScalar(prhs[0]));

}

void gl_end( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEnd();

}

void gl_endlist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEndList();

}

void gl_evalcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord1d((GLdouble)mxGetScalar(prhs[0]));

}

void gl_evalcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord1dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_evalcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord1f((GLfloat)mxGetScalar(prhs[0]));

}

void gl_evalcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord1fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_evalcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_evalcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_evalcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_evalcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalCoord2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_evalmesh1( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalMesh1((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_evalmesh2( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalMesh2((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_evalpoint1( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalPoint1((GLint)mxGetScalar(prhs[0]));

}

void gl_evalpoint2( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEvalPoint2((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_feedbackbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFeedbackBuffer((GLsizei)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_finish( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFinish();

}

void gl_flush( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFlush();

}

void gl_fogf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_fogfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_fogi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogi((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_fogiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogiv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_frontface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFrontFace((GLenum)mxGetScalar(prhs[0]));

}

void gl_frustum( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFrustum((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_genlists( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGenLists((GLsizei)mxGetScalar(prhs[0]));

}

void gl_gentextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGenTextures((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getbooleanv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetBooleanv((GLenum)mxGetScalar(prhs[0]),
		(GLboolean*)mxGetData(prhs[1]));

}

void gl_getclipplane( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetClipPlane((GLenum)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_getcolortable( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetColorTable((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_getcolortableparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetColorTableParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getcolortableparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetColorTableParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getconvolutionfilter( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetConvolutionFilter((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_getconvolutionparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetConvolutionParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getconvolutionparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetConvolutionParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getdoublev( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetDoublev((GLenum)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]));

}

void gl_geterror( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetError();

}

void gl_getfloatv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetFloatv((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_gethistogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetHistogram((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

}

void gl_gethistogramparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetHistogramParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gethistogramparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetHistogramParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getintegerv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetIntegerv((GLenum)mxGetScalar(prhs[0]),
		(GLint*)mxGetData(prhs[1]));

}

void gl_getlightfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetLightfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getlightiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetLightiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getmapdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMapdv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getmapiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMapiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getmaterialfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMaterialfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getmaterialiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMaterialiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getminmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMinmax((GLenum)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

}

void gl_getminmaxparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMinmaxParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getminmaxparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetMinmaxParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getpixelmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetPixelMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLfloat*)mxGetData(prhs[1]));

}

void gl_getpixelmapuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetPixelMapuiv((GLenum)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_getpixelmapusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetPixelMapusv((GLenum)mxGetScalar(prhs[0]),
		(GLushort*)mxGetData(prhs[1]));

}

void gl_getpolygonstipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetPolygonStipple((GLubyte*)mxGetData(prhs[0]));

}

void gl_getseparablefilter( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetSeparableFilter((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]),
		(GLvoid*)mxGetData(prhs[5]));

}

void gl_gettexenvfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexEnvfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexenviv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexEnviv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_gettexgendv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexGendv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_gettexgenfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexGenfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexgeniv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexGeniv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexImage((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLvoid*)mxGetData(prhs[4]));

}

void gl_gettexlevelparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexLevelParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLfloat*)mxGetData(prhs[3]));

}

void gl_gettexlevelparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexLevelParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLint*)mxGetData(prhs[3]));

}

void gl_gettexparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_gettexparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetTexParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_hint( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glHint((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_histogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glHistogram((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]));

}

void gl_indexmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexMask((GLuint)mxGetScalar(prhs[0]));

}

void gl_indexpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_indexd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexd((GLdouble)mxGetScalar(prhs[0]));

}

void gl_indexdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexdv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_indexf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexf((GLfloat)mxGetScalar(prhs[0]));

}

void gl_indexfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexfv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_indexi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexi((GLint)mxGetScalar(prhs[0]));

}

void gl_indexiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexiv((const GLint*)mxGetData(prhs[0]));

}

void gl_indexs( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexs((GLshort)mxGetScalar(prhs[0]));

}

void gl_indexsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexsv((const GLshort*)mxGetData(prhs[0]));

}

void gl_indexub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexub((GLubyte)mxGetScalar(prhs[0]));

}

void gl_indexubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glIndexubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_initnames( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glInitNames();

}

void gl_interleavedarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glInterleavedArrays((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_isenabled( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsEnabled((GLenum)mxGetScalar(prhs[0]));

}

void gl_islist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsList((GLuint)mxGetScalar(prhs[0]));

}

void gl_istexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsTexture((GLuint)mxGetScalar(prhs[0]));

}

void gl_lightmodelf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightModelf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_lightmodelfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightModelfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_lightmodeli( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightModeli((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_lightmodeliv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightModeliv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_lightf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_lightfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_lighti( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLighti((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_lightiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLightiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_linestipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLineStipple((GLint)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]));

}

void gl_linewidth( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLineWidth((GLfloat)mxGetScalar(prhs[0]));

}

void gl_listbase( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glListBase((GLuint)mxGetScalar(prhs[0]));

}

void gl_loadidentity( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadIdentity();

}

void gl_loadmatrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_loadmatrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_loadname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadName((GLuint)mxGetScalar(prhs[0]));

}

void gl_logicop( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLogicOp((GLenum)mxGetScalar(prhs[0]));

}

void gl_map1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMap1d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(const GLdouble*)mxGetData(prhs[5]));

}

void gl_map1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMap1f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(const GLfloat*)mxGetData(prhs[5]));

}

void gl_map2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glMapGrid1d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_mapgrid1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMapGrid1f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_mapgrid2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMapGrid2d((GLint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_mapgrid2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMapGrid2f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]),
		(GLfloat)mxGetScalar(prhs[5]));

}

void gl_materialf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMaterialf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_materialfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMaterialfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_materiali( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMateriali((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_materialiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMaterialiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_matrixmode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMatrixMode((GLenum)mxGetScalar(prhs[0]));

}

void gl_minmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMinmax((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]));

}

void gl_multmatrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_multmatrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_newlist( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNewList((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_normal3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_normal3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_normal3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_normal3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_normal3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_normal3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_normal3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_normal3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_normal3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_normal3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormal3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_normalpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glNormalPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_ortho( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glOrtho((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]));

}

void gl_passthrough( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPassThrough((GLfloat)mxGetScalar(prhs[0]));

}

void gl_pixelmapfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelMapfv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_pixelmapuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelMapuiv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLuint*)mxGetData(prhs[2]));

}

void gl_pixelmapusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelMapusv((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(const GLushort*)mxGetData(prhs[2]));

}

void gl_pixelstoref( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelStoref((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pixelstorei( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelStorei((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pixeltransferf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelTransferf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pixeltransferi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelTransferi((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pixelzoom( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPixelZoom((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointsize( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPointSize((GLfloat)mxGetScalar(prhs[0]));

}

void gl_polygonmode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPolygonMode((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_polygonoffset( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPolygonOffset((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_polygonstipple( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPolygonStipple((const GLubyte*)mxGetData(prhs[0]));

}

void gl_popattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPopAttrib();

}

void gl_popclientattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPopClientAttrib();

}

void gl_popmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPopMatrix();

}

void gl_popname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPopName();

}

void gl_prioritizetextures( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPrioritizeTextures((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]),
		(const GLclampf*)mxGetData(prhs[2]));

}

void gl_pushattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPushAttrib((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_pushclientattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPushClientAttrib((GLbitfield)mxGetScalar(prhs[0]));

}

void gl_pushmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPushMatrix();

}

void gl_pushname( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPushName((GLuint)mxGetScalar(prhs[0]));

}

void gl_rasterpos2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_rasterpos2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_rasterpos2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_rasterpos2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_rasterpos2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_rasterpos3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_rasterpos3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_rasterpos3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_rasterpos3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_rasterpos3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_rasterpos4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rasterpos4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_rasterpos4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_rasterpos4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_rasterpos4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_rasterpos4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_rasterpos4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_rasterpos4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRasterPos4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_readbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glReadBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_readpixels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glReadPixels((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(GLvoid*)mxGetData(prhs[6]));

}

void gl_rectd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectd((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rectdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectdv((const GLdouble*)mxGetData(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_rectf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectf((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_rectfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectfv((const GLfloat*)mxGetData(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_recti( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRecti((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_rectiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectiv((const GLint*)mxGetData(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_rects( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRects((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_rectsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRectsv((const GLshort*)mxGetData(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_rendermode( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glRenderMode((GLenum)mxGetScalar(prhs[0]));

}

void gl_resethistogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glResetHistogram((GLenum)mxGetScalar(prhs[0]));

}

void gl_resetminmax( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glResetMinmax((GLenum)mxGetScalar(prhs[0]));

}

void gl_rotated( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRotated((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_rotatef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glRotatef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_scaled( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glScaled((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_scalef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glScalef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_scissor( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glScissor((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_selectbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSelectBuffer((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_separablefilter2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glShadeModel((GLenum)mxGetScalar(prhs[0]));

}

void gl_stencilfunc( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilFunc((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_stencilmask( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilMask((GLuint)mxGetScalar(prhs[0]));

}

void gl_stencilop( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilOp((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]));

}

void gl_texcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1d((GLdouble)mxGetScalar(prhs[0]));

}

void gl_texcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1f((GLfloat)mxGetScalar(prhs[0]));

}

void gl_texcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1i((GLint)mxGetScalar(prhs[0]));

}

void gl_texcoord1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1s((GLshort)mxGetScalar(prhs[0]));

}

void gl_texcoord1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord1sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_texcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_texcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_texcoord2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_texcoord2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_texcoord3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texcoord3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texcoord3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_texcoord3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoord4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_texcoord4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_texcoord4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_texcoord4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_texcoord4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_texcoord4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_texcoord4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_texcoord4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoord4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_texcoordpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexCoordPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_texenvf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexEnvf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texenvfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexEnvfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texenvi( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexEnvi((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texenviv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexEnviv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texgend( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGend((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_texgendv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGendv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLdouble*)mxGetData(prhs[2]));

}

void gl_texgenf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGenf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texgenfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGenfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texgeni( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGeni((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texgeniv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexGeniv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_teximage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glTexParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_texparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexParameterfv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_texparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_texparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_texsubimage1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLenum)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_texsubimage2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glTranslated((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_translatef( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glTranslatef((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertex2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertex2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertex2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_vertex2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertex2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_vertex3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertex3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertex3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_vertex3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertex3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_vertex4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertex4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_vertex4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertex4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_vertex4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_vertex4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4iv((const GLint*)mxGetData(prhs[0]));

}

void gl_vertex4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertex4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertex4sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_vertexpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_viewport( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glViewport((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_samplecoverage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSampleCoverage((GLclampf)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void gl_samplepass( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

  //glSamplePass((GLenum)mxGetScalar(prhs[0]));

}

void gl_loadtransposematrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadTransposeMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_loadtransposematrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLoadTransposeMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_multtransposematrixf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultTransposeMatrixf((const GLfloat*)mxGetData(prhs[0]));

}

void gl_multtransposematrixd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultTransposeMatrixd((const GLdouble*)mxGetData(prhs[0]));

}

void gl_compressedteximage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glCompressedTexImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_compressedtexsubimage3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glCompressedTexSubImage1D((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(GLsizei)mxGetScalar(prhs[5]),
		(const GLvoid*)mxGetData(prhs[6]));

}

void gl_getcompressedteximage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetCompressedTexImage((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLvoid*)mxGetData(prhs[2]));

}

void gl_activetexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glActiveTexture((GLenum)mxGetScalar(prhs[0]));

}

void gl_clientactivetexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glClientActiveTexture((GLenum)mxGetScalar(prhs[0]));

}

void gl_multitexcoord1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_multitexcoord1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord1sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_multitexcoord2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord2sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_multitexcoord3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord3sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_multitexcoord4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4d((GLenum)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4dv((GLenum)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_multitexcoord4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4f((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4fv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_multitexcoord4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4i((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4iv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_multitexcoord4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4s((GLenum)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_multitexcoord4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiTexCoord4sv((GLenum)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_fogcoordf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogCoordf((GLfloat)mxGetScalar(prhs[0]));

}

void gl_fogcoordfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogCoordfv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_fogcoordd( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogCoordd((GLdouble)mxGetScalar(prhs[0]));

}

void gl_fogcoorddv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogCoorddv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_fogcoordpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glFogCoordPointer((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]));

}

void gl_secondarycolor3b( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3b((GLbyte)mxGetScalar(prhs[0]),
		(GLbyte)mxGetScalar(prhs[1]),
		(GLbyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3bv((const GLbyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_secondarycolor3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_secondarycolor3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_secondarycolor3ub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3ub((GLubyte)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3ubv((const GLubyte*)mxGetData(prhs[0]));

}

void gl_secondarycolor3ui( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3ui((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLuint)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3uiv((const GLuint*)mxGetData(prhs[0]));

}

void gl_secondarycolor3us( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3us((GLushort)mxGetScalar(prhs[0]),
		(GLushort)mxGetScalar(prhs[1]),
		(GLushort)mxGetScalar(prhs[2]));

}

void gl_secondarycolor3usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColor3usv((const GLushort*)mxGetData(prhs[0]));

}

void gl_secondarycolorpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glSecondaryColorPointer((GLint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_pointparameterf( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPointParameterf((GLenum)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_pointparameterfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPointParameterfv((GLenum)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_pointparameteri( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPointParameteri((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_pointparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glPointParameteriv((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_blendfuncseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBlendFuncSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_multidrawarrays( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glMultiDrawArrays((GLenum)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]),
		(const GLsizei*)mxGetData(prhs[2]),
		(GLsizei)mxGetScalar(prhs[3]));

}

void gl_windowpos2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_windowpos2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_windowpos2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_windowpos2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2iv((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_windowpos2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos2sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_windowpos3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3d((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_windowpos3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3dv((const GLdouble*)mxGetData(prhs[0]));

}

void gl_windowpos3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3f((GLfloat)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_windowpos3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3fv((const GLfloat*)mxGetData(prhs[0]));

}

void gl_windowpos3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_windowpos3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3iv((const GLint*)mxGetData(prhs[0]));

}

void gl_windowpos3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3s((GLshort)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_windowpos3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glWindowPos3sv((const GLshort*)mxGetData(prhs[0]));

}

void gl_genqueries( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGenQueries((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_deletequeries( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteQueries((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_isquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsQuery((GLuint)mxGetScalar(prhs[0]));

}

void gl_beginquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBeginQuery((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_endquery( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEndQuery((GLenum)mxGetScalar(prhs[0]));

}

void gl_getqueryiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetQueryiv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetQueryObjectiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getqueryobjectuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetQueryObjectuiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLuint*)mxGetData(prhs[2]));

}

void gl_bindbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBindBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_deletebuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteBuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_genbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGenBuffers((GLsizei)mxGetScalar(prhs[0]),
		(GLuint*)mxGetData(prhs[1]));

}

void gl_isbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsBuffer((GLuint)mxGetScalar(prhs[0]));

}

void gl_bufferdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBufferData((GLenum)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLvoid*)mxGetData(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_buffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(const GLvoid*)mxGetData(prhs[3]));

}

void gl_getbuffersubdata( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetBufferSubData((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLvoid*)mxGetData(prhs[3]));

}

void gl_mapbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)glMapBuffer((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void gl_unmapbuffer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glUnmapBuffer((GLenum)mxGetScalar(prhs[0]));

}

void gl_getbufferparameteriv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetBufferParameteriv((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_drawbuffers( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDrawBuffers((GLsizei)mxGetScalar(prhs[0]),
		(const GLenum*)mxGetData(prhs[1]));

}

void gl_vertexattrib1d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib1s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]));

}

void gl_vertexattrib1sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib1sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib2d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib2s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]));

}

void gl_vertexattrib2sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib2sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib3d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib3s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]));

}

void gl_vertexattrib3sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib3sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nbv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nbv((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4niv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Niv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nsv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nsv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nub( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nub((GLuint)mxGetScalar(prhs[0]),
		(GLubyte)mxGetScalar(prhs[1]),
		(GLubyte)mxGetScalar(prhs[2]),
		(GLubyte)mxGetScalar(prhs[3]),
		(GLubyte)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4nubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nubv((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nuiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nuiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4nusv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4Nusv((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4bv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4bv((GLuint)mxGetScalar(prhs[0]),
		(const GLbyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4d( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4d((GLuint)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLdouble)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4dv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4dv((GLuint)mxGetScalar(prhs[0]),
		(const GLdouble*)mxGetData(prhs[1]));

}

void gl_vertexattrib4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4f((GLuint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4fv((GLuint)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]));

}

void gl_vertexattrib4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4iv((GLuint)mxGetScalar(prhs[0]),
		(const GLint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4s( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4s((GLuint)mxGetScalar(prhs[0]),
		(GLshort)mxGetScalar(prhs[1]),
		(GLshort)mxGetScalar(prhs[2]),
		(GLshort)mxGetScalar(prhs[3]),
		(GLshort)mxGetScalar(prhs[4]));

}

void gl_vertexattrib4sv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4sv((GLuint)mxGetScalar(prhs[0]),
		(const GLshort*)mxGetData(prhs[1]));

}

void gl_vertexattrib4ubv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4ubv((GLuint)mxGetScalar(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void gl_vertexattrib4uiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4uiv((GLuint)mxGetScalar(prhs[0]),
		(const GLuint*)mxGetData(prhs[1]));

}

void gl_vertexattrib4usv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttrib4usv((GLuint)mxGetScalar(prhs[0]),
		(const GLushort*)mxGetData(prhs[1]));

}

void gl_vertexattribpointer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glVertexAttribPointer((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLboolean)mxGetScalar(prhs[3]),
		(GLsizei)mxGetScalar(prhs[4]),
		(const GLvoid*)mxGetData(prhs[5]));

}

void gl_enablevertexattribarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glEnableVertexAttribArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_disablevertexattribarray( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDisableVertexAttribArray((GLuint)mxGetScalar(prhs[0]));

}

void gl_getvertexattribdv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetVertexAttribdv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void gl_getvertexattribfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetVertexAttribfv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getvertexattribiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetVertexAttribiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_deleteshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_detachshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDetachShader((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_createshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateShader((GLenum)mxGetScalar(prhs[0]));

}

void gl_compileshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glCompileShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_createprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glCreateProgram();

}

void gl_attachshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glAttachShader((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void gl_linkprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glLinkProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_useprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUseProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_deleteprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glDeleteProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_validateprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glValidateProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_uniform1f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform1f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]));

}

void gl_uniform2f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform2f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void gl_uniform3f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform3f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]));

}

void gl_uniform4f( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform4f((GLint)mxGetScalar(prhs[0]),
		(GLfloat)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]),
		(GLfloat)mxGetScalar(prhs[3]),
		(GLfloat)mxGetScalar(prhs[4]));

}

void gl_uniform1i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform1i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]));

}

void gl_uniform2i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform2i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void gl_uniform3i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform3i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void gl_uniform4i( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform4i((GLint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void gl_uniform1fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform1fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]));

}

void gl_uniform1iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform1iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform2iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform2iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform3iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform3iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniform4iv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniform4iv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(const GLint*)mxGetData(prhs[2]));

}

void gl_uniformmatrix2fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniformMatrix2fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix3fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniformMatrix3fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_uniformmatrix4fv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glUniformMatrix4fv((GLint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLboolean)mxGetScalar(prhs[2]),
		(const GLfloat*)mxGetData(prhs[3]));

}

void gl_isshader( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsShader((GLuint)mxGetScalar(prhs[0]));

}

void gl_isprogram( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glIsProgram((GLuint)mxGetScalar(prhs[0]));

}

void gl_getshaderiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetShaderiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getprogramiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetProgramiv((GLuint)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getattachedshaders( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetAttachedShaders((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLuint*)mxGetData(prhs[3]));

}

void gl_getshaderinfolog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetShaderInfoLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_getprograminfolog( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetProgramInfoLog((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_getuniformlocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetUniformLocation((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_getactiveuniform( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetActiveUniform((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_getuniformfv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetUniformfv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void gl_getuniformiv( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetUniformiv((GLuint)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint*)mxGetData(prhs[2]));

}

void gl_getshadersource( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetShaderSource((GLuint)mxGetScalar(prhs[0]),
		(GLsizei)mxGetScalar(prhs[1]),
		(GLsizei*)mxGetData(prhs[2]),
		(GLchar*)mxGetData(prhs[3]));

}

void gl_bindattriblocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glBindAttribLocation((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(const GLchar*)mxGetData(prhs[2]));

}

void gl_getactiveattrib( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glGetActiveAttrib((GLuint)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLsizei*)mxGetData(prhs[3]),
		(GLsizei*)mxGetData(prhs[4]),
		(GLenum*)mxGetData(prhs[5]),
		(GLchar*)mxGetData(prhs[6]));

}

void gl_getattriblocation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)glGetAttribLocation((GLuint)mxGetScalar(prhs[0]),
		(const GLchar*)mxGetData(prhs[1]));

}

void gl_stencilfuncseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilFuncSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLuint)mxGetScalar(prhs[3]));

}

void gl_stencilopseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilOpSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLenum)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]));

}

void gl_stencilmaskseparate( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glStencilMaskSeparate((GLenum)mxGetScalar(prhs[0]),
		(GLuint)mxGetScalar(prhs[1]));

}

void glu_begincurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluBeginCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_beginpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluBeginPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_beginsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluBeginSurface((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_begintrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluBeginTrim((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_build1dmipmaplevels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluBuild1DMipmaps((GLenum)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLsizei)mxGetScalar(prhs[2]),
		(GLenum)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]),
		(const void*)mxGetData(prhs[5]));

}

void glu_build2dmipmaplevels( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[0])=(double)gluCheckExtension((const GLubyte*)mxGetData(prhs[0]),
		(const GLubyte*)mxGetData(prhs[1]));

}

void glu_cylinder( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluCylinder((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLint)mxGetScalar(prhs[5]));

}

void glu_deletenurbsrenderer( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluDeleteNurbsRenderer((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_deletequadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluDeleteQuadric((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_deletetess( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluDeleteTess((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_disk( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluDisk((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]));

}

void glu_endcurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluEndCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluEndPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endsurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluEndSurface((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_endtrim( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluEndTrim((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_getnurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluGetNurbsProperty((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]));

}

void glu_gettessproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluGetTessProperty((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble*)mxGetData(prhs[2]));

}

void glu_loadsamplingmatrices( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluLoadSamplingMatrices((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(const GLfloat*)mxGetData(prhs[1]),
		(const GLfloat*)mxGetData(prhs[2]),
		(const GLint*)mxGetData(prhs[3]));

}

void glu_lookat( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewNurbsRenderer();

}

void glu_newquadric( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewQuadric();

}

void glu_newtess( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);
	*(unsigned int *)mxGetData(plhs[0])=(unsigned int)gluNewTess();

}

void glu_nextcontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluNextContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_nurbscurve( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluNurbsCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLfloat*)mxGetData(prhs[4]),
		(GLint)mxGetScalar(prhs[5]),
		(GLenum)mxGetScalar(prhs[6]));

}

void glu_nurbsproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluNurbsProperty((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLfloat)mxGetScalar(prhs[2]));

}

void glu_nurbssurface( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	gluOrtho2D((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_partialdisk( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluPartialDisk((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLint)mxGetScalar(prhs[4]),
		(GLdouble)mxGetScalar(prhs[5]),
		(GLdouble)mxGetScalar(prhs[6]));

}

void glu_perspective( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluPerspective((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_pickmatrix( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluPickMatrix((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]),
		(GLint*)mxGetData(prhs[4]));

}

void glu_project( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	gluPwlCurve((GLUnurbs*)(unsigned int)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLfloat*)mxGetData(prhs[2]),
		(GLint)mxGetScalar(prhs[3]),
		(GLenum)mxGetScalar(prhs[4]));

}

void glu_quadricdrawstyle( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluQuadricDrawStyle((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricnormals( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluQuadricNormals((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadricorientation( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluQuadricOrientation((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]));

}

void glu_quadrictexture( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluQuadricTexture((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLboolean)mxGetScalar(prhs[1]));

}

void glu_scaleimage( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	gluSphere((GLUquadric*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glu_tessbegincontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessBeginContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessbeginpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessBeginPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLvoid*)mxGetData(prhs[1]));

}

void glu_tessendcontour( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessEndContour((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessendpolygon( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessEndPolygon((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]));

}

void glu_tessnormal( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessNormal((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]),
		(GLdouble)mxGetScalar(prhs[3]));

}

void glu_tessproperty( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessProperty((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLenum)mxGetScalar(prhs[1]),
		(GLdouble)mxGetScalar(prhs[2]));

}

void glu_tessvertex( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	gluTessVertex((GLUtesselator*)(unsigned int)mxGetScalar(prhs[0]),
		(GLdouble*)mxGetData(prhs[1]),
		(GLvoid*)mxGetData(prhs[2]));

}

void glu_unproject( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

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

	glutWireSphere((GLdouble)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void glut_solidsphere( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidSphere((GLdouble)mxGetScalar(prhs[0]),
		(GLint)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]));

}

void glut_wirecone( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireCone((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_solidcone( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidCone((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_wirecube( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireCube((GLdouble)mxGetScalar(prhs[0]));

}

void glut_solidcube( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidCube((GLdouble)mxGetScalar(prhs[0]));

}

void glut_wiretorus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireTorus((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_solidtorus( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidTorus((GLdouble)mxGetScalar(prhs[0]),
		(GLdouble)mxGetScalar(prhs[1]),
		(GLint)mxGetScalar(prhs[2]),
		(GLint)mxGetScalar(prhs[3]));

}

void glut_wiredodecahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireDodecahedron();

}

void glut_soliddodecahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidDodecahedron();

}

void glut_wireteapot( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireTeapot((GLdouble)mxGetScalar(prhs[0]));

}

void glut_solidteapot( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidTeapot((GLdouble)mxGetScalar(prhs[0]));

}

void glut_wireoctahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireOctahedron();

}

void glut_solidoctahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidOctahedron();

}

void glut_wiretetrahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireTetrahedron();

}

void glut_solidtetrahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidTetrahedron();

}

void glut_wireicosahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutWireIcosahedron();

}

void glut_solidicosahedron( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] ) {

	glutSolidIcosahedron();

}

int gl_auto_map_count=642;
cmdhandler gl_auto_map[] = {
{ "glAccum",                         gl_accum                            },
{ "glActiveTexture",                 gl_activetexture                    },
{ "glAlphaFunc",                     gl_alphafunc                        },
{ "glAreTexturesResident",           gl_aretexturesresident              },
{ "glArrayElement",                  gl_arrayelement                     },
{ "glAttachShader",                  gl_attachshader                     },
{ "glBegin",                         gl_begin                            },
{ "glBeginQuery",                    gl_beginquery                       },
{ "glBindAttribLocation",            gl_bindattriblocation               },
{ "glBindBuffer",                    gl_bindbuffer                       },
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
{ "glDeleteLists",                   gl_deletelists                      },
{ "glDeleteProgram",                 gl_deleteprogram                    },
{ "glDeleteQueries",                 gl_deletequeries                    },
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
{ "glFeedbackBuffer",                gl_feedbackbuffer                   },
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
{ "glFrontFace",                     gl_frontface                        },
{ "glFrustum",                       gl_frustum                          },
{ "glGenBuffers",                    gl_genbuffers                       },
{ "glGenLists",                      gl_genlists                         },
{ "glGenQueries",                    gl_genqueries                       },
{ "glGenTextures",                   gl_gentextures                      },
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
{ "glGetCompressedTexImage",         gl_getcompressedteximage            },
{ "glGetConvolutionFilter",          gl_getconvolutionfilter             },
{ "glGetConvolutionParameterfv",     gl_getconvolutionparameterfv        },
{ "glGetConvolutionParameteriv",     gl_getconvolutionparameteriv        },
{ "glGetDoublev",                    gl_getdoublev                       },
{ "glGetError",                      gl_geterror                         },
{ "glGetFloatv",                     gl_getfloatv                        },
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
{ "glGetProgramiv",                  gl_getprogramiv                     },
{ "glGetQueryObjectiv",              gl_getqueryobjectiv                 },
{ "glGetQueryObjectuiv",             gl_getqueryobjectuiv                },
{ "glGetQueryiv",                    gl_getqueryiv                       },
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
{ "glGetUniformLocation",            gl_getuniformlocation               },
{ "glGetUniformfv",                  gl_getuniformfv                     },
{ "glGetUniformiv",                  gl_getuniformiv                     },
{ "glGetVertexAttribdv",             gl_getvertexattribdv                },
{ "glGetVertexAttribfv",             gl_getvertexattribfv                },
{ "glGetVertexAttribiv",             gl_getvertexattribiv                },
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
{ "glIsList",                        gl_islist                           },
{ "glIsProgram",                     gl_isprogram                        },
{ "glIsQuery",                       gl_isquery                          },
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
{ "glPointParameteriv",              gl_pointparameteriv                 },
{ "glPointSize",                     gl_pointsize                        },
{ "glPolygonMode",                   gl_polygonmode                      },
{ "glPolygonOffset",                 gl_polygonoffset                    },
{ "glPolygonStipple",                gl_polygonstipple                   },
{ "glPopAttrib",                     gl_popattrib                        },
{ "glPopClientAttrib",               gl_popclientattrib                  },
{ "glPopMatrix",                     gl_popmatrix                        },
{ "glPopName",                       gl_popname                          },
{ "glPrioritizeTextures",            gl_prioritizetextures               },
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
{ "glResetHistogram",                gl_resethistogram                   },
{ "glResetMinmax",                   gl_resetminmax                      },
{ "glRotated",                       gl_rotated                          },
{ "glRotatef",                       gl_rotatef                          },
{ "glSampleCoverage",                gl_samplecoverage                   },
{ "glSamplePass",                    gl_samplepass                       },
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
{ "glSelectBuffer",                  gl_selectbuffer                     },
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
{ "glVertexAttrib1dv",               gl_vertexattrib1dv                  },
{ "glVertexAttrib1f",                gl_vertexattrib1f                   },
{ "glVertexAttrib1fv",               gl_vertexattrib1fv                  },
{ "glVertexAttrib1s",                gl_vertexattrib1s                   },
{ "glVertexAttrib1sv",               gl_vertexattrib1sv                  },
{ "glVertexAttrib2d",                gl_vertexattrib2d                   },
{ "glVertexAttrib2dv",               gl_vertexattrib2dv                  },
{ "glVertexAttrib2f",                gl_vertexattrib2f                   },
{ "glVertexAttrib2fv",               gl_vertexattrib2fv                  },
{ "glVertexAttrib2s",                gl_vertexattrib2s                   },
{ "glVertexAttrib2sv",               gl_vertexattrib2sv                  },
{ "glVertexAttrib3d",                gl_vertexattrib3d                   },
{ "glVertexAttrib3dv",               gl_vertexattrib3dv                  },
{ "glVertexAttrib3f",                gl_vertexattrib3f                   },
{ "glVertexAttrib3fv",               gl_vertexattrib3fv                  },
{ "glVertexAttrib3s",                gl_vertexattrib3s                   },
{ "glVertexAttrib3sv",               gl_vertexattrib3sv                  },
{ "glVertexAttrib4Nbv",              gl_vertexattrib4nbv                 },
{ "glVertexAttrib4Niv",              gl_vertexattrib4niv                 },
{ "glVertexAttrib4Nsv",              gl_vertexattrib4nsv                 },
{ "glVertexAttrib4Nub",              gl_vertexattrib4nub                 },
{ "glVertexAttrib4Nubv",             gl_vertexattrib4nubv                },
{ "glVertexAttrib4Nuiv",             gl_vertexattrib4nuiv                },
{ "glVertexAttrib4Nusv",             gl_vertexattrib4nusv                },
{ "glVertexAttrib4bv",               gl_vertexattrib4bv                  },
{ "glVertexAttrib4d",                gl_vertexattrib4d                   },
{ "glVertexAttrib4dv",               gl_vertexattrib4dv                  },
{ "glVertexAttrib4f",                gl_vertexattrib4f                   },
{ "glVertexAttrib4fv",               gl_vertexattrib4fv                  },
{ "glVertexAttrib4iv",               gl_vertexattrib4iv                  },
{ "glVertexAttrib4s",                gl_vertexattrib4s                   },
{ "glVertexAttrib4sv",               gl_vertexattrib4sv                  },
{ "glVertexAttrib4ubv",              gl_vertexattrib4ubv                 },
{ "glVertexAttrib4uiv",              gl_vertexattrib4uiv                 },
{ "glVertexAttrib4usv",              gl_vertexattrib4usv                 },
{ "glVertexAttribPointer",           gl_vertexattribpointer              },
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
