function glRasterPos3sv( v )

% glRasterPos3sv  Interface to OpenGL function glRasterPos3sv
%
% usage:  glRasterPos3sv( v )
%
% C function:  void glRasterPos3sv(const GLshort* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3sv', int16(v) );

return
