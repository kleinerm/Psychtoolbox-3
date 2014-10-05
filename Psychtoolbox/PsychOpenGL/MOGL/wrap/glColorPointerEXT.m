function glColorPointerEXT( size, type, stride, count, pointer )

% glColorPointerEXT  Interface to OpenGL function glColorPointerEXT
%
% usage:  glColorPointerEXT( size, type, stride, count, pointer )
%
% C function:  void glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glColorPointerEXT', size, type, stride, count, pointer );

return
