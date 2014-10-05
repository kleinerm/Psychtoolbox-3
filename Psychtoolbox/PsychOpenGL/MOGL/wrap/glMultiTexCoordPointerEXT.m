function glMultiTexCoordPointerEXT( texunit, size, type, stride, pointer )

% glMultiTexCoordPointerEXT  Interface to OpenGL function glMultiTexCoordPointerEXT
%
% usage:  glMultiTexCoordPointerEXT( texunit, size, type, stride, pointer )
%
% C function:  void glMultiTexCoordPointerEXT(GLenum texunit, GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoordPointerEXT', texunit, size, type, stride, pointer );

return
