function glCopyMultiTexImage1DEXT( texunit, target, level, internalformat, x, y, width, border )

% glCopyMultiTexImage1DEXT  Interface to OpenGL function glCopyMultiTexImage1DEXT
%
% usage:  glCopyMultiTexImage1DEXT( texunit, target, level, internalformat, x, y, width, border )
%
% C function:  void glCopyMultiTexImage1DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyMultiTexImage1DEXT', texunit, target, level, internalformat, x, y, width, border );

return
