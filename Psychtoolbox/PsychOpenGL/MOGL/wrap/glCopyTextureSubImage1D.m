function glCopyTextureSubImage1D( texture, level, xoffset, x, y, width )

% glCopyTextureSubImage1D  Interface to OpenGL function glCopyTextureSubImage1D
%
% usage:  glCopyTextureSubImage1D( texture, level, xoffset, x, y, width )
%
% C function:  void glCopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCopyTextureSubImage1D', texture, level, xoffset, x, y, width );

return
