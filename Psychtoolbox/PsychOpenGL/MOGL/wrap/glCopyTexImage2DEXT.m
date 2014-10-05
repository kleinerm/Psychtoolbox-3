function glCopyTexImage2DEXT( target, level, internalformat, x, y, width, height, border )

% glCopyTexImage2DEXT  Interface to OpenGL function glCopyTexImage2DEXT
%
% usage:  glCopyTexImage2DEXT( target, level, internalformat, x, y, width, height, border )
%
% C function:  void glCopyTexImage2DEXT(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexImage2DEXT', target, level, internalformat, x, y, width, height, border );

return
