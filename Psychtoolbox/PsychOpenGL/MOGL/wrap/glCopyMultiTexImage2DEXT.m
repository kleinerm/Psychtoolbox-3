function glCopyMultiTexImage2DEXT( texunit, target, level, internalformat, x, y, width, height, border )

% glCopyMultiTexImage2DEXT  Interface to OpenGL function glCopyMultiTexImage2DEXT
%
% usage:  glCopyMultiTexImage2DEXT( texunit, target, level, internalformat, x, y, width, height, border )
%
% C function:  void glCopyMultiTexImage2DEXT(GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyMultiTexImage2DEXT', texunit, target, level, internalformat, x, y, width, height, border );

return
