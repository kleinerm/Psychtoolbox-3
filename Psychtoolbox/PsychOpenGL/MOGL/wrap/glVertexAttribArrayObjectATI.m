function glVertexAttribArrayObjectATI( index, size, type, normalized, stride, buffer, offset )

% glVertexAttribArrayObjectATI  Interface to OpenGL function glVertexAttribArrayObjectATI
%
% usage:  glVertexAttribArrayObjectATI( index, size, type, normalized, stride, buffer, offset )
%
% C function:  void glVertexAttribArrayObjectATI(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLuint buffer, GLuint offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribArrayObjectATI', index, size, type, normalized, stride, buffer, offset );

return
