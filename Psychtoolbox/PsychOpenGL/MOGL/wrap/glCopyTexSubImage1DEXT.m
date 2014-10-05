function glCopyTexSubImage1DEXT( target, level, xoffset, x, y, width )

% glCopyTexSubImage1DEXT  Interface to OpenGL function glCopyTexSubImage1DEXT
%
% usage:  glCopyTexSubImage1DEXT( target, level, xoffset, x, y, width )
%
% C function:  void glCopyTexSubImage1DEXT(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage1DEXT', target, level, xoffset, x, y, width );

return
