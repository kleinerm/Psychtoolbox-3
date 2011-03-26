function glRasterPos4dv( v )

% glRasterPos4dv  Interface to OpenGL function glRasterPos4dv
%
% usage:  glRasterPos4dv( v )
%
% C function:  void glRasterPos4dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos4dv', double(v) );

return
