function glCopyTexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, x, y, width, height )

% glCopyTexSubImage3DEXT  Interface to OpenGL function glCopyTexSubImage3DEXT
%
% usage:  glCopyTexSubImage3DEXT( target, level, xoffset, yoffset, zoffset, x, y, width, height )
%
% C function:  void glCopyTexSubImage3DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage3DEXT', target, level, xoffset, yoffset, zoffset, x, y, width, height );

return
