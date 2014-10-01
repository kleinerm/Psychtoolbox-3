function glMultiTexImage2DEXT( texunit, target, level, internalformat, width, height, border, format, type, pixels )

% glMultiTexImage2DEXT  Interface to OpenGL function glMultiTexImage2DEXT
%
% usage:  glMultiTexImage2DEXT( texunit, target, level, internalformat, width, height, border, format, type, pixels )
%
% C function:  void glMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glMultiTexImage2DEXT', texunit, target, level, internalformat, width, height, border, format, type, pixels );

return
