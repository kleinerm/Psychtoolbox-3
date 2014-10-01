function glTangentPointerEXT( type, stride, pointer )

% glTangentPointerEXT  Interface to OpenGL function glTangentPointerEXT
%
% usage:  glTangentPointerEXT( type, stride, pointer )
%
% C function:  void glTangentPointerEXT(GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTangentPointerEXT', type, stride, pointer );

return
