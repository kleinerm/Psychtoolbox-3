function glSeparableFilter2DEXT( target, internalformat, width, height, format, type, row, column )

% glSeparableFilter2DEXT  Interface to OpenGL function glSeparableFilter2DEXT
%
% usage:  glSeparableFilter2DEXT( target, internalformat, width, height, format, type, row, column )
%
% C function:  void glSeparableFilter2DEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* row, const void* column)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glSeparableFilter2DEXT', target, internalformat, width, height, format, type, row, column );

return
