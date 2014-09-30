function glVertexFormatNV( size, type, stride )

% glVertexFormatNV  Interface to OpenGL function glVertexFormatNV
%
% usage:  glVertexFormatNV( size, type, stride )
%
% C function:  void glVertexFormatNV(GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexFormatNV', size, type, stride );

return
