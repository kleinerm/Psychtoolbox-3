function pixels = glGetTexImage( target, level, format, type )

% glGetTexImage  Interface to OpenGL function glGetTexImage
%
% usage:  pixels = glGetTexImage( target, level, format, type )
%
% C function:  void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetTexImage', target, level, format, type, pixels );

return
