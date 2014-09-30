function glMultiTexImage3DEXT( texunit, target, level, internalformat, width, height, depth, border, format, type, pixels )

% glMultiTexImage3DEXT  Interface to OpenGL function glMultiTexImage3DEXT
%
% usage:  glMultiTexImage3DEXT( texunit, target, level, internalformat, width, height, depth, border, format, type, pixels )
%
% C function:  void glMultiTexImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=11,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexImage3DEXT', texunit, target, level, internalformat, width, height, depth, border, format, type, pixels );

return
