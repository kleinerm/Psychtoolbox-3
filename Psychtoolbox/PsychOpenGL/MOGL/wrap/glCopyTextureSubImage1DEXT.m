function glCopyTextureSubImage1DEXT( texture, target, level, xoffset, x, y, width )

% glCopyTextureSubImage1DEXT  Interface to OpenGL function glCopyTextureSubImage1DEXT
%
% usage:  glCopyTextureSubImage1DEXT( texture, target, level, xoffset, x, y, width )
%
% C function:  void glCopyTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage1DEXT', texture, target, level, xoffset, x, y, width );

return
