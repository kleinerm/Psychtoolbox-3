function glMatrixIndexPointerARB( size, type, stride, pointer )

% glMatrixIndexPointerARB  Interface to OpenGL function glMatrixIndexPointerARB
%
% usage:  glMatrixIndexPointerARB( size, type, stride, pointer )
%
% C function:  void glMatrixIndexPointerARB(GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMatrixIndexPointerARB', size, type, stride, pointer );

return
