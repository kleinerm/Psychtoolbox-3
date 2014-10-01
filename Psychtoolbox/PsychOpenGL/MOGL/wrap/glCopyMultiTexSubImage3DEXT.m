function glCopyMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, x, y, width, height )

% glCopyMultiTexSubImage3DEXT  Interface to OpenGL function glCopyMultiTexSubImage3DEXT
%
% usage:  glCopyMultiTexSubImage3DEXT( texunit, target, level, xoffset, yoffset, zoffset, x, y, width, height )
%
% C function:  void glCopyMultiTexSubImage3DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=10,
    error('invalid number of arguments');
end

moglcore( 'glCopyMultiTexSubImage3DEXT', texunit, target, level, xoffset, yoffset, zoffset, x, y, width, height );

return
