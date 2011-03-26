function glMultiTexCoord2i( target, s, t )

% glMultiTexCoord2i  Interface to OpenGL function glMultiTexCoord2i
%
% usage:  glMultiTexCoord2i( target, s, t )
%
% C function:  void glMultiTexCoord2i(GLenum target, GLint s, GLint t)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexCoord2i', target, s, t );

return
