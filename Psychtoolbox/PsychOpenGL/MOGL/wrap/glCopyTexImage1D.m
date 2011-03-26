function glCopyTexImage1D( target, level, internalformat, x, y, width, border )

% glCopyTexImage1D  Interface to OpenGL function glCopyTexImage1D
%
% usage:  glCopyTexImage1D( target, level, internalformat, x, y, width, border )
%
% C function:  void glCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexImage1D', target, level, internalformat, x, y, width, border );

return
