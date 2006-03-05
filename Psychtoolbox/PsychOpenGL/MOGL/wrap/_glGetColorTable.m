function table = glGetColorTable( target, format, type )

% glGetColorTable  Interface to OpenGL function glGetColorTable
%
% usage:  table = glGetColorTable( target, format, type )
%
% C function:  void glGetColorTable(GLenum target, GLenum format, GLenum type, GLvoid* table)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

table = (0);

moglcore( 'glGetColorTable', target, format, type, table );

return
