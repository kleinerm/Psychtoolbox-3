function glCopyTextureImage2DEXT( texture, target, level, internalformat, x, y, width, height, border )

% glCopyTextureImage2DEXT  Interface to OpenGL function glCopyTextureImage2DEXT
%
% usage:  glCopyTextureImage2DEXT( texture, target, level, internalformat, x, y, width, height, border )
%
% C function:  void glCopyTextureImage2DEXT(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureImage2DEXT', texture, target, level, internalformat, x, y, width, height, border );

return
