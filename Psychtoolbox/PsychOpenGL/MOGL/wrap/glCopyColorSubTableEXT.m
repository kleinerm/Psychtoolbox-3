function glCopyColorSubTableEXT( target, start, x, y, width )

% glCopyColorSubTableEXT  Interface to OpenGL function glCopyColorSubTableEXT
%
% usage:  glCopyColorSubTableEXT( target, start, x, y, width )
%
% C function:  void glCopyColorSubTableEXT(GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glCopyColorSubTableEXT', target, start, x, y, width );

return
