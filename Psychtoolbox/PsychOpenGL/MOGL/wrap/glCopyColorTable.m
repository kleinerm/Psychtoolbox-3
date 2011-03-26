function glCopyColorTable( target, internalformat, x, y, width )

% glCopyColorTable  Interface to OpenGL function glCopyColorTable
%
% usage:  glCopyColorTable( target, internalformat, x, y, width )
%
% C function:  void glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyColorTable', target, internalformat, x, y, width );

return
