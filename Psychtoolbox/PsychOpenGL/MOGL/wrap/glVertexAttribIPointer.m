function glVertexAttribIPointer( index, size, type, stride, pointer )

% glVertexAttribIPointer  Interface to OpenGL function glVertexAttribIPointer
%
% usage:  glVertexAttribIPointer( index, size, type, stride, pointer )
%
% C function:  void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribIPointer', index, size, type, stride, pointer );

return
