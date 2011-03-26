function glCopyTexSubImage1D( target, level, xoffset, x, y, width )

% glCopyTexSubImage1D  Interface to OpenGL function glCopyTexSubImage1D
%
% usage:  glCopyTexSubImage1D( target, level, xoffset, x, y, width )
%
% C function:  void glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage1D', target, level, xoffset, x, y, width );

return
