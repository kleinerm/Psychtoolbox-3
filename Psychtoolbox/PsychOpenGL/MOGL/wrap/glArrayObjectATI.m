function glArrayObjectATI( array, size, type, stride, buffer, offset )

% glArrayObjectATI  Interface to OpenGL function glArrayObjectATI
%
% usage:  glArrayObjectATI( array, size, type, stride, buffer, offset )
%
% C function:  void glArrayObjectATI(GLenum array, GLint size, GLenum type, GLsizei stride, GLuint buffer, GLuint offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glArrayObjectATI', array, size, type, stride, buffer, offset );

return
