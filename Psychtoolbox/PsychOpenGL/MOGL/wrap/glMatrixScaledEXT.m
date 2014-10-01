function glMatrixScaledEXT( mode, x, y, z )

% glMatrixScaledEXT  Interface to OpenGL function glMatrixScaledEXT
%
% usage:  glMatrixScaledEXT( mode, x, y, z )
%
% C function:  void glMatrixScaledEXT(GLenum mode, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMatrixScaledEXT', mode, x, y, z );

return
