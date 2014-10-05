function glMapVertexAttrib2dAPPLE( index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )

% glMapVertexAttrib2dAPPLE  Interface to OpenGL function glMapVertexAttrib2dAPPLE
%
% usage:  glMapVertexAttrib2dAPPLE( index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )
%
% C function:  void glMapVertexAttrib2dAPPLE(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glMapVertexAttrib2dAPPLE', index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, double(points) );

return
