function glSecondaryColorPointerEXT( size, type, stride, pointer )

% glSecondaryColorPointerEXT  Interface to OpenGL function glSecondaryColorPointerEXT
%
% usage:  glSecondaryColorPointerEXT( size, type, stride, pointer )
%
% C function:  void glSecondaryColorPointerEXT(GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColorPointerEXT', size, type, stride, pointer );

return
