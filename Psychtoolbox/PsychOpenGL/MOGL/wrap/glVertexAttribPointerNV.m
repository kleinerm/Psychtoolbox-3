function glVertexAttribPointerNV( index, fsize, type, stride, pointer )

% glVertexAttribPointerNV  Interface to OpenGL function glVertexAttribPointerNV
%
% usage:  glVertexAttribPointerNV( index, fsize, type, stride, pointer )
%
% C function:  void glVertexAttribPointerNV(GLuint index, GLint fsize, GLenum type, GLsizei stride, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribPointerNV', index, fsize, type, stride, pointer );

return
