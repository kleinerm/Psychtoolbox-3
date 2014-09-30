function glCopyTextureSubImage2D( texture, level, xoffset, yoffset, x, y, width, height )

% glCopyTextureSubImage2D  Interface to OpenGL function glCopyTextureSubImage2D
%
% usage:  glCopyTextureSubImage2D( texture, level, xoffset, yoffset, x, y, width, height )
%
% C function:  void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage2D', texture, level, xoffset, yoffset, x, y, width, height );

return
