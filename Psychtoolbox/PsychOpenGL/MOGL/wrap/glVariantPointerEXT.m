function glVariantPointerEXT( id, type, stride, addr )

% glVariantPointerEXT  Interface to OpenGL function glVariantPointerEXT
%
% usage:  glVariantPointerEXT( id, type, stride, addr )
%
% C function:  void glVariantPointerEXT(GLuint id, GLenum type, GLuint stride, const void* addr)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVariantPointerEXT', id, type, stride, addr );

return
