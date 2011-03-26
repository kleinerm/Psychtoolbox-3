function glMultiTexCoord3f( target, s, t, r )

% glMultiTexCoord3f  Interface to OpenGL function glMultiTexCoord3f
%
% usage:  glMultiTexCoord3f( target, s, t, r )
%
% C function:  void glMultiTexCoord3f(GLenum target, GLfloat s, GLfloat t, GLfloat r)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord3f', target, s, t, r );

return
