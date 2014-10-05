function glNormalFormatNV( type, stride )

% glNormalFormatNV  Interface to OpenGL function glNormalFormatNV
%
% usage:  glNormalFormatNV( type, stride )
%
% C function:  void glNormalFormatNV(GLenum type, GLsizei stride)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormalFormatNV', type, stride );

return
