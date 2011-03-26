function glColor3dv( v )

% glColor3dv  Interface to OpenGL function glColor3dv
%
% usage:  glColor3dv( v )
%
% C function:  void glColor3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3dv', double(v) );

return
