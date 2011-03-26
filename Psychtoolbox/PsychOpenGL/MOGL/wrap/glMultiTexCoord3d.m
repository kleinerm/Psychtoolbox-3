function glMultiTexCoord3d( target, s, t, r )

% glMultiTexCoord3d  Interface to OpenGL function glMultiTexCoord3d
%
% usage:  glMultiTexCoord3d( target, s, t, r )
%
% C function:  void glMultiTexCoord3d(GLenum target, GLdouble s, GLdouble t, GLdouble r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3d', target, s, t, r );

return
