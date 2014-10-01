function glMatrixRotatedEXT( mode, angle, x, y, z )

% glMatrixRotatedEXT  Interface to OpenGL function glMatrixRotatedEXT
%
% usage:  glMatrixRotatedEXT( mode, angle, x, y, z )
%
% C function:  void glMatrixRotatedEXT(GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMatrixRotatedEXT', mode, angle, x, y, z );

return
