function glSecondaryColorFormatNV( size, type, stride )

% glSecondaryColorFormatNV  Interface to OpenGL function glSecondaryColorFormatNV
%
% usage:  glSecondaryColorFormatNV( size, type, stride )
%
% C function:  void glSecondaryColorFormatNV(GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColorFormatNV', size, type, stride );

return
