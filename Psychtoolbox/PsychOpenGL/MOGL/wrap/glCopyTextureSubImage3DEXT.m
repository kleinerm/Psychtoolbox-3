function glCopyTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, x, y, width, height )

% glCopyTextureSubImage3DEXT  Interface to OpenGL function glCopyTextureSubImage3DEXT
%
% usage:  glCopyTextureSubImage3DEXT( texture, target, level, xoffset, yoffset, zoffset, x, y, width, height )
%
% C function:  void glCopyTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage3DEXT', texture, target, level, xoffset, yoffset, zoffset, x, y, width, height );

return
