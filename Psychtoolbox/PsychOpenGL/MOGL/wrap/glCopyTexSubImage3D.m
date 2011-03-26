function glCopyTexSubImage3D( target, level, xoffset, yoffset, zoffset, x, y, width, height )

% glCopyTexSubImage3D  Interface to OpenGL function glCopyTexSubImage3D
%
% usage:  glCopyTexSubImage3D( target, level, xoffset, yoffset, zoffset, x, y, width, height )
%
% C function:  void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage3D', target, level, xoffset, yoffset, zoffset, x, y, width, height );

return
