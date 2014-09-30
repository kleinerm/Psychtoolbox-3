function glTexCoordPointerEXT( size, type, stride, count, pointer )

% glTexCoordPointerEXT  Interface to OpenGL function glTexCoordPointerEXT
%
% usage:  glTexCoordPointerEXT( size, type, stride, count, pointer )
%
% C function:  void glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glTexCoordPointerEXT', size, type, stride, count, pointer );

return
