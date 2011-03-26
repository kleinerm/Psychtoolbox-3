function glCopyTexImage2D( target, level, internalformat, x, y, width, height, border )

% glCopyTexImage2D  Interface to OpenGL function glCopyTexImage2D
%
% usage:  glCopyTexImage2D( target, level, internalformat, x, y, width, height, border )
%
% C function:  void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexImage2D', target, level, internalformat, x, y, width, height, border );

return
