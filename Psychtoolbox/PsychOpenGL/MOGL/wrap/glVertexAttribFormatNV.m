function glVertexAttribFormatNV( index, size, type, normalized, stride )

% glVertexAttribFormatNV  Interface to OpenGL function glVertexAttribFormatNV
%
% usage:  glVertexAttribFormatNV( index, size, type, normalized, stride )
%
% C function:  void glVertexAttribFormatNV(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribFormatNV', index, size, type, normalized, stride );

return
