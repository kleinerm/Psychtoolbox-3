function glMultiTexCoord1d( target, s )

% glMultiTexCoord1d  Interface to OpenGL function glMultiTexCoord1d
%
% usage:  glMultiTexCoord1d( target, s )
%
% C function:  void glMultiTexCoord1d(GLenum target, GLdouble s)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1d', target, s );

return
