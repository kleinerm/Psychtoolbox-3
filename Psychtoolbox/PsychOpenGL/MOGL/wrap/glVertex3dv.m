function glVertex3dv( v )

% glVertex3dv  Interface to OpenGL function glVertex3dv
%
% usage:  glVertex3dv( v )
%
% C function:  void glVertex3dv(const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex3dv', double(v) );

return
