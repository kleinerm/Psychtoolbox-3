function glMultiTexCoord1f( target, s )

% glMultiTexCoord1f  Interface to OpenGL function glMultiTexCoord1f
%
% usage:  glMultiTexCoord1f( target, s )
%
% C function:  void glMultiTexCoord1f(GLenum target, GLfloat s)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord1f', target, s );

return
