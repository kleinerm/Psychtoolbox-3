function glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels )

% glTexImage2D  Interface to OpenGL function glTexImage2D
%
% usage:  glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels )
%
% C function:  void glTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glTexImage2D', target, level, internalformat, width, height, border, format, type, pixels );

return
