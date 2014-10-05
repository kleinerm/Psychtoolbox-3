function glCopyColorTableSGI( target, internalformat, x, y, width )

% glCopyColorTableSGI  Interface to OpenGL function glCopyColorTableSGI
%
% usage:  glCopyColorTableSGI( target, internalformat, x, y, width )
%
% C function:  void glCopyColorTableSGI(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyColorTableSGI', target, internalformat, x, y, width );

return
