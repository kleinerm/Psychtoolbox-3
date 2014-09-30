function glVertexPointerEXT( size, type, stride, count, pointer )

% glVertexPointerEXT  Interface to OpenGL function glVertexPointerEXT
%
% usage:  glVertexPointerEXT( size, type, stride, count, pointer )
%
% C function:  void glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexPointerEXT', size, type, stride, count, pointer );

return
