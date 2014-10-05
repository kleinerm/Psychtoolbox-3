function glMapVertexAttrib2fAPPLE( index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )

% glMapVertexAttrib2fAPPLE  Interface to OpenGL function glMapVertexAttrib2fAPPLE
%
% usage:  glMapVertexAttrib2fAPPLE( index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points )
%
% C function:  void glMapVertexAttrib2fAPPLE(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glMapVertexAttrib2fAPPLE', index, size, u1, u2, ustride, uorder, v1, v2, vstride, vorder, single(points) );

return
