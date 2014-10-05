function glIndexPointerEXT( type, stride, count, pointer )

% glIndexPointerEXT  Interface to OpenGL function glIndexPointerEXT
%
% usage:  glIndexPointerEXT( type, stride, count, pointer )
%
% C function:  void glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glIndexPointerEXT', type, stride, count, pointer );

return
