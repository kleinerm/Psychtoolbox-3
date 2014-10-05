function glCopyMultiTexSubImage1DEXT( texunit, target, level, xoffset, x, y, width )

% glCopyMultiTexSubImage1DEXT  Interface to OpenGL function glCopyMultiTexSubImage1DEXT
%
% usage:  glCopyMultiTexSubImage1DEXT( texunit, target, level, xoffset, x, y, width )
%
% C function:  void glCopyMultiTexSubImage1DEXT(GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glCopyMultiTexSubImage1DEXT', texunit, target, level, xoffset, x, y, width );

return
