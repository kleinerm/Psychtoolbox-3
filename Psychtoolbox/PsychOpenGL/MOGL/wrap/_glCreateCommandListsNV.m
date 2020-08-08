function lists = glCreateCommandListsNV( n )

% glCreateCommandListsNV  Interface to OpenGL function glCreateCommandListsNV
%
% usage:  lists = glCreateCommandListsNV( n )
%
% C function:  void glCreateCommandListsNV(GLsizei n, GLuint* lists)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

lists = uint32(0);

moglcore( 'glCreateCommandListsNV', n, lists );

return
