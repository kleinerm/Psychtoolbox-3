function glVertexAttribIPointerEXT( index, size, type, stride, pointer )

% glVertexAttribIPointerEXT  Interface to OpenGL function glVertexAttribIPointerEXT
%
% usage:  glVertexAttribIPointerEXT( index, size, type, stride, pointer )
%
% C function:  void glVertexAttribIPointerEXT(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribIPointerEXT', index, size, type, stride, pointer );

return
