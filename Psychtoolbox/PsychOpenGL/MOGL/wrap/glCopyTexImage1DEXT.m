function glCopyTexImage1DEXT( target, level, internalformat, x, y, width, border )

% glCopyTexImage1DEXT  Interface to OpenGL function glCopyTexImage1DEXT
%
% usage:  glCopyTexImage1DEXT( target, level, internalformat, x, y, width, border )
%
% C function:  void glCopyTexImage1DEXT(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexImage1DEXT', target, level, internalformat, x, y, width, border );

return
