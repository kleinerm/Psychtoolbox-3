function glCopyTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, x, y, width, height )

% glCopyTextureSubImage3D  Interface to OpenGL function glCopyTextureSubImage3D
%
% usage:  glCopyTextureSubImage3D( texture, level, xoffset, yoffset, zoffset, x, y, width, height )
%
% C function:  void glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage3D', texture, level, xoffset, yoffset, zoffset, x, y, width, height );

return
