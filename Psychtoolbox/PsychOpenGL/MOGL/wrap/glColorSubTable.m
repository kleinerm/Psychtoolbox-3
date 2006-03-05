function glColorSubTable( target, start, count, format, type, data )

% glColorSubTable  Interface to OpenGL function glColorSubTable
%
% usage:  glColorSubTable( target, start, count, format, type, data )
%
% C function:  void glColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColorSubTable', target, start, count, format, type, data );

return
