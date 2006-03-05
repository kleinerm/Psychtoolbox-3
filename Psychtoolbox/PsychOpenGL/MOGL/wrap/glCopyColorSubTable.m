function glCopyColorSubTable( target, start, x, y, width )

% glCopyColorSubTable  Interface to OpenGL function glCopyColorSubTable
%
% usage:  glCopyColorSubTable( target, start, x, y, width )
%
% C function:  void glCopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyColorSubTable', target, start, x, y, width );

return
