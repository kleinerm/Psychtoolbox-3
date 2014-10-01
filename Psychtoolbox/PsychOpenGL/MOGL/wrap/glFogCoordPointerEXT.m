function glFogCoordPointerEXT( type, stride, pointer )

% glFogCoordPointerEXT  Interface to OpenGL function glFogCoordPointerEXT
%
% usage:  glFogCoordPointerEXT( type, stride, pointer )
%
% C function:  void glFogCoordPointerEXT(GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFogCoordPointerEXT', type, stride, pointer );

return
