function glColorTable( target, internalformat, width, format, type, table )

% glColorTable  Interface to OpenGL function glColorTable
%
% usage:  glColorTable( target, internalformat, width, format, type, table )
%
% C function:  void glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* table)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColorTable', target, internalformat, width, format, type, table );

return
