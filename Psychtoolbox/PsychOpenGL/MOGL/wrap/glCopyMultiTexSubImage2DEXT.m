function glCopyMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, x, y, width, height )

% glCopyMultiTexSubImage2DEXT  Interface to OpenGL function glCopyMultiTexSubImage2DEXT
%
% usage:  glCopyMultiTexSubImage2DEXT( texunit, target, level, xoffset, yoffset, x, y, width, height )
%
% C function:  void glCopyMultiTexSubImage2DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=9,
    error('invalid number of arguments');
end

moglcore( 'glCopyMultiTexSubImage2DEXT', texunit, target, level, xoffset, yoffset, x, y, width, height );

return
