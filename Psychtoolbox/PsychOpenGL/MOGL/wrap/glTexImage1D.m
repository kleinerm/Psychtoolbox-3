function glTexImage1D( target, level, internalformat, width, border, format, type, pixels )

% glTexImage1D  Interface to OpenGL function glTexImage1D
%
% usage:  glTexImage1D( target, level, internalformat, width, border, format, type, pixels )
%
% C function:  void glTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexImage1D', target, level, internalformat, width, border, format, type, pixels );

return
