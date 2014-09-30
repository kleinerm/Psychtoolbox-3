function glMapVertexAttrib1fAPPLE( index, size, u1, u2, stride, order, points )

% glMapVertexAttrib1fAPPLE  Interface to OpenGL function glMapVertexAttrib1fAPPLE
%
% usage:  glMapVertexAttrib1fAPPLE( index, size, u1, u2, stride, order, points )
%
% C function:  void glMapVertexAttrib1fAPPLE(GLuint index, GLuint size, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMapVertexAttrib1fAPPLE', index, size, u1, u2, stride, order, single(points) );

return
