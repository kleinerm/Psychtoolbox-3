function glTexCoord3dv( v )

% glTexCoord3dv  Interface to OpenGL function glTexCoord3dv
%
% usage:  glTexCoord3dv( v )
%
% C function:  void glTexCoord3dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3dv', double(v) );

return
