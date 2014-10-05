function glColorFormatNV( size, type, stride )

% glColorFormatNV  Interface to OpenGL function glColorFormatNV
%
% usage:  glColorFormatNV( size, type, stride )
%
% C function:  void glColorFormatNV(GLint size, GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColorFormatNV', size, type, stride );

return
