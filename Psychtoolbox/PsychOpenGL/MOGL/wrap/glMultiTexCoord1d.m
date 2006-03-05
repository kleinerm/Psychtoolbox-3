function glMultiTexCoord1d( target, s )

% glMultiTexCoord1d  Interface to OpenGL function glMultiTexCoord1d
%
% usage:  glMultiTexCoord1d( target, s )
%
% C function:  void glMultiTexCoord1d(GLenum target, GLdouble s)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1d', target, s );

return
