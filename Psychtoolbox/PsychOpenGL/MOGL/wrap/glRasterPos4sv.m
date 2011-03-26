function glRasterPos4sv( v )

% glRasterPos4sv  Interface to OpenGL function glRasterPos4sv
%
% usage:  glRasterPos4sv( v )
%
% C function:  void glRasterPos4sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4sv', int16(v) );

return
