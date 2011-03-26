function glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height )

% glCopyTexSubImage2D  Interface to OpenGL function glCopyTexSubImage2D
%
% usage:  glCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height )
%
% C function:  void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glCopyTexSubImage2D', target, level, xoffset, yoffset, x, y, width, height );

return
