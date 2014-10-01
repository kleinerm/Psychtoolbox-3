function glFogCoordFormatNV( type, stride )

% glFogCoordFormatNV  Interface to OpenGL function glFogCoordFormatNV
%
% usage:  glFogCoordFormatNV( type, stride )
%
% C function:  void glFogCoordFormatNV(GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFogCoordFormatNV', type, stride );

return
