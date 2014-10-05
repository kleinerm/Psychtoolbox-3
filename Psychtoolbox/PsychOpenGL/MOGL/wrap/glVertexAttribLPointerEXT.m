function glVertexAttribLPointerEXT( index, size, type, stride, pointer )

% glVertexAttribLPointerEXT  Interface to OpenGL function glVertexAttribLPointerEXT
%
% usage:  glVertexAttribLPointerEXT( index, size, type, stride, pointer )
%
% C function:  void glVertexAttribLPointerEXT(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribLPointerEXT', index, size, type, stride, pointer );

return
