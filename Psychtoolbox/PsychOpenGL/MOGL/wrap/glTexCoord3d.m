function glTexCoord3d( s, t, r )

% glTexCoord3d  Interface to OpenGL function glTexCoord3d
%
% usage:  glTexCoord3d( s, t, r )
%
% C function:  void glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3d', s, t, r );

return
