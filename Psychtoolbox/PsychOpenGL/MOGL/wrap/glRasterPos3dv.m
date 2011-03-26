function glRasterPos3dv( v )

% glRasterPos3dv  Interface to OpenGL function glRasterPos3dv
%
% usage:  glRasterPos3dv( v )
%
% C function:  void glRasterPos3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRasterPos3dv', double(v) );

return
