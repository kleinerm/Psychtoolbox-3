function glMapVertexAttrib1dAPPLE( index, size, u1, u2, stride, order, points )

% glMapVertexAttrib1dAPPLE  Interface to OpenGL function glMapVertexAttrib1dAPPLE
%
% usage:  glMapVertexAttrib1dAPPLE( index, size, u1, u2, stride, order, points )
%
% C function:  void glMapVertexAttrib1dAPPLE(GLuint index, GLuint size, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMapVertexAttrib1dAPPLE', index, size, u1, u2, stride, order, double(points) );

return
