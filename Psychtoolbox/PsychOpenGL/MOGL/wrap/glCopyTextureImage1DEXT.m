function glCopyTextureImage1DEXT( texture, target, level, internalformat, x, y, width, border )

% glCopyTextureImage1DEXT  Interface to OpenGL function glCopyTextureImage1DEXT
%
% usage:  glCopyTextureImage1DEXT( texture, target, level, internalformat, x, y, width, border )
%
% C function:  void glCopyTextureImage1DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureImage1DEXT', texture, target, level, internalformat, x, y, width, border );

return
