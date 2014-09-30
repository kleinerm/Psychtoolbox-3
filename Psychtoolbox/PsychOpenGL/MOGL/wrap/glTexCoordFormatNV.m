function glTexCoordFormatNV( size, type, stride )

% glTexCoordFormatNV  Interface to OpenGL function glTexCoordFormatNV
%
% usage:  glTexCoordFormatNV( size, type, stride )
%
% C function:  void glTexCoordFormatNV(GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoordFormatNV', size, type, stride );

return
