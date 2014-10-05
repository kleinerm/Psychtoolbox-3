function glTexImage3DEXT( target, level, internalformat, width, height, depth, border, format, type, pixels )

% glTexImage3DEXT  Interface to OpenGL function glTexImage3DEXT
%
% usage:  glTexImage3DEXT( target, level, internalformat, width, height, depth, border, format, type, pixels )
%
% C function:  void glTexImage3DEXT(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glTexImage3DEXT', target, level, internalformat, width, height, depth, border, format, type, pixels );

return
