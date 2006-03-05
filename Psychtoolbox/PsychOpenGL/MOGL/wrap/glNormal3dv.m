function glNormal3dv( v )

% glNormal3dv  Interface to OpenGL function glNormal3dv
%
% usage:  glNormal3dv( v )
%
% C function:  void glNormal3dv(const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glNormal3dv', double(v) );

return
