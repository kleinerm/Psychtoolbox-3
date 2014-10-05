function glColorTableSGI( target, internalformat, width, format, type, table )

% glColorTableSGI  Interface to OpenGL function glColorTableSGI
%
% usage:  glColorTableSGI( target, internalformat, width, format, type, table )
%
% C function:  void glColorTableSGI(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const void* table)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColorTableSGI', target, internalformat, width, format, type, table );

return
