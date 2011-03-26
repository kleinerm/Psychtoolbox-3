function glSecondaryColor3dv( v )

% glSecondaryColor3dv  Interface to OpenGL function glSecondaryColor3dv
%
% usage:  glSecondaryColor3dv( v )
%
% C function:  void glSecondaryColor3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glSecondaryColor3dv', double(v) );

return
