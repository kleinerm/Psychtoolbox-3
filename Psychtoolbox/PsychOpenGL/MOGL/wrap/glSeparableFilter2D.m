function glSeparableFilter2D( target, internalformat, width, height, format, type, row, column )

% glSeparableFilter2D  Interface to OpenGL function glSeparableFilter2D
%
% usage:  glSeparableFilter2D( target, internalformat, width, height, format, type, row, column )
%
% C function:  void glSeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* row, const GLvoid* column)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glSeparableFilter2D', target, internalformat, width, height, format, type, row, column );

return
