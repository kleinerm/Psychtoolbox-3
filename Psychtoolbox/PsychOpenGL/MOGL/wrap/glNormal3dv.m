function glNormal3dv( v )

% glNormal3dv  Interface to OpenGL function glNormal3dv
%
% usage:  glNormal3dv( v )
%
% C function:  void glNormal3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glNormal3dv', double(v) );

return
