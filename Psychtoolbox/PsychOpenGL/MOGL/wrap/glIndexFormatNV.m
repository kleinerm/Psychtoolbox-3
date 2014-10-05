function glIndexFormatNV( type, stride )

% glIndexFormatNV  Interface to OpenGL function glIndexFormatNV
%
% usage:  glIndexFormatNV( type, stride )
%
% C function:  void glIndexFormatNV(GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glIndexFormatNV', type, stride );

return
