function glVertexAttribLFormatNV( index, size, type, stride )

% glVertexAttribLFormatNV  Interface to OpenGL function glVertexAttribLFormatNV
%
% usage:  glVertexAttribLFormatNV( index, size, type, stride )
%
% C function:  void glVertexAttribLFormatNV(GLuint index, GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribLFormatNV', index, size, type, stride );

return
