function glVertexAttribIFormatNV( index, size, type, stride )

% glVertexAttribIFormatNV  Interface to OpenGL function glVertexAttribIFormatNV
%
% usage:  glVertexAttribIFormatNV( index, size, type, stride )
%
% C function:  void glVertexAttribIFormatNV(GLuint index, GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribIFormatNV', index, size, type, stride );

return
