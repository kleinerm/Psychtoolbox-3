function glWeightPointerARB( size, type, stride, pointer )

% glWeightPointerARB  Interface to OpenGL function glWeightPointerARB
%
% usage:  glWeightPointerARB( size, type, stride, pointer )
%
% C function:  void glWeightPointerARB(GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glWeightPointerARB', size, type, stride, pointer );

return
