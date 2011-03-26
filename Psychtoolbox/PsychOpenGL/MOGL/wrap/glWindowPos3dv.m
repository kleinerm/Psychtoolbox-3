function glWindowPos3dv( v )

% glWindowPos3dv  Interface to OpenGL function glWindowPos3dv
%
% usage:  glWindowPos3dv( v )
%
% C function:  void glWindowPos3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3dv', double(v) );

return
