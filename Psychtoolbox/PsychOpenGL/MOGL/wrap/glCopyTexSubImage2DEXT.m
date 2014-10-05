function glCopyTexSubImage2DEXT( target, level, xoffset, yoffset, x, y, width, height )

% glCopyTexSubImage2DEXT  Interface to OpenGL function glCopyTexSubImage2DEXT
%
% usage:  glCopyTexSubImage2DEXT( target, level, xoffset, yoffset, x, y, width, height )
%
% C function:  void glCopyTexSubImage2DEXT(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage2DEXT', target, level, xoffset, yoffset, x, y, width, height );

return
