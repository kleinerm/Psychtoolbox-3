function glNormalPointerEXT( type, stride, count, pointer )

% glNormalPointerEXT  Interface to OpenGL function glNormalPointerEXT
%
% usage:  glNormalPointerEXT( type, stride, count, pointer )
%
% C function:  void glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNormalPointerEXT', type, stride, count, pointer );

return
