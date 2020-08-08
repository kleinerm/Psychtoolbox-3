function glDeleteCommandListsNV( n, lists )

% glDeleteCommandListsNV  Interface to OpenGL function glDeleteCommandListsNV
%
% usage:  glDeleteCommandListsNV( n, lists )
%
% C function:  void glDeleteCommandListsNV(GLsizei n, const GLuint* lists)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteCommandListsNV', n, uint32(lists) );

return
