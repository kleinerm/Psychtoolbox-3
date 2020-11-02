function states = glCreateStatesNV( n )

% glCreateStatesNV  Interface to OpenGL function glCreateStatesNV
%
% usage:  states = glCreateStatesNV( n )
%
% C function:  void glCreateStatesNV(GLsizei n, GLuint* states)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

states = uint32(0);

moglcore( 'glCreateStatesNV', n, states );

return
