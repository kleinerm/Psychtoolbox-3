function glCopyTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, x, y, width, height )

% glCopyTextureSubImage2DEXT  Interface to OpenGL function glCopyTextureSubImage2DEXT
%
% usage:  glCopyTextureSubImage2DEXT( texture, target, level, xoffset, yoffset, x, y, width, height )
%
% C function:  void glCopyTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage2DEXT', texture, target, level, xoffset, yoffset, x, y, width, height );

return
