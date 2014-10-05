function glMultiTexImage1DEXT( texunit, target, level, internalformat, width, border, format, type, pixels )

% glMultiTexImage1DEXT  Interface to OpenGL function glMultiTexImage1DEXT
%
% usage:  glMultiTexImage1DEXT( texunit, target, level, internalformat, width, border, format, type, pixels )
%
% C function:  void glMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexImage1DEXT', texunit, target, level, internalformat, width, border, format, type, pixels );

return
