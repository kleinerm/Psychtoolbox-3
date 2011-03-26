function glMultiTexCoord2f( target, s, t )

% glMultiTexCoord2f  Interface to OpenGL function glMultiTexCoord2f
%
% usage:  glMultiTexCoord2f( target, s, t )
%
% C function:  void glMultiTexCoord2f(GLenum target, GLfloat s, GLfloat t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2f', target, s, t );

return
