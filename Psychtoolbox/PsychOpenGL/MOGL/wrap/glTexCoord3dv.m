function glTexCoord3dv( v )

% glTexCoord3dv  Interface to OpenGL function glTexCoord3dv
%
% usage:  glTexCoord3dv( v )
%
% C function:  void glTexCoord3dv(const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3dv', double(v) );

return
